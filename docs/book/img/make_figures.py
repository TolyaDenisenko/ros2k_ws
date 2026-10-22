#!/usr/bin/env python3
"""Рисунки для глав 8–13 книги (docs/book/img/*.png).

Данные: настоящий кадр облака камеры гриппера из симуляции «рука на столе»
(поза наблюдения, 8 объектов) — img/data/frame_observe.npz (80 тыс. точек в
link_base, единицы 0.1 мм), и настоящие конфиги проекта (объекты, зоны).
Алгоритмы — те же функции, что в нодах (src/xarm_grasping/xarm_grasping/perception.py,
scene_manager.py), поэтому рисунки показывают именно то, что делает код.

    python3 docs/book/img/make_figures.py        # из корня workspace
"""
import importlib.util
import math
import os
import sys

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
import yaml  # noqa: E402
from matplotlib.patches import Circle, Polygon, Rectangle  # noqa: E402
from scipy.spatial import ConvexHull  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
WS = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(WS, 'src', 'xarm_grasping'))
from xarm_grasping import perception as P  # noqa: E402

plt.rcParams.update({'font.size': 10, 'figure.dpi': 110, 'axes.grid': True,
                     'grid.alpha': 0.3})
OBJ_CFG = os.path.join(WS, 'src/r2_robot/cart_xarm_bringup/config/arm_table_objects.yaml')
ZONES_CFG = os.path.join(WS, 'src/xarm_grasping/config/place_zones.yaml')
RULES_CFG = os.path.join(WS, 'src/xarm_grasping/config/objects.yaml')


def load_frame():
    d = np.load(os.path.join(HERE, 'data', 'frame_observe.npz'))
    return d['base_01mm'].astype(float) * 1e-4


def objects_truth():
    with open(OBJ_CFG) as f:
        return yaml.safe_load(f)['objects']


def nearest_name(xy, objs):
    return min(objs, key=lambda o: math.dist(o['xy'], xy))['name']


def save(fig, name):
    fig.tight_layout()
    fig.savefig(os.path.join(HERE, name), bbox_inches='tight')
    plt.close(fig)
    print('  ', name)


# ---------------------------------------------------------------- 1. шум и прореживание
def fig_voxel_bias(pts):
    """Высоты точек на верхней грани pick_box и на столе: все точки, voxelize, decimate."""
    def region(p, c, r):
        return p[np.hypot(p[:, 0] - c[0], p[:, 1] - c[1]) < r]
    fig, axs = plt.subplots(1, 2, figsize=(11, 3.8))
    for ax, (title, c, r, true_z) in zip(axs, [
            ('Стол (истинная высота 0)', (0.22, -0.30), 0.04, 0.0),
            ('Верх pick_box (истинная высота 5 см)', (0.40, 0.10), 0.02, 0.05)]):
        raw = region(pts, c, r)[:, 2]
        vox = region(P.voxelize(pts, 0.01), c, r)[:, 2]
        bins = np.linspace(true_z - 0.04, true_z + 0.04, 41)
        dec = region(P.decimate(pts, 60000), c, r)[:, 2]
        ax.hist(raw * 100, bins=bins * 100, density=True, alpha=0.5,
                label=f'все точки: σ {np.std(raw) * 1000:.1f} мм')
        ax.hist(vox * 100, bins=bins * 100, density=True, alpha=0.5,
                label=f'voxelize 1 см: σ {np.std(vox) * 1000:.1f} мм')
        ax.hist(dec * 100, bins=bins * 100, density=True, histtype='step', lw=1.5,
                color='k', label=f'decimate (в коде): σ {np.std(dec) * 1000:.1f} мм')
        ax.axvline(true_z * 100, color='k', ls='--', lw=1, label='истина')
        ax.set_title(title)
        ax.set_xlabel('высота, см')
        ax.set_ylabel('плотность')
        ax.legend(fontsize=8)
    fig.suptitle('Шум глубины и прореживание: «один воксель — одна точка» уравнивает центр '
                 'распределения и его хвосты', fontsize=10)
    save(fig, 'fig08_voxel_bias.png')


