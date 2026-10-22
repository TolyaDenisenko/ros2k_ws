# -*- coding: utf-8 -*-
"""
perception — объекты на столе по облаку точек (без ROS, numpy/scipy).

Конвейер (используется нодой object_perception.py):
  1. fit_table      — плоскость стола z = a·x + b·y + c около ожидаемой высоты,
                      σ шума глубины;
  2. segment        — точки над столом -> кластеры. Ячейка XY (voxel) считается
                      «предметом» по МЕДИАНЕ высоты её точек (хвосты шума столешницы
                      медиану не поднимают), одиночные ячейки отбрасываются
                      (объект в присоске отсекает нода — по расстоянию до TCP);
  3. fit_shape      — форма по виду сверху (как shape_extraction.cpp из R2,
                      docs/reference/quasi_ros_r2_grasp): выпуклая оболочка XY ->
                      прямоугольник минимальной площади (коробка, с поворотом) и
                      описанная окружность (цилиндр); выбор — что контур заполняет
                      лучше (CYLINDER_MARGIN);
  4. classify       — класс по правилам (форма + размеры с допуском).

Все координаты — в кадре, где Z вверх (base_frame ноды, обычно link_base).
"""
import math

import numpy as np
from scipy import ndimage
from scipy.optimize import minimize
from scipy.spatial import ConvexHull, QhullError

SHAPE_BOX = 1        # = shape_msgs/SolidPrimitive.BOX
SHAPE_CYLINDER = 3   # = shape_msgs/SolidPrimitive.CYLINDER
SHAPE_NAMES = {SHAPE_BOX: 'box', SHAPE_CYLINDER: 'cylinder'}
# Цилиндр или коробка — по тому, что контур заполняет лучше: описанный круг или
# прямоугольник. Цилиндр: круг ~0.9–1, прямоугольник π/4 ≈ 0.79; коробка наоборот.
# Признак fill_circle - fill_rect > CYLINDER_MARGIN -> цилиндр. На облаках симуляции
# (8 объектов, 18 выборок): коробки ≤ -0.06, цилиндры ≥ -0.01.
CYLINDER_MARGIN = -0.035


def voxelize(pts, voxel):
    """Один представитель на воксель — для скорости и равномерной плотности."""
    keys = np.floor(pts / voxel).astype(np.int64)
    _, idx = np.unique(keys, axis=0, return_index=True)
    return pts[idx]


def decimate(pts, max_points=60000, seed=0):
    """Случайное прореживание до max_points точек.

    НЕ voxelize: «один воксель — одна точка» при шуме глубины σ≈7 мм размазывает
    каждую поверхность на 2–3 вокселя по высоте с РАВНЫМ весом — хвосты шума
    получают вес центра, высоты объектов завышались на 1–1.3 см, σ стола — вдвое.
    Случайная выборка сохраняет распределение (проверено на облаке симуляции).
    """
    if pts.shape[0] <= max_points:
        return pts
    idx = np.random.default_rng(seed).choice(pts.shape[0], max_points, replace=False)
    return pts[idx]


def fit_table(pts, z_guess, search=0.05, iters=4):
    """Плоскость стола по точкам в окне z_guess ± search.

    Возвращает (coef (a, b, c), sigma, n_points) или None.
    """
    win = pts[np.abs(pts[:, 2] - z_guess) < search]
    if win.shape[0] < 50:
        return None
    A = np.c_[win[:, 0], win[:, 1], np.ones(win.shape[0])]
    z = win[:, 2]
    for _ in range(iters):
        coef = np.linalg.lstsq(A, z, rcond=None)[0]
        res = z - A @ coef
        keep = np.abs(res) < max(0.004, 2.0 * float(np.std(res)))
        A, z = A[keep], z[keep]
    sigma = float(np.std(z - A @ coef))
    return coef, sigma, int(z.shape[0])


def height_above(pts, coef):
    return pts[:, 2] - (coef[0] * pts[:, 0] + coef[1] * pts[:, 1] + coef[2])


def segment(pts, coef, tol, max_height=0.30, cell=0.01, min_points=20):
    """Кластеры объектов над столом.

    tol — порог «над столом» (обычно max(1 см, 3σ)).
    Возвращает список массивов (N, 3) — точки объекта с dz > tol.
    """
    dz = height_above(pts, coef)
    near = (dz > -tol) & (dz < max_height)
    w, wd = pts[near], dz[near]
    if w.shape[0] == 0:
        return []
    ij = np.floor(w[:, :2] / cell).astype(np.int64)
    ij -= ij.min(axis=0)
    shape = tuple(ij.max(axis=0) + 1)
    flat = np.ravel_multi_index(ij.T, shape)
    order = np.argsort(flat)
    cells, start, cnt = np.unique(flat[order], return_index=True, return_counts=True)
    ds = wd[order]
    med = np.array([np.median(ds[i:i + n]) for i, n in zip(start, cnt)])
    grid = np.zeros(int(np.prod(shape)), dtype=bool)
    grid[cells[med > tol]] = True
    grid = ndimage.binary_opening(grid.reshape(shape))
    labels, n = ndimage.label(ndimage.binary_dilation(grid))
    lab = labels[ij[:, 0], ij[:, 1]]

    clusters = []
    for k in range(1, n + 1):
        m = (lab == k) & (wd > tol)
        if m.sum() >= min_points:
            clusters.append(w[m])
    return clusters


