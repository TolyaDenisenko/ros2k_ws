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

Запуск (подробно — docs/book/MODEL_TRAINING.md):
  # окно 1
  ros2 launch cart_xarm_bringup arm_table_gz.launch.py use_moveit:=false headless:=true \
      use_rviz:=false gripper_camera_preview:=true
  # окно 2 (подождать ~40 с, пока появится камера)
  python3 capture_dataset.py --out ~/.local/share/quasi/training/datasets/sim_tabletop_v1 --count 20
"""
import argparse
import json
import math
import os
import random
import subprocess
import time

import cv2
import numpy as np
import rclpy
import yaml
from cv_bridge import CvBridge
from rclpy.node import Node
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
        out.append(dict(name=o['name'], shape=o['shape'], size=o['size'], radius=r, height=h,
                        sx=sx, sy=sy, sz=sz))
    return out


def sample_layout(objs, rng, margin=0.015):
    """Случайная расстановка без пересечений; часть объектов (3..N) на виду, остальные спрятаны."""
    k = rng.randint(3, len(objs))
    visible = set(rng.sample([o['name'] for o in objs], k))
    placed, layout = [], {}
    hide_slots = [(0.15 + 0.08 * i) for i in range(len(objs))]
    for o in sorted(objs, key=lambda o: -o['radius']):
        if o['name'] not in visible:
            slot = hide_slots.pop(0)
            layout[o['name']] = (slot, HIDE_Y if rng.random() < 0.5 else -HIDE_Y, rng.uniform(-math.pi, math.pi), False)
            continue
        for _ in range(300):
            x, y = rng.uniform(*VIEW_X), rng.uniform(*VIEW_Y)
            if all(math.hypot(x - px, y - py) > o['radius'] + pr + margin for px, py, pr in placed):
                placed.append((x, y, o['radius']))
                layout[o['name']] = (x, y, rng.uniform(-math.pi, math.pi), True)
                break
        else:
            layout[o['name']] = (None, None, None, False)       # не поместился — пропустим кадр
    return layout


def gz_set_pose_start(world, name, x, y, z, yaw):
    """Запускает `gz service set_pose` в фоне (процесс на объект; их запускают разом — так быстрее)."""
    req = (f'name: "{name}", position: {{x: {x:.4f}, y: {y:.4f}, z: {z:.4f}}}, '
           f'orientation: {{x: 0, y: 0, z: {math.sin(yaw / 2):.6f}, w: {math.cos(yaw / 2):.6f}}}')
    return subprocess.Popen(['gz', 'service', '-s', f'/world/{world}/set_pose', '--reqtype', 'gz.msgs.Pose',
                             '--reptype', 'gz.msgs.Boolean', '--timeout', '3000', '--req', req],
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)


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
    ap.add_argument('--preview', type=int, default=12, help='сколько кадров с рамками сохранить для проверки')
    a = ap.parse_args()
    out = os.path.expanduser(a.out)
    for d in ('images/train', 'images/val', 'annotations', 'preview'):
        os.makedirs(os.path.join(out, d), exist_ok=True)
    objs = load_objects(a.objects)
    names = [o['name'] for o in objs]
    cat = {n: i + 1 for i, n in enumerate(names)}
    rng = random.Random(a.seed)
    rclpy.init()
    node = Capture(a)
    t0 = time.time()
    while (node.k is None or node.img is None) and time.time() - t0 < 60:
        node.spin_for(0.2)
    if node.k is None or node.img is None:
        raise SystemExit('нет кадра или camera_info: запущена ли симуляция (arm_table_gz)?')
    coco = {s: dict(images=[], annotations=[], categories=[dict(id=cat[n], name=n) for n in names])
            for s in ('train', 'val')}
    ann_id, done, skipped, failures = 1, 0, 0, 0
    prev_img, frozen = None, 0
    n_val = int(a.count * a.val_fraction)

    def dump():
        for sp in ('train', 'val'):
            with open(os.path.join(out, 'annotations', f'{sp}.json'), 'w') as fh:
                json.dump(coco[sp], fh)
    while done < a.count and rclpy.ok():
        layout = sample_layout(objs, rng)
        if any(v[0] is None for v in layout.values()):
            skipped += 1
            continue
        def zc(o):
            return TABLE_TOP_WORLD_Z + (o['sz'] if o['shape'] != 'cylinder' else o['height']) / 2 + 0.002
        pending = list(objs)
        for attempt in range(4):                 # неудавшиеся объекты повторяем (бывает при загруженной машине)
            procs = [(o, gz_set_pose_start(a.world, o['name'], layout[o['name']][0], layout[o['name']][1],
                                           zc(o), layout[o['name']][2])) for o in pending]
            pending = [o for o, p in procs if 'true' not in p.communicate()[0].lower()]
            if not pending:
                break
            time.sleep(0.3)
        ok = not pending
        if not ok:
            failures += 1
            print(f'set_pose не прошёл (бывает при высокой нагрузке; подряд {failures}) — пропускаю кадр', flush=True)
            if failures >= 8:
                print('слишком много сбоев подряд: проверьте имя мира (--world) и GZ_PARTITION — прерываю')
                break
            time.sleep(1.0)
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
        anns = []
        for o in objs:
            x, y, yaw, vis = layout[o['name']]
            if not vis:
                continue
            box = project_box(object_points(o, x, y, yaw), R, T, node.k, a.min_visible)
            if box is not None:
                anns.append((o['name'], box))
        # проверочные кадры вперемешку с обучающими (равномерно), а не подряд в начале
        split = 'val' if int((done + 1) * a.val_fraction) > int(done * a.val_fraction) else 'train'
        iid = done + 1
        fn = f'{iid:06d}.jpg'
        cv2.imwrite(os.path.join(out, 'images', split, fn), img, [cv2.IMWRITE_JPEG_QUALITY, 95])
        H, W = img.shape[:2]
        coco[split]['images'].append(dict(id=iid, file_name=fn, width=W, height=H))
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
            cv2.imwrite(os.path.join(out, 'preview', fn), pv)
        done += 1
        print(f'кадр {done}/{a.count} ({split}): рамок {len(anns)}', flush=True)
        if done % 50 == 0:
            dump()
    dump()
    with open(os.path.join(out, 'class_names.txt'), 'w') as f:
        f.write('\n'.join(names) + '\n')
    with open(os.path.join(out, 'meta.json'), 'w') as f:
        json.dump(dict(seed=a.seed, count=done, val=len(coco['val']['images']), skipped_layouts=skipped, camera_k=node.k,
                       date=time.strftime('%Y-%m-%d %H:%M'), objects_yaml=os.path.abspath(a.objects),
                       view_x=VIEW_X, view_y=VIEW_Y), f, indent=1)
    print(f'готово: {done} кадров ({len(coco["val"]["images"])} val), рамок: train {len(coco["train"]["annotations"])}, '
          f'val {len(coco["val"]["annotations"])} -> {out}')


if __name__ == '__main__':
    main()