# ---------------------------------------------------------------- 2. стол
def fig_table_fit(pts):
    coef, sigma, n = P.fit_table(pts, 0.0)
    dz = P.height_above(pts, coef)
    near = dz[np.abs(dz) < 0.05]
    tol = max(0.01, 3 * sigma)
    fig, ax = plt.subplots(figsize=(8, 3.6))
    ax.hist(near * 100, bins=100, color='tab:gray')
    for x, lab in [(sigma, 'σ'), (-sigma, '-σ'), (tol, 'порог 3σ'), (-tol, '')]:
        ax.axvline(x * 100, color='tab:red' if 'порог' in lab or not lab else 'tab:blue',
                   ls='--', lw=1)
        if lab:
            ax.text(x * 100, ax.get_ylim()[1] * 0.9, ' ' + lab, fontsize=8)
    ax.set_xlabel('высота над найденной плоскостью стола, см')
    ax.set_ylabel('точек')
    ax.set_title(f'Плоскость стола: z = {coef[0]:+.4f}·x {coef[1]:+.4f}·y {coef[2]:+.4f};  '
                 f'σ = {sigma * 1000:.1f} мм; всё выше {tol * 100:.1f} см — «над столом»')
    save(fig, 'fig09_table_fit.png')
    return coef, sigma, tol


# ---------------------------------------------------------------- 3. сегментация
def fig_segmentation(pts, coef, tol, objs, rules):
    dz = P.height_above(pts, coef)
    clusters = P.segment(pts, coef, tol)
    fig, axs = plt.subplots(1, 3, figsize=(15, 5.2))
    m = (dz > -0.03) & (dz < 0.16)
    sc = axs[0].scatter(pts[m, 1], pts[m, 0], c=dz[m] * 100, s=0.3, cmap='viridis',
                        vmin=-1, vmax=13)
    fig.colorbar(sc, ax=axs[0], label='высота над столом, см')
    axs[0].set_title('1. Облако в link_base (вид сверху)')
    above = m & (dz > tol)
    axs[1].scatter(pts[m, 1], pts[m, 0], c='lightgray', s=0.2)
    axs[1].scatter(pts[above, 1], pts[above, 0], c='tab:red', s=0.3)
    axs[1].set_title(f'2. Точки выше порога {tol * 100:.1f} см (красные):\n'
                     'одиночные — шум стола, сплошные пятна — объекты')
    axs[2].scatter(pts[m, 1], pts[m, 0], c='lightgray', s=0.2)
    colors = plt.cm.tab10(np.linspace(0, 1, 10))
    for i, c in enumerate(clusters):
        f = P.fit_shape(c, coef)
        if f is None:
            continue
        label, conf = P.classify(f, rules)
        axs[2].scatter(c[:, 1], c[:, 0], s=0.6, color=colors[i % 10])
        ctr = f['center']
        if f['shape'] == P.SHAPE_BOX:
            L, W, _ = f['dims']
            yaw = f['yaw']
            cs = np.array([[L / 2, W / 2], [-L / 2, W / 2], [-L / 2, -W / 2], [L / 2, -W / 2]])
            R = np.array([[math.cos(yaw), -math.sin(yaw)], [math.sin(yaw), math.cos(yaw)]])
            poly = cs @ R.T + ctr[:2]
            axs[2].add_patch(Polygon(poly[:, ::-1], fill=False, ec='k', lw=1.2))
        else:
            axs[2].add_patch(Circle((ctr[1], ctr[0]), f['dims'][1], fill=False, ec='k', lw=1.2))
        axs[2].text(ctr[1], ctr[0] + 0.05, f'{label}\n{conf:.0%}', fontsize=7, ha='center')
    axs[2].set_title('3. Кластеры, подобранная форма и класс')
    for ax in axs:
        ax.set_aspect('equal')
        ax.set_xlabel('y, м')
        ax.set_ylabel('x, м')
        ax.invert_xaxis()                 # y влево — как смотрим на стол от базы руки
    save(fig, 'fig10_segmentation.png')


