#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
capture_dataset — собирает датасет для обучения детектора из симуляции «рука на столе».

Для каждого кадра: случайно расставляет объекты на столе (команда gz `set_pose`), ждёт, пока они
успокоятся, снимает кадр цветной камеры запястья и считает рамку каждого объекта по ИЗВЕСТНОЙ позе
и размеру (проекция углов или окружностей через camera_info и TF). Размечать вручную не нужно:
симулятор знает, где что лежит.

Результат (формат COCO — его читает обучение YOLOX):
  <out>/images/{train,val}/000123.jpg
  <out>/annotations/{train,val}.json
  <out>/class_names.txt      — классы, по строке (для openvino_detector: labels_path)
  <out>/meta.json            — параметры сбора (сид, камера, число кадров)
  <out>/preview/*.jpg        — первые кадры с нарисованными рамками: ГЛАЗАМИ проверить разметку

Что варьируется от кадра к кадру (подробно — docs/RECOGNITION_IMPROVEMENT.md):
  * РАССТАНОВКА: «свободная» (зазор ≥ 1.5 см) или «плотная» (зазор 2–12 мм: предметы почти
    касаются; доля плотных — --tight-frac). Зазоры считаются по настоящим контурам, а не по кругам.
  * ВНЕШНИЙ ВИД: случайные цвета предметов, цвет стола и пола, направление и яркость света
    (доля таких кадров — --color-frac; остальные кадры — «как в сцене по умолчанию», зелёные предметы).
    Цвета меняются командой gz `visual_config`, свет — `light_config`; класс от цвета не зависит.
  * РАКУРС (--view-frac, нужен MoveIt: use_moveit:=true): камера на руке встаёт вокруг одного из
    предметов — азимут, наклон 40–80°, расстояние 0.30–0.45 м, поворот кадра ±25° — через /compute_ik
    и /move_action (с учётом стола). Остальные кадры — из позы наблюдения, как раньше. Так модель
    видит форму с разных сторон, а не одну картинку сверху.
  * ПЕРЕКРЫТИЯ (если есть кадр глубины): точки поверхности каждого предмета сравниваются с глубиной;
    видна меньше --min-surface доли — рамка не ставится (сбоку предметы закрывают друг друга).
  Класс предмета = поле `class` в arm_table_objects.yaml, иначе имя модели без хвоста _N (cube_2 -> cube).
  Чтобы получить прежнее поведение (v3): --tight-frac 0 --color-frac 0 --view-frac 0.

Запуск (подробно — docs/MODEL_TRAINING.md):
  # окно 1
  ros2 launch cart_xarm_bringup arm_table_gz.launch.py use_moveit:=true headless:=true \
      use_rviz:=false gripper_camera_preview:=true        # use_moveit:=false — только при --view-frac 0
  # окно 2 (подождать ~40 с, пока появится камера)
  python3 capture_dataset.py --out ~/.local/share/quasi/training/datasets/sim_tabletop_v1 --count 20
"""
import argparse
import colorsys
import json
import math
import os
import random
import re
import subprocess
import time

import cv2
import numpy as np
import rclpy
import yaml
from cv_bridge import CvBridge
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from moveit_msgs.action import MoveGroup
from moveit_msgs.msg import Constraints, JointConstraint, MotionPlanRequest, MoveItErrorCodes
from moveit_msgs.srv import GetPositionIK
from rclpy.action import ActionClient
from sensor_msgs.msg import CameraInfo, Image
from tf2_ros import Buffer, TransformListener

HERE = os.path.dirname(os.path.abspath(__file__))
OBJECTS_YAML = os.path.join(HERE, '..', 'config', 'arm_table_objects.yaml')
TABLE_TOP_WORLD_Z = 0.75           # верх стола в мире Gazebo (link_base стоит на нём, z = 0)
VIEW_X = (0.15, 0.58)              # где объекты видны камере из позы наблюдения, link_base, м
VIEW_Y = (-0.33, 0.33)
HIDE_Y = 0.55                      # «спрятать» объект: на край стола, вне кадра (|y| > 0.47)


def load_objects(path):
    with open(path) as f:
        objs = yaml.safe_load(f)['objects']
    out = []
    for o in objs:
        if o['shape'] in ('box', 'pyramid'):
            sx, sy, sz = o['size']
            r, h = 0.5 * math.hypot(sx, sy), sz
        else:
            rad, h = o['size']
            sx = sy = 2 * rad
            r, sz = rad, h
        cls = o.get('class') or re.sub(r'_\d+$', '', o['name'])     # cube_2 -> cube
        out.append(dict(name=o['name'], cls=cls, shape=o['shape'], size=o['size'], radius=r, height=h,
                        sx=sx, sy=sy, sz=sz))
    return out


def footprint(o, x, y, yaw):
    """Контур предмета на столе (выпуклый многоугольник, м): прямоугольник или 24-угольник цилиндра."""
    if o['shape'] == 'cylinder':
        return [(x + o['radius'] * math.cos(2 * math.pi * i / 24), y + o['radius'] * math.sin(2 * math.pi * i / 24))
                for i in range(24)]
    c, s = math.cos(yaw), math.sin(yaw)
    return [(x + c * dx * o['sx'] - s * dy * o['sy'], y + s * dx * o['sx'] + c * dy * o['sy'])
            for dx, dy in ((-0.5, -0.5), (0.5, -0.5), (0.5, 0.5), (-0.5, 0.5))]


def poly_gap(A, B):
    """Зазор между двумя выпуклыми многоугольниками (метод разделяющих осей), м.
    > 0 — не касаются (настоящее расстояние не меньше этого числа), <= 0 — пересекаются."""
    best = -1e9
    for P in (A, B):
        for i in range(len(P)):
            x1, y1 = P[i]
            x2, y2 = P[(i + 1) % len(P)]
            nx, ny = y2 - y1, x1 - x2
            ln = math.hypot(nx, ny)
            if ln < 1e-9:
                continue
            nx, ny = nx / ln, ny / ln
            pa = [nx * px + ny * py for px, py in A]
            pb = [nx * px + ny * py for px, py in B]
            best = max(best, min(pb) - max(pa), min(pa) - max(pb))
    return best


def sample_layout(objs, rng, tight=False, gap_min=0.015, gap_tight=(0.002, 0.012)):
    """Случайная расстановка без пересечений.

    Свободная (tight=False): все видимые предметы на расстоянии не менее gap_min друг от друга.
    Плотная (tight=True): предметы жмутся друг к другу, зазор между контурами 2–12 мм: так предметы
    стоят на настоящем столе, и облако точек склеивает их в один кластер. Видимых предметов обычно все.
    Возвращает ({имя: (x, y, поворот, виден)}, минимальный зазор между видимыми, м)."""
    n = len(objs)
    k = n if (tight and rng.random() < 0.7) else rng.randint(min(3, n), n)
    visible = set(rng.sample([o['name'] for o in objs], k))
    placed, layout = [], {}                       # placed: (объект, x, y, yaw)
    hide_slots = [(0.15 + 0.08 * i) for i in range(n)]
    for o in sorted(objs, key=lambda o: -o['radius']):
        if o['name'] not in visible:
            slot = hide_slots.pop(0)
            layout[o['name']] = (slot, HIDE_Y if rng.random() < 0.5 else -HIDE_Y, rng.uniform(-math.pi, math.pi), False)
            continue
        want = rng.uniform(*gap_tight) if tight else gap_min
        ok = False
        for _ in range(400):
            yaw = rng.uniform(-math.pi, math.pi)
            if tight and placed:                   # встать рядом с уже стоящим предметом
                ao, ax, ay, _ = rng.choice(placed)
                ang = rng.uniform(0, 2 * math.pi)
                d = ao['radius'] + o['radius'] + rng.uniform(-0.02, 0.01)
                x, y = ax + d * math.cos(ang), ay + d * math.sin(ang)
                if not (VIEW_X[0] <= x <= VIEW_X[1] and VIEW_Y[0] <= y <= VIEW_Y[1]):
                    continue
            else:
                x, y = rng.uniform(*VIEW_X), rng.uniform(*VIEW_Y)
            fp = footprint(o, x, y, yaw)
            gaps = [poly_gap(fp, footprint(po, px, py, pyaw)) for po, px, py, pyaw in placed]
            g = min(gaps) if gaps else None
            if g is None:                          # первый предмет — любое место
                good = True
            elif tight:                            # зазор с ближайшим соседом в узком коридоре
                good = want <= g <= want + 0.02
            else:
                good = g >= want
            if good:
                placed.append((o, x, y, yaw))
                layout[o['name']] = (x, y, yaw, True)
                ok = True
                break
        if not ok:
            layout[o['name']] = (None, None, None, False)       # не поместился — пропустим кадр
    gaps = [poly_gap(footprint(a, ax, ay, ayaw), footprint(b, bx, by, byaw))
            for i, (a, ax, ay, ayaw) in enumerate(placed) for (b, bx, by, byaw) in placed[i + 1:]]
    return layout, (min(gaps) if gaps else None)


def gz_call_start(world, service, reqtype, req):
    """Запускает `gz service` в фоне; ответ `data: true` читают через communicate()."""
    return subprocess.Popen(['gz', 'service', '-s', f'/world/{world}/{service}', '--reqtype', reqtype,
                             '--reptype', 'gz.msgs.Boolean', '--timeout', '3000', '--req', req],
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)


def gz_set_pose_start(world, name, x, y, z, yaw):
    """Команда set_pose в фоне (процесс на объект; их запускают разом — так быстрее)."""
    req = (f'name: "{name}", position: {{x: {x:.4f}, y: {y:.4f}, z: {z:.4f}}}, '
           f'orientation: {{x: 0, y: 0, z: {math.sin(yaw / 2):.6f}, w: {math.cos(yaw / 2):.6f}}}')
    return gz_call_start(world, 'set_pose', 'gz.msgs.Pose', req)


# ---------------------------------------------------------------- внешний вид (цвета, свет)
# Исходные значения из worlds/arm_table.world (и объекты из gen_arm_table_scene.py): кадры «по умолчанию»
# возвращают сцену ровно к ним, поэтому стандартная зелёная сцена остаётся в датасете.
DEFAULT_OBJ = dict(amb=(0.1, 1.0, 0.1), dif=(0.1, 1.0, 0.1), emi=(0.0, 0.35, 0.0), spe=(0.0, 0.0, 0.0))
DEFAULT_TABLE = dict(amb=(0.6, 0.4, 0.2), dif=(0.7, 0.5, 0.3), emi=(0.0, 0.0, 0.0), spe=(0.0, 0.0, 0.0))
DEFAULT_GROUND = dict(amb=(0.4, 0.4, 0.4), dif=(0.5, 0.5, 0.5), emi=(0.0, 0.0, 0.0), spe=(0.0, 0.0, 0.0))
DEFAULT_SUN = dict(dif=(1.0, 1.0, 1.0), spe=(0.3, 0.3, 0.3), dir=(-0.5, 0.1, -0.9), shadows=True, intensity=1.0)
DEFAULT_FILL = dict(dif=(0.5, 0.5, 0.5), spe=(0.1, 0.1, 0.1), dir=(0.6, -0.3, -0.7), shadows=False, intensity=1.0)
MIN_CONTRAST = 0.40        # минимальное расстояние в RGB между цветом предмета и стола: чтобы предмет был виден


def scene_ids(world, names):
    """Номера визуальных элементов (нужны для visual_config): предметы, столешница, пол.
    Берутся из описания сцены Gazebo (сервис scene/info); у каждого запуска симуляции они свои."""
    out = subprocess.run(['gz', 'service', '-s', f'/world/{world}/scene/info', '--reqtype', 'gz.msgs.Empty',
                          '--reptype', 'gz.msgs.Scene', '--timeout', '5000', '--req', ''],
                         capture_output=True, text=True).stdout
    ids = {}
    for part in re.split(r'\nmodel \{\n', '\n' + out)[1:]:
        m = re.match(r'\s*name: "(\w+)"', part)
        if not m:
            continue
        vis = dict(re.findall(r'visual \{\s*name: "(\w+)"\s*id: (\d+)', part))
        key = {'table': 'top_visual', 'ground_plane': 'visual'}.get(m.group(1), 'visual')
        if key in vis:
            ids[m.group(1)] = int(vis[key])
    missing = [n for n in list(names) + ['table', 'ground_plane'] if n not in ids]
    if missing:
        raise SystemExit(f'в сцене Gazebo не нашлись визуальные элементы: {missing} '
                         '(запущена ли симуляция, тот ли --world и GZ_PARTITION?). Без цветов: --color-frac 0')
    return ids


def _hsv(rng, s_lo, s_hi, v_lo, v_hi):
    return colorsys.hsv_to_rgb(rng.random(), rng.uniform(s_lo, s_hi), rng.uniform(v_lo, v_hi))


def sample_appearance(rng, names):
    """Случайный внешний вид: цвет каждого предмета, столешницы, пола и два источника света.
    Цвет предмета подбирается так, чтобы он заметно отличался от стола (иначе рамка была бы
    у «невидимого» предмета). Класс предмета от цвета не зависит: цвета берутся независимо."""
    if rng.random() < 0.3:                                   # нейтральный стол: серый, белый, тёмный
        g = rng.uniform(0.2, 0.9)
        table = (g, g * rng.uniform(0.95, 1.05), g * rng.uniform(0.95, 1.05))
    else:
        table = _hsv(rng, 0.15, 0.75, 0.35, 0.95)
    gg = rng.uniform(0.15, 0.8)
    colors = {}
    for n in names:
        for _ in range(60):
            c = _hsv(rng, 0.35, 1.0, 0.45, 1.0)
            if math.dist(c, table) >= MIN_CONTRAST:
                break
        colors[n] = c
    br, tint = rng.uniform(0.55, 1.0), [rng.uniform(0.85, 1.0) for _ in range(3)]
    d = [rng.uniform(-0.9, 0.3), rng.uniform(-0.5, 0.5), rng.uniform(-1.0, -0.6)]
    ln = math.sqrt(sum(v * v for v in d))
    sun = dict(dif=tuple(br * t for t in tint), spe=(0.3, 0.3, 0.3), dir=tuple(v / ln for v in d), shadows=True,
               intensity=rng.uniform(0.7, 1.4))
    fill = dict(dif=(rng.uniform(0.2, 0.7),) * 3, spe=(0.1, 0.1, 0.1), dir=DEFAULT_FILL['dir'], shadows=False,
                intensity=1.0)
    k = lambda c, e: dict(amb=c, dif=c, emi=tuple(e * v for v in c), spe=(0.1, 0.1, 0.1))  # noqa: E731
    return dict(kind='random', objects={n: k(colors[n], 0.3) for n in names},
                table=dict(amb=table, dif=table, emi=(0, 0, 0), spe=(0, 0, 0)),
                ground=dict(amb=(gg,) * 3, dif=(gg,) * 3, emi=(0, 0, 0), spe=(0, 0, 0)), sun=sun, fill=fill)


def default_appearance(names):
    return dict(kind='default', objects={n: DEFAULT_OBJ for n in names}, table=DEFAULT_TABLE,
                ground=DEFAULT_GROUND, sun=DEFAULT_SUN, fill=DEFAULT_FILL)


def _c(name, v):
    return f'{name} {{r: {v[0]:.4f} g: {v[1]:.4f} b: {v[2]:.4f} a: 1}}'


def appearance_calls(world, ids, ap):
    """Список запусков gz service, которые приводят сцену к внешнему виду ap."""
    def vis(vid, m):
        req = f'id: {vid}, material {{{_c("ambient", m["amb"])} {_c("diffuse", m["dif"])} ' \
              f'{_c("specular", m["spe"])} {_c("emissive", m["emi"])}}}'
        return lambda: gz_call_start(world, 'visual_config', 'gz.msgs.Visual', req)

    def light(name, L):
        req = (f'name: "{name}", type: DIRECTIONAL, pose {{position {{z: 10}} orientation {{w: 1}}}}, '
               f'{_c("diffuse", L["dif"])}, {_c("specular", L["spe"])}, '
               f'direction {{x: {L["dir"][0]:.4f} y: {L["dir"][1]:.4f} z: {L["dir"][2]:.4f}}}, '
               f'cast_shadows: {"true" if L["shadows"] else "false"}, intensity: {L["intensity"]:.3f}, range: 10, '
               'attenuation_constant: 1, attenuation_linear: 1')
        return lambda: gz_call_start(world, 'light_config', 'gz.msgs.Light', req)
    calls = [vis(ids[n], m) for n, m in ap['objects'].items()]
    calls += [vis(ids['table'], ap['table']), vis(ids['ground_plane'], ap['ground']),
              light('sun', ap['sun']), light('fill', ap['fill'])]
    return calls


def run_calls(calls, attempts=4):
    """Запускает все вызовы разом, неудавшиеся повторяет. True — все ответили `true`."""
    pending = list(calls)
    for _ in range(attempts):
        procs = [(c, c()) for c in pending]
        pending = [c for c, p in procs if 'true' not in p.communicate()[0].lower()]
        if not pending:
            return True
        time.sleep(0.3)
    return False


def object_points(o, x, y, yaw, n=16):
    """Характерные точки объекта в link_base (верх стола z=0): углы коробки или окружности цилиндра."""
    c, s = math.cos(yaw), math.sin(yaw)
    pts = []
    if o['shape'] == 'pyramid':       # 4 угла основания и вершина
        for dx, dy in ((-0.5, -0.5), (-0.5, 0.5), (0.5, -0.5), (0.5, 0.5)):
            lx, ly = dx * o['sx'], dy * o['sy']
            pts.append((x + c * lx - s * ly, y + s * lx + c * ly, 0.0))
        pts.append((x, y, o['sz']))
    elif o['shape'] == 'box':
        for dx, dy, z in ((a, b, h) for a in (-0.5, 0.5) for b in (-0.5, 0.5) for h in (0.0, 1.0)):
            lx, ly = dx * o['sx'], dy * o['sy']
            pts.append((x + c * lx - s * ly, y + s * lx + c * ly, z * o['sz']))
    else:
        for i in range(n):
            a = 2 * math.pi * i / n
            for z in (0.0, o['height']):
                pts.append((x + o['radius'] * math.cos(a), y + o['radius'] * math.sin(a), z))
    return np.array(pts)


def surface_points(o, x, y, yaw, step=0.01):
    """Точки на ВИДИМЫХ сверху и сбоку гранях предмета (без дна), link_base, шаг ~1 см —
    для проверки перекрытий по глубине."""
    c, s = math.cos(yaw), math.sin(yaw)
    loc = []
    if o['shape'] == 'cylinder':
        r, h = o['radius'], o['height']
        for rr in np.arange(0, r, step):                     # верхний круг
            n = max(1, int(2 * math.pi * rr / step))
            loc += [(rr * math.cos(2 * math.pi * i / n), rr * math.sin(2 * math.pi * i / n), h) for i in range(n)]
        n = max(8, int(2 * math.pi * r / step))
        for z in np.arange(step / 2, h, step):               # боковая поверхность
            loc += [(r * math.cos(2 * math.pi * i / n), r * math.sin(2 * math.pi * i / n), z) for i in range(n)]
    else:
        sx, sy, sz = o['sx'], o['sy'], o['sz']
        gx = np.linspace(-sx / 2, sx / 2, max(2, int(sx / step) + 1))
        gy = np.linspace(-sy / 2, sy / 2, max(2, int(sy / step) + 1))
        gz = np.linspace(0.002, sz, max(2, int(sz / step) + 1))
        loc += [(u, v, sz) for u in gx for v in gy]
        loc += [(u, sy / 2 * sg, z) for u in gx for z in gz for sg in (-1, 1)]
        loc += [(sx / 2 * sg, v, z) for v in gy for z in gz for sg in (-1, 1)]
    L = np.array(loc)
    return np.stack([x + c * L[:, 0] - s * L[:, 1], y + s * L[:, 0] + c * L[:, 1], L[:, 2]], 1)


def visible_share(o, x, y, yaw, R, T, k, depth, tol=0.012):
    """Доля точек поверхности предмета, которые камера реально видит (глубина совпала), 0..1;
    считается среди точек, попавших в кадр. Невидимые грани (обратная сторона) тоже в знаменателе,
    поэтому у открыто стоящего предмета доля ~0.3–0.6, у закрытого соседом — заметно меньше."""
    fx, fy, cx, cy, W, H = k
    pc = surface_points(o, x, y, yaw) @ R.T + T
    pc = pc[pc[:, 2] > 0.05]
    u = np.round(fx * pc[:, 0] / pc[:, 2] + cx).astype(int)
    v = np.round(fy * pc[:, 1] / pc[:, 2] + cy).astype(int)
    m = (u >= 0) & (u < W) & (v >= 0) & (v < H)
    if m.sum() < 10:
        return 0.0
    d = depth[v[m], u[m]]
    ok = np.isfinite(d) & (np.abs(d - pc[m, 2]) < tol)
    return float(ok.sum()) / float(m.sum())


def look_at(cam, target, roll):
    """Поворот оптического кадра камеры (z — вперёд, x — вправо, y — вниз), смотрящей на target."""
    z = target - cam
    z /= np.linalg.norm(z)
    x = np.cross(z, [0.0, 0.0, 1.0])
    x /= np.linalg.norm(x)
    y = np.cross(z, x)
    cr, sr = math.cos(roll), math.sin(roll)
    x, y = cr * x + sr * y, -sr * x + cr * y
    return np.stack([x, y, z], 1)


def rot_quat(Rm):
    t = np.trace(Rm)
    if t > 0:
        s = math.sqrt(t + 1.0) * 2
        return ((Rm[2, 1] - Rm[1, 2]) / s, (Rm[0, 2] - Rm[2, 0]) / s, (Rm[1, 0] - Rm[0, 1]) / s, 0.25 * s)
    i = int(np.argmax(np.diag(Rm)))
    j, kk = (i + 1) % 3, (i + 2) % 3
    s = math.sqrt(1.0 + Rm[i, i] - Rm[j, j] - Rm[kk, kk]) * 2
    q = [0.0, 0.0, 0.0, (Rm[kk, j] - Rm[j, kk]) / s]
    q[i], q[j], q[kk] = 0.25 * s, (Rm[j, i] + Rm[i, j]) / s, (Rm[kk, i] + Rm[i, kk]) / s
    return tuple(q)


def quat_rot(x, y, z, w):
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


class Capture(Node):
    def __init__(self, a):
        super().__init__('capture_dataset')
        self.bridge = CvBridge()
        self.buf = Buffer()
        TransformListener(self.buf, self)
        self.img, self.img_t, self.k = None, 0.0, None
        self.create_subscription(Image, a.image_topic, self.on_img, 1)
        self.create_subscription(CameraInfo, a.camera_info, self.on_info, 1)
        self.optical, self.base = a.optical_frame, a.base_frame
        self.depth, self.depth_t = None, 0.0
        if a.depth_topic:
            self.create_subscription(Image, a.depth_topic, self.on_depth, 1)
        self.group, self.tip = a.group, a.tip_link
        self.ik = self.create_client(GetPositionIK, '/compute_ik')
        self.move = ActionClient(self, MoveGroup, '/move_action')
        self.T_tip_cam = None

    def on_depth(self, m):
        if m.encoding == '32FC1':
            self.depth = np.frombuffer(m.data, np.float32).reshape(m.height, m.step // 4)[:, :m.width].copy()
        elif m.encoding == '16UC1':
            self.depth = np.frombuffer(m.data, np.uint16).reshape(m.height, m.step // 2)[:, :m.width] * 1e-3
        self.depth_t = time.monotonic()

    def wait(self, fut, timeout):
        end = time.monotonic() + timeout
        while not fut.done() and time.monotonic() < end and rclpy.ok():
            rclpy.spin_once(self, timeout_sec=0.05)
        return fut.result() if fut.done() else None

    def goto_joints(self, names, values, scaling=0.5):
        """Рука в заданные углы через MoveIt (/move_action, с объездом стола). True — доехала."""
        if not self.move.wait_for_server(timeout_sec=5.0):
            return False
        req = MotionPlanRequest()
        req.group_name = self.group
        req.num_planning_attempts, req.allowed_planning_time = 3, 5.0
        req.max_velocity_scaling_factor = req.max_acceleration_scaling_factor = scaling
        c = Constraints()
        for n, v in zip(names, values):
            c.joint_constraints.append(JointConstraint(joint_name=n, position=float(v), tolerance_above=0.005,
                                                       tolerance_below=0.005, weight=1.0))
        req.goal_constraints = [c]
        goal = MoveGroup.Goal(request=req)
        goal.planning_options.plan_only = False
        gh = self.wait(self.move.send_goal_async(goal), 10.0)
        if gh is None or not gh.accepted:
            return False
        r = self.wait(gh.get_result_async(), 60.0)
        return r is not None and r.result.error_code.val == MoveItErrorCodes.SUCCESS

    def view_from(self, cam, target, roll):
        """Камера в точке cam (link_base), смотрит на target: IK для кончика руки и движение."""
        if self.T_tip_cam is None:
            tf = self.buf.lookup_transform(self.tip, self.optical, rclpy.time.Time())
            r, t = tf.transform.rotation, tf.transform.translation
            self.T_tip_cam = (quat_rot(r.x, r.y, r.z, r.w), np.array([t.x, t.y, t.z]))
        Rc = look_at(np.asarray(cam, float), np.asarray(target, float), roll)
        Rtc, ttc = self.T_tip_cam
        Rt = Rc @ Rtc.T                        # T_base_tip = T_base_cam * inv(T_tip_cam)
        pt = np.asarray(cam, float) - Rt @ ttc
        if not self.ik.wait_for_service(timeout_sec=5.0):
            return False
        req = GetPositionIK.Request()
        req.ik_request.group_name = self.group
        req.ik_request.ik_link_name = self.tip
        req.ik_request.avoid_collisions = True
        req.ik_request.timeout.sec = 1
        ps = PoseStamped()
        ps.header.frame_id = self.base
        ps.pose.position.x, ps.pose.position.y, ps.pose.position.z = map(float, pt)
        q = rot_quat(Rt)
        ps.pose.orientation.x, ps.pose.orientation.y, ps.pose.orientation.z, ps.pose.orientation.w = q
        req.ik_request.pose_stamped = ps
        r = self.wait(self.ik.call_async(req), 5.0)
        if r is None or r.error_code.val != MoveItErrorCodes.SUCCESS:
            return False
        js = r.solution.joint_state
        names = [n for n in js.name if n.startswith('joint')]
        vals = [js.position[js.name.index(n)] for n in names]
        return self.goto_joints(names, vals)

    def on_img(self, m):
        self.img, self.img_t = self.bridge.imgmsg_to_cv2(m, 'bgr8'), time.monotonic()

    def on_info(self, m):
        self.k = (m.k[0], m.k[4], m.k[2], m.k[5], m.width, m.height)

    def spin_for(self, sec):
        end = time.monotonic() + sec
        while time.monotonic() < end and rclpy.ok():
            rclpy.spin_once(self, timeout_sec=0.05)

    def fresh_image(self, after, timeout=10.0):
        end = time.monotonic() + timeout
        while time.monotonic() < end and rclpy.ok():
            rclpy.spin_once(self, timeout_sec=0.05)
            if self.img is not None and self.img_t > after:
                return self.img
        return None

    def cam_pose(self):
        tf = self.buf.lookup_transform(self.optical, self.base, rclpy.time.Time())
        r, t = tf.transform.rotation, tf.transform.translation
        return quat_rot(r.x, r.y, r.z, r.w), np.array([t.x, t.y, t.z])


def project_box(pts_base, R, T, k, min_visible):
    fx, fy, cx, cy, W, H = k
    pc = pts_base @ R.T + T
    if (pc[:, 2] <= 0.05).any():
        return None
    u, v = fx * pc[:, 0] / pc[:, 2] + cx, fy * pc[:, 1] / pc[:, 2] + cy
    x1, x2, y1, y2 = u.min(), u.max(), v.min(), v.max()
    full = max(1e-6, (x2 - x1) * (y2 - y1))
    cx1, cx2, cy1, cy2 = max(0, x1), min(W, x2), max(0, y1), min(H, y2)
    if cx2 <= cx1 or cy2 <= cy1:
        return None
    visible = (cx2 - cx1) * (cy2 - cy1) / full
    if visible < min_visible or (cx2 - cx1) < 6 or (cy2 - cy1) < 6:
        return None
    return cx1, cy1, cx2 - cx1, cy2 - cy1


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--out', required=True)
    ap.add_argument('--count', type=int, default=20)
    ap.add_argument('--val-fraction', type=float, default=0.15)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--settle', type=float, default=1.0, help='сколько секунд ждать после расстановки')
    ap.add_argument('--min-visible', type=float, default=0.6, help='минимальная видимая доля рамки')
    ap.add_argument('--world', default='arm_table_world')
    ap.add_argument('--objects', default=OBJECTS_YAML)
    ap.add_argument('--image-topic', default='/gripper_camera/color/image_raw')
    ap.add_argument('--camera-info', default='/gripper_camera/color/camera_info')
    ap.add_argument('--optical-frame', default='gripper_camera_color_optical_frame')
    ap.add_argument('--base-frame', default='link_base')
    ap.add_argument('--tight-frac', type=float, default=0.5,
                    help='доля кадров с ПЛОТНОЙ расстановкой (предметы почти касаются, зазор около 2–22 мм, см. docs/RECOGNITION_IMPROVEMENT.md)')
    ap.add_argument('--color-frac', type=float, default=0.75,
                    help='доля кадров со СЛУЧАЙНЫМ внешним видом (цвета, стол, пол, свет); остальные — как в сцене по умолчанию')
    ap.add_argument('--preview', type=int, default=12, help='сколько кадров с рамками сохранить для проверки')
    ap.add_argument('--view-frac', type=float, default=0.0,
                    help='доля кадров с ракурсом вокруг предмета (нужен MoveIt); 0 — все из позы наблюдения')
    ap.add_argument('--view-dist', type=float, nargs=2, default=[0.30, 0.45], help='камера — предмет, м')
    ap.add_argument('--view-elev', type=float, nargs=2, default=[40.0, 80.0], help='наклон взгляда, градусы')
    ap.add_argument('--view-roll', type=float, default=25.0, help='поворот кадра ±, градусы')
    ap.add_argument('--observe-joints', type=float, nargs=6, default=[0, -0.596, -1.013, 0, 1.609, 0],
                    help='поза наблюдения (joint1..6) — как arm_initial_positions в arm_table_gz')
    ap.add_argument('--group', default='xarm6')
    ap.add_argument('--tip-link', default='link_tcp')
    ap.add_argument('--depth-topic', default='/gripper_camera/aligned_depth/image_raw',
                    help='кадр глубины для проверки перекрытий; пусто — не проверять')
    ap.add_argument('--min-surface', type=float, default=0.12,
                    help='минимальная видимая доля точек поверхности предмета (перекрытия)')
    a = ap.parse_args()
    out = os.path.expanduser(a.out)
    for d in ('images/train', 'images/val', 'annotations', 'preview'):
        os.makedirs(os.path.join(out, d), exist_ok=True)
    objs = load_objects(a.objects)
    names = [o['name'] for o in objs]
    classes = list(dict.fromkeys(o['cls'] for o in objs))        # без повторов, в порядке файла
    cat = {c: i + 1 for i, c in enumerate(classes)}
    cls_of = {o['name']: o['cls'] for o in objs}
    rng = random.Random(a.seed)
    ids = scene_ids(a.world, names) if a.color_frac > 0 else None
    rclpy.init()
    node = Capture(a)
    t0 = time.time()
    while (node.k is None or node.img is None) and time.time() - t0 < 60:
        node.spin_for(0.2)
    if node.k is None or node.img is None:
        raise SystemExit('нет кадра или camera_info: запущена ли симуляция (arm_table_gz)?')
    coco = {s: dict(images=[], annotations=[], categories=[dict(id=cat[c], name=c) for c in classes])
            for s in ('train', 'val')}
    joint_names = [f'joint{i}' for i in range(1, 7)]
    at_observe = None             # где сейчас рука: True — поза наблюдения, False — ракурс, None — неизвестно
    if a.view_frac > 0 and not node.goto_joints(joint_names, a.observe_joints):
        raise SystemExit('рука не поехала в позу наблюдения: запущен ли MoveIt (use_moveit:=true)? '
                         'Без MoveIt — --view-frac 0')
    at_observe = True if a.view_frac > 0 else None
    ann_id, done, skipped, failures = 1, 0, 0, 0
    prev_img, frozen = None, 0
    cur_default = None            # в каком виде сейчас сцена (чтобы не слать лишние команды)
    stats = dict(tight=0, spread=0, random=0, default=0, observe=0, orbit=0, orbit_failed=0, hidden_boxes=0)
    n_val = int(a.count * a.val_fraction)

    def dump():
        for sp in ('train', 'val'):
            with open(os.path.join(out, 'annotations', f'{sp}.json'), 'w') as fh:
                json.dump(coco[sp], fh)
    while done < a.count and rclpy.ok():
        tight = rng.random() < a.tight_frac
        layout, min_gap = sample_layout(objs, rng, tight=tight)
        if any(v[0] is None for v in layout.values()):
            skipped += 1
            continue
        # РАКУРС: сначала рука (предметы ещё на старых местах — их всё равно сейчас переставим),
        # потом расстановка: предметы не должны оказаться под движущейся рукой
        view = dict(kind='observe')
        if a.view_frac > 0 and rng.random() < a.view_frac:
            vis_objs = [o for o in objs if layout[o['name']][3]]
            for _ in range(6):
                o = rng.choice(vis_objs)
                tx, ty = layout[o['name']][0], layout[o['name']][1]
                target = np.array([tx, ty, 0.5 * (o['sz'] if o['shape'] != 'cylinder' else o['height'])])
                # азимут — от направления «предмет -> основание руки»: камера между рукой и предметом
                # или сбоку (за предметом рука не достаёт)
                base_dir = math.atan2(-ty, -tx)
                az = base_dir + math.radians(rng.uniform(-100, 100))
                el = math.radians(rng.uniform(*a.view_elev))
                dist = rng.uniform(*a.view_dist)
                cam = target + dist * np.array([math.cos(el) * math.cos(az), math.cos(el) * math.sin(az), math.sin(el)])
                roll = math.radians(rng.uniform(-a.view_roll, a.view_roll))
                if node.view_from(cam, target, roll):
                    view = dict(kind='orbit', target=o['name'], az_deg=round(math.degrees(az - base_dir), 1),
                                elev_deg=round(math.degrees(el), 1), dist=round(dist, 3),
                                roll_deg=round(math.degrees(roll), 1))
                    at_observe = False
                    break
            else:
                stats['orbit_failed'] += 1
        if view['kind'] == 'observe' and at_observe is False:
            if not node.goto_joints(joint_names, a.observe_joints):
                print('рука не вернулась в позу наблюдения — пропускаю кадр', flush=True)
                at_observe = None
                continue
            at_observe = True
        want_random = a.color_frac > 0 and rng.random() < a.color_frac
        appearance = sample_appearance(rng, names) if want_random else default_appearance(names)

        def zc(o):
            return TABLE_TOP_WORLD_Z + (o['sz'] if o['shape'] != 'cylinder' else o['height']) / 2 + 0.002
        pose_calls = [(lambda o=o: gz_set_pose_start(a.world, o['name'], layout[o['name']][0], layout[o['name']][1],
                                                     zc(o), layout[o['name']][2])) for o in objs]
        calls = list(pose_calls)
        if a.color_frac > 0 and (want_random or cur_default is not True):    # вернуть умолчания или задать случайные
            calls += appearance_calls(a.world, ids, appearance)
        ok = run_calls(calls)
        if ok and a.color_frac > 0:
            cur_default = not want_random
        if not ok:
            failures += 1
            print(f'gz-команда не прошла (бывает при высокой нагрузке; подряд {failures}) — пропускаю кадр', flush=True)
            if failures >= 20:
                print('слишком много сбоев подряд: проверьте имя мира (--world) и GZ_PARTITION — прерываю')
                break
            cur_default = None if a.color_frac > 0 else cur_default
            time.sleep(3.0)
            continue
        failures = 0
        node.spin_for(a.settle)
        t_req = time.monotonic()
        img = node.fresh_image(t_req)
        if img is None:
            print('нет свежего кадра — пропускаю')
            continue
        # защита от тихого сбоя: расстановки случайные, одинаковые кадры подряд = предметы не двигаются
        # (set_pose ушёл не в ту симуляцию / кадры не обновляются) — такой датасет учить нельзя
        if prev_img is not None and np.abs(img.astype(np.int16) - prev_img.astype(np.int16)).mean() < 0.5:
            frozen += 1
            if frozen >= 5:
                print('ОШИБКА: 5 кадров подряд одинаковые, предметы не двигаются. Проверьте, что в этом '
                      'GZ_PARTITION/ROS_DOMAIN_ID запущена одна симуляция (нет старых процессов) — прерываю')
                break
        else:
            frozen = 0
        prev_img = img
        try:
            R, T = node.cam_pose()
        except Exception as e:  # noqa: BLE001
            print('нет TF камеры:', e)
            continue
        depth = node.depth if (a.depth_topic and node.depth is not None and node.depth_t > t_req) else None
        anns = []
        for o in objs:
            x, y, yaw, vis = layout[o['name']]
            if not vis:
                continue
            box = project_box(object_points(o, x, y, yaw), R, T, node.k, a.min_visible)
            if box is None:
                continue
            if depth is not None and visible_share(o, x, y, yaw, R, T, node.k, depth) < a.min_surface:
                stats['hidden_boxes'] += 1          # закрыт соседом: рамку не ставим
                continue
            anns.append((cls_of[o['name']], box))
        # проверочные кадры вперемешку с обучающими (равномерно), а не подряд в начале
        split = 'val' if int((done + 1) * a.val_fraction) > int(done * a.val_fraction) else 'train'
        iid = done + 1
        fn = f'{iid:06d}.jpg'
        cv2.imwrite(os.path.join(out, 'images', split, fn), img, [cv2.IMWRITE_JPEG_QUALITY, 95])
        H, W = img.shape[:2]
        coco[split]['images'].append(dict(id=iid, file_name=fn, width=W, height=H,
                                          layout='tight' if tight else 'spread',
                                          min_gap_mm=None if min_gap is None else round(min_gap * 1000, 1),
                                          appearance=appearance['kind'], view=view['kind'],
                                          view_info=view))
        stats['tight' if tight else 'spread'] += 1
        stats['random' if want_random else 'default'] += 1
        stats[view['kind']] += 1
        for name, (bx, by, bw, bh) in anns:
            coco[split]['annotations'].append(dict(id=ann_id, image_id=iid, category_id=cat[name],
                                                   bbox=[round(bx, 1), round(by, 1), round(bw, 1), round(bh, 1)],
                                                   area=round(bw * bh, 1), iscrowd=0))
            ann_id += 1
        if done < a.preview:
            pv = img.copy()
            for name, (bx, by, bw, bh) in anns:
                cv2.rectangle(pv, (int(bx), int(by)), (int(bx + bw), int(by + bh)), (0, 0, 255), 2)
                cv2.putText(pv, name, (int(bx), max(12, int(by) - 4)), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                            (0, 0, 255), 1, cv2.LINE_AA)
            cv2.putText(pv, f"{'tight' if tight else 'spread'} / {appearance['kind']} / {view['kind']}", (6, H - 8),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
            cv2.imwrite(os.path.join(out, 'preview', fn), pv)
        done += 1
        print(f'кадр {done}/{a.count} ({split}): рамок {len(anns)}', flush=True)
        if done % 50 == 0:
            dump()
    dump()
    with open(os.path.join(out, 'class_names.txt'), 'w') as f:
        f.write('\n'.join(classes) + '\n')
    with open(os.path.join(out, 'meta.json'), 'w') as f:
        json.dump(dict(seed=a.seed, count=done, val=len(coco['val']['images']), skipped_layouts=skipped, camera_k=node.k,
                       date=time.strftime('%Y-%m-%d %H:%M'), objects_yaml=os.path.abspath(a.objects),
                       tight_frac=a.tight_frac, color_frac=a.color_frac, view_frac=a.view_frac,
                       view_dist=a.view_dist, view_elev=a.view_elev, view_roll=a.view_roll,
                       min_surface=a.min_surface if a.depth_topic else None, classes=classes, frames=stats,
                       view_x=VIEW_X, view_y=VIEW_Y), f, indent=1)
    print(f'расстановки/вид/ракурсы: {stats}')
    print(f'готово: {done} кадров ({len(coco["val"]["images"])} val), рамок: train {len(coco["train"]["annotations"])}, '
          f'val {len(coco["val"]["annotations"])} -> {out}')


if __name__ == '__main__':
    main()