def _min_area_rect(xy):
    """Прямоугольник минимальной площади по выпуклой оболочке (rotating calipers).

    Возвращает (center (2,), (length, width), yaw) — yaw вдоль длинной стороны.
    """
    hull = xy[ConvexHull(xy).vertices]
    best = None
    for i in range(len(hull)):
        e = hull[(i + 1) % len(hull)] - hull[i]
        ang = math.atan2(e[1], e[0])
        c, s = math.cos(ang), math.sin(ang)
        R = np.array([[c, s], [-s, c]])          # в систему ребра
        p = hull @ R.T
        lo, hi = p.min(axis=0), p.max(axis=0)
        area = float(np.prod(hi - lo))
        if best is None or area < best[0]:
            ctr = R.T @ ((lo + hi) / 2)
            best = (area, ctr, hi - lo, ang)
    _, ctr, size, ang = best
    if size[1] > size[0]:                       # X — длинная сторона
        size = size[::-1]
        ang += math.pi / 2
    ang = (ang + math.pi / 2) % math.pi - math.pi / 2   # yaw в [-π/2, π/2)
    return ctr, (float(size[0]), float(size[1])), ang


def _min_circle(xy):
    """Описанная окружность (минимальный радиус) по выпуклой оболочке."""
    hull = xy[ConvexHull(xy).vertices]
    f = lambda c: float(np.max(np.linalg.norm(hull - c, axis=1)))  # noqa: E731
    res = minimize(f, hull.mean(axis=0), method='Nelder-Mead',
                   options={'xatol': 1e-4, 'fatol': 1e-5})
    return res.x, f(res.x)


def fit_shape(cluster, coef, trim=0.01):
    """Коробка или цилиндр по точкам объекта.

    Высота — медиана точек верхней грани над столом.
    Для оболочки берутся точки верхней половины объекта: верхняя грань видна
    сверху целиком, а нижние точки боковых стенок шумнее.
    Возвращает dict: shape, dims (BOX: [L, W, H], CYLINDER: [H, R]),
    center (3,), yaw, top_z, height.
    """
    dz = height_above(cluster, coef)
    # высота = медиана точек верхней грани (выше 80% от 95-го процентиля):
    # сам 95-й процентиль на шумной грани завышает высоту на ~1.6σ
    h95 = float(np.percentile(dz, 95))
    h = float(np.median(dz[dz > 0.8 * h95]))
    top = cluster[dz > 0.5 * h] if (dz > 0.5 * h).sum() >= 10 else cluster
    xy = top[:, :2]
    # отрезать крайние trim точек по расстоянию от медианы — выбросы раздувают
    # оболочку; больше 1% не надо — у коробки самые дальние точки это углы
    d = np.linalg.norm(xy - np.median(xy, axis=0), axis=1)
    xy = xy[d <= np.quantile(d, 1.0 - trim)]
    try:
        ctr_b, (L, W), yaw = _min_area_rect(xy)
        ctr_c, r = _min_circle(xy)
        hull_area = ConvexHull(xy).volume        # в 2D volume — площадь
    except (QhullError, ValueError):
        return None
    z_table = lambda p: coef[0] * p[0] + coef[1] * p[1] + coef[2]  # noqa: E731
    # Сравнение «меньший объём» (как в R2) на шумном контуре переворачивалось:
    # плоская коробка 12x8 в 1 кадре из 6 выходила цилиндром r 6 см.
    fill_circle = hull_area / (math.pi * r * r)
    fill_rect = hull_area / (L * W)
    if fill_circle - fill_rect > CYLINDER_MARGIN:
        ctr = ctr_c
        res = {'shape': SHAPE_CYLINDER, 'dims': [h, r], 'yaw': 0.0}
    else:
        ctr = ctr_b
        res = {'shape': SHAPE_BOX, 'dims': [L, W, h], 'yaw': yaw}
    zt = z_table(ctr)
    res.update(center=np.array([ctr[0], ctr[1], zt + h / 2]), top_z=zt + h, height=h)
    return res


def classify(fit, rules):
    """Класс по правилам.

    rules: {label: {'shape': 'box'|'cylinder', 'dims': [...], 'tol': м}};
    dims как в fit (BOX [L, W, H], CYLINDER [H, R]). Подходит правило той же
    формы, у которого все размеры отличаются не больше tol; из подходящих —
    с наименьшим отклонением. confidence = 1 - max_отклонение / tol.
    Нет подходящего — (имя формы, 0.0).
    """
    best = (SHAPE_NAMES[fit['shape']], 0.0)
    best_err = None
    for label, r in rules.items():
        if r['shape'] != SHAPE_NAMES[fit['shape']]:
            continue
        err = float(np.max(np.abs(np.array(fit['dims']) - np.array(r['dims']))))
        if err <= r['tol'] and (best_err is None or err < best_err):
            best_err = err
            best = (label, 1.0 - err / r['tol'])
    return best


def grasp_quat(yaw):
    """Кватернион (x, y, z, w) присоски «вниз» с поворотом yaw вокруг вертикали:
    R = Rz(yaw) · Rx(π) — ось Z инструмента смотрит в -Z (на объект)."""
    h = yaw / 2.0
    # Rz(yaw) * Rx(pi): q = (cos h, 0, 0, sin h) * (0, 1, 0, 0) [w, x, y, z]
    return (math.cos(h), math.sin(h), 0.0, 0.0)