# ---------------------------------------------------------------- 4. форма
def fig_shape_fit(pts, coef, tol, objs):
    clusters = P.segment(pts, coef, tol)
    pick = {}
    for c in clusters:
        name = nearest_name(c[:, :2].mean(axis=0), objs)
        pick[name] = c
    fig, axs = plt.subplots(1, 2, figsize=(10, 4.6))
    for ax, name in zip(axs, ['box_flat', 'cyl_wide']):
        c = pick[name]
        dz = P.height_above(c, coef)
        h95 = np.percentile(dz, 95)
        h = np.median(dz[dz > 0.8 * h95])
        xy = c[dz > 0.5 * h][:, :2]
        hull = xy[ConvexHull(xy).vertices]
        ctr_b, (L, W), yaw = P._min_area_rect(xy)
        ctr_c, r = P._min_circle(xy)
        A = ConvexHull(xy).volume
        ax.scatter(xy[:, 0] * 100, xy[:, 1] * 100, s=1, c='tab:gray', label='точки верха')
        ax.add_patch(Polygon(hull * 100, fill=False, ec='tab:green', lw=2,
                             label=f'выпуклая оболочка, S={A * 1e4:.0f} см²'))
        cs = np.array([[L / 2, W / 2], [-L / 2, W / 2], [-L / 2, -W / 2], [L / 2, -W / 2]])
        R = np.array([[math.cos(yaw), -math.sin(yaw)], [math.sin(yaw), math.cos(yaw)]])
        ax.add_patch(Polygon((cs @ R.T + ctr_b) * 100, fill=False, ec='tab:blue', lw=1.5,
                             ls='--', label=f'прямоугольник {L * 100:.1f}×{W * 100:.1f}: '
                                            f'заполнен на {A / (L * W):.2f}'))
        ax.add_patch(Circle(ctr_c * 100, r * 100, fill=False, ec='tab:red', lw=1.5, ls=':',
                            label=f'круг r={r * 100:.1f}: заполнен на '
                                  f'{A / (math.pi * r * r):.2f}'))
        ax.set_aspect('equal')
        ax.set_title(f'{name}: разница {A / (math.pi * r * r) - A / (L * W):+.2f} '
                     f'→ {"цилиндр" if A / (math.pi * r * r) - A / (L * W) > P.CYLINDER_MARGIN else "коробка"}')
        ax.set_xlabel('x, см')
        ax.set_ylabel('y, см')
        ax.legend(fontsize=7, loc='lower left')
        ax.autoscale_view()
        ax.margins(0.25)
    save(fig, 'fig11_shape_fit.png')


def fig_fill_metric(pts, coef, tol, objs):
    res = {}
    rng = np.random.default_rng(0)
    for k in range(20):                      # 20 случайных подвыборок кадра
        sub = pts[rng.choice(len(pts), 60000, replace=False)]
        for c in P.segment(sub, coef, tol):
            name = nearest_name(c[:, :2].mean(axis=0), objs)
            dz = P.height_above(c, coef)
            h95 = np.percentile(dz, 95)
            h = np.median(dz[dz > 0.8 * h95])
            xy = c[dz > 0.5 * h][:, :2]
            d = np.linalg.norm(xy - np.median(xy, axis=0), axis=1)
            xy = xy[d <= np.quantile(d, 0.99)]
            _, (L, W), _ = P._min_area_rect(xy)
            _, r = P._min_circle(xy)
            A = ConvexHull(xy).volume
            res.setdefault(name, []).append(A / (math.pi * r * r) - A / (L * W))
    order = [o['name'] for o in objs]
    fig, ax = plt.subplots(figsize=(10, 3.8))
    for i, n in enumerate(order):
        v = res.get(n, [])
        is_cyl = next(o for o in objs if o['name'] == n)['shape'] == 'cylinder'
        ax.scatter(np.full(len(v), i) + rng.uniform(-0.15, 0.15, len(v)), v, s=12,
                   color='tab:orange' if is_cyl else 'tab:blue')
    ax.axhline(P.CYLINDER_MARGIN, color='k', ls='--', lw=1)
    ax.text(len(order) - 0.5, P.CYLINDER_MARGIN + 0.01, f'порог {P.CYLINDER_MARGIN}',
            ha='right', fontsize=8)
    ax.set_xticks(range(len(order)))
    ax.set_xticklabels(order, rotation=20)
    ax.set_ylabel('заполнение круга − заполнение\nпрямоугольника')
    ax.set_title('Признак «цилиндр или коробка» на 20 подвыборках настоящего кадра '
                 '(синие — коробки, оранжевые — цилиндры)')
    save(fig, 'fig12_fill_metric.png')


# ---------------------------------------------------------------- 5. сцена и зоны
def obstacle_patch(o, **kw):
    if o['shape'] == 'box':
        X, Y, _ = o['size']
        yaw = o.get('yaw', 0.0)
        cs = np.array([[X / 2, Y / 2], [-X / 2, Y / 2], [-X / 2, -Y / 2], [X / 2, -Y / 2]])
        R = np.array([[math.cos(yaw), -math.sin(yaw)], [math.sin(yaw), math.cos(yaw)]])
        p = cs @ R.T + o['xy']
        return Polygon(p[:, ::-1], **kw)
    return Circle((o['xy'][1], o['xy'][0]), o['size'][0], **kw)


def fig_scene_layout(objs):
    with open(ZONES_CFG) as f:
        zones = yaml.safe_load(f)['zones']
    fig, ax = plt.subplots(figsize=(8, 7))
    ax.add_patch(Rectangle((-0.6, -0.2), 1.2, 1.0, color='burlywood', alpha=0.35,
                           label='стол 1.0×1.2 м'))
    for name, z in zones.items():
        (cx, cy), (sx, sy) = z['center'], z['size']
        ax.add_patch(Rectangle((cy - sy / 2, cx - sx / 2), sy, sx, color=z['color'], alpha=0.35))
        ax.text(cy, cx + sx / 2 + 0.015, name, ha='center', fontsize=8)
    for o in objs:
        ax.add_patch(obstacle_patch(o, color='tab:green', alpha=0.8))
        ax.text(o['xy'][1], o['xy'][0] - 0.06, o['name'], ha='center', fontsize=7)
    ax.add_patch(Circle((0, 0), 0.06, color='tab:orange', label='база руки (link_base)'))
    for r, ls in [(0.55, '--'), (0.70, ':')]:
        ax.add_patch(Circle((0, 0), r, fill=False, ls=ls, ec='k', lw=0.8))
        ax.text(-r * 0.7, r * 0.72, f'{r} м', fontsize=7)
    # поле зрения камеры из позы наблюдения: камера ~(0.36, 0, 0.47) над столом,
    # ширина кадра (HFOV 90°) — вдоль Y, высота (~64°) — вдоль X
    hz = 0.47
    hx, hy = hz * math.tan(math.radians(32)), hz * math.tan(math.radians(45))
    ax.add_patch(Rectangle((-hy, 0.36 - hx), 2 * hy, 2 * hx, fill=False, ec='tab:red',
                           lw=1.2, ls='--', label='кадр камеры из позы наблюдения'))
    ax.plot(0, 0.30, 'r+', ms=14, mew=2, label='присоска в позе наблюдения (0.30, 0, 0.40)')
    ax.set_xlim(0.65, -0.65)
    ax.set_ylim(-0.25, 0.85)
    ax.set_aspect('equal')
    ax.set_xlabel('y, м  (влево от базы — положительный)')
    ax.set_ylabel('x, м  (вперёд от базы)')
    ax.set_title('Сцена «рука на столе» сверху (link_base)')
    ax.legend(fontsize=7, loc='upper left')
    save(fig, 'fig13_scene_layout.png')


def fig_camera_side():
    """Геометрия по gripper_camera.xacro: link_eef (фланец) на 0.126 выше кончика
    присоски; камера под фланцем (z_eef 0.003..0.053), сбоку (x_eef 0.046..0.0755),
    объектив — нижняя грань, т.е. 0.126 - 0.053 = 0.073 над кончиком присоски."""
    fig, ax = plt.subplots(figsize=(9, 5))
    ax.axhline(0, color='burlywood', lw=6)
    ax.text(0.66, 0.012, 'стол', fontsize=8)

    def tool(x, ztcp, lab, lab_dx):
        flange = ztcp + 0.126
        ax.add_patch(Rectangle((x - 0.0375, ztcp), 0.075, 0.126, fc='lightgray', ec='k'))
        ax.add_patch(Rectangle((x - 0.0375, flange), 0.075, 0.003, fc='steelblue', ec='k'))
        ax.add_patch(Rectangle((x + 0.046, flange - 0.053), 0.0295, 0.050,
                               fc='tab:orange', ec='k'))
        lens_x, lens_z = x + 0.061, ztcp + 0.073
        ax.plot(x, ztcp, 'ko', ms=3)
        ax.plot(lens_x, lens_z, 'r^', ms=5)
        for a in (-32, 32):
            ax.plot([lens_x, lens_x + math.tan(math.radians(a)) * lens_z], [lens_z, 0],
                    color='tab:red', lw=0.8, ls='--')
        ax.add_patch(matplotlib.patches.Wedge((lens_x, lens_z), 0.25, 238, 302,
                                              color='tab:red', alpha=0.12))
        ax.text(x + lab_dx, flange + 0.03, lab, ha='center', fontsize=8)
        return lens_z

    z_obs = tool(0.10, 0.40, f'поза наблюдения: присоска 0.40 м,\nобъектив {0.40 + 0.073:.2f} м '
                 f'над столом', 0.03)
    tool(0.48, 0.055, 'у объекта: объектив\n0.13 м над столом', 0.0)
    ax.add_patch(Rectangle((0.44, 0), 0.08, 0.05, color='tab:green'))
    ax.annotate('', xy=(0.60, 0.0), xytext=(0.60, 0.128),
                arrowprops=dict(arrowstyle='<->', color='tab:red'))
    ax.text(0.61, 0.04, '0.13 м < 0.25 м:\nстол в мёртвой\nзоне камеры', fontsize=8,
            color='tab:red')
    ax.set_xlim(-0.05, 0.8)
    ax.set_ylim(-0.02, 0.68)
    ax.set_aspect('equal')
    ax.set_xlabel('x, м')
    ax.set_ylabel('z над столом, м')
    ax.set_title('Камера под фланцем, сбоку от присоски (серое — присоска, синее — кронштейн,\n'
                 'оранжевое — камера, ▲ — объектив, розовое — мёртвая зона 0.25 м)', fontsize=9)
    save(fig, 'fig14_camera_side.png')


def fig_place_search():
    spec = importlib.util.spec_from_file_location(
        'sm', os.path.join(WS, 'src/xarm_grasping/scripts/scene_manager.py'))
    # scene_manager импортирует ROS-сообщения; для рисунка нужна только геометрия
    src = open(spec.origin).read()
    geo = src[src.index('def yaw_of'):src.index('class SceneManager')]
    ns = {'math': math, 'np': np, 'DetectedObject': None}
    exec(geo.replace('o: DetectedObject', 'o'), ns)
    gap = ns['gap']
    with open(ZONES_CFG) as f:
        zc = yaml.safe_load(f)
    z = zc['zones']['zone_cylinders']
    clear, step = zc['clearance'], zc['search_step']
    (cx, cy), (sx, sy) = z['center'], z['size']
    placed = [('circle', np.array([0.17, -0.31]), 0.053),
              ('circle', np.array([0.31, -0.30]), 0.045)]
    r = 0.028                                 # ставим cyl_small (r 2.5 см + погрешность)
    fig, ax = plt.subplots(figsize=(8, 4.6))
    ax.add_patch(Rectangle((cx - sx / 2, cy - sy / 2), sx, sy, color=z['color'], alpha=0.25))
    free, busy = [], []
    for x in np.arange(cx - sx / 2 + r, cx + sx / 2 - r + 1e-9, step):
        for y in np.arange(cy - sy / 2 + r, cy + sy / 2 - r + 1e-9, step):
            fp = ('circle', np.array([x, y]), r)
            (free if all(gap(fp, p) > clear for p in placed) else busy).append((x, y))
    free, busy = np.array(free), np.array(busy)
    ax.scatter(busy[:, 0], busy[:, 1], s=4, c='tab:red', label='занято (ближе зазора)')
    ax.scatter(free[:, 0], free[:, 1], s=4, c='tab:green', label='свободно')
    cost = (free[:, 0] - (cx - sx / 2)) + 0.3 * np.abs(np.abs(free[:, 1]) - (abs(cy) - sy / 2))
    best = free[np.argmin(cost)]
    for i, p in enumerate(placed):
        ax.add_patch(Circle(p[1], p[2], color='tab:gray', alpha=0.8,
                            label='уже стоят' if i == 0 else None))
        ax.add_patch(Circle(p[1], p[2] + clear + r, fill=False, ls='--', ec='gray',
                            label='сюда центр нового нельзя\n(r соседа + зазор + r нового)'
                            if i == 0 else None))
    ax.add_patch(Circle(best, r, color='tab:orange', label='выбранное место'))
    ax.set_aspect('equal')
    ax.set_xlabel('x, м (от базы)')
    ax.set_ylabel('y, м')
    ax.set_title(f'Поиск места в zone_cylinders для цилиндра r={r * 100:.1f} см: сетка '
                 f'{step * 100:.0f} см, зазор {clear * 100:.0f} см, укладка от ближнего края')
    ax.legend(fontsize=8, loc='lower right')
    save(fig, 'fig15_place_search.png')


def main():
    pts = load_frame()
    objs = objects_truth()
    with open(RULES_CFG) as f:
        rules = yaml.safe_load(f)['objects']
    print('рисунки:')
    fig_voxel_bias(pts)
    coef, sigma, tol = fig_table_fit(pts)
    fig_segmentation(pts, coef, tol, objs, rules)
    fig_shape_fit(pts, coef, tol, objs)
    fig_fill_metric(pts, coef, tol, objs)
    fig_scene_layout(objs)
    fig_camera_side()
    fig_place_search()


if __name__ == '__main__':
    main()
