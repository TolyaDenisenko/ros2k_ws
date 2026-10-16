#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
grasp_generator.py — генератор вариантов захвата цилиндрического блока
вакуумным гриппером xArm (присоска на link_tcp).

Чистая библиотека (numpy/scipy только), без ROS-зависимостей, чтобы её
можно было тестировать офлайн:  python3 -m xarm_grasping.grasp_generator

Для цилиндра радиуса R и высоты H на столе (ось Z вертикальна) перебираются
типы захватов вакуумной присоской:

 1. TOP      — присоска точно в центр верхней грани, подход сверху вниз
               (tcp rpy = pi 0 0, инструмент направлен -Z). Основной вариант.
 2. SIDE     — боковые подходы к торцевой стенке цилиндра (4 azimuth-варианта),
               tcp развёрнут на 90° вокруг Z, точка контакта на середине высоты.
 3. RIM      — присоска на "ободке" верхней грани (несколько azimuth-вариантов),
               используется когда верхняя грань занята/неполная.

Каждый вариант содержит:
  pose        — [x, y, z, roll, pitch, yaw] в системе координат стола (base_link)
  pre_grasp   — та же поза, отодвинутая вдоль подхода на approach_offset
  score       — эвристическая оценка пригодности (0..1): площадь присоски vs
                площадь грани, устойчивость, доступность луча камеры
"""
from math import pi, cos, sin, atan2, sqrt

import numpy as np


def rotation_z(angle):
    c, s = cos(angle), sin(angle)
    return np.array([[c, -s, 0.0],
                     [s,  c, 0.0],
                     [0.0, 0.0, 1.0]])


def grasp_pose(position, approach_dir):
    """Собрать 4x4 позу TCP: ось Z инструмента смотрит В ОБЪЕКТ (-approach)."""
    z_axis = -np.asarray(approach_dir, dtype=float)
    z_axis /= np.linalg.norm(z_axis)
    # референс "вверх" для оси X: глобальный +Z, если подход не вертикальный
    ref = np.array([0.0, 0.0, 1.0])
    if abs(np.dot(ref, z_axis)) > 0.95:
        ref = np.array([1.0, 0.0, 0.0])
    x_axis = np.cross(ref, z_axis)
    x_axis /= np.linalg.norm(x_axis)
    y_axis = np.cross(z_axis, x_axis)
    R = np.column_stack((x_axis, y_axis, z_axis))
    T = np.eye(4)
    T[:3, :3] = R
    T[:3, 3] = position
    return T


def matrix_to_xyzrpy(T):
    from scipy.spatial.transform import Rotation
    r = Rotation.from_matrix(T[:3, :3])
    xyz = T[:3, 3]
    rpy = r.as_euler('xyz')
    return list(xyz) + list(rpy)


def generate_cylinder_grasps(center_xy=(0.35, -0.75), radius=0.04, height=0.09,
                             table_z=1.0, suction_radius=0.02,
                             approach_offset=0.10, n_side=4, n_rim=6):
    """
    Центр основания цилиндра: center_xy, низ цилиндра стоит на table_z.
    Возвращает список словарей-вариантов захвата.
    """
    cx, cy = float(center_xy[0]), float(center_xy[1])
    R, H = float(radius), float(height)
    top_z = table_z + H
    mid_z = table_z + H / 2.0

    grasps = []

    # ---------- 1. TOP: центр верхней грани ----------
    # Площадь присоски должна вписываться в грань цилиндра
    coverage = min(1.0, (suction_radius ** 2) / (R ** 2)) if R > 0 else 0.0
    top_pos = np.array([cx, cy, top_z])
    T = grasp_pose(top_pos + np.array([0, 0, approach_offset]),
                   np.array([0, 0, -1.0]))  # подход сверху вниз
    # финальная поза: присоска касается крышки; tcp над центром, ориентация -Z
    from scipy.spatial.transform import Rotation
    Tg = np.eye(4)
    Tg[:3, :3] = Rotation.from_euler('xyz', [pi, 0.0, 0.0]).as_matrix()
    Tg[:3, 3] = top_pos
    grasps.append({
        'type': 'TOP',
        'label': 'центр верхней грани (штатный вакуумный захват)',
        'pose': matrix_to_xyzrpy(Tg),
        'pre_grasp': matrix_to_xyzrpy(grasp_pose(
            top_pos + np.array([0, 0, approach_offset]),
            np.array([0, 0, -1.0]))),
        'score': round(0.9 * coverage + 0.1, 3),
        'contact_normal': [0.0, 0.0, 1.0],
    })

    # ---------- 2. SIDE: боковые подходы к стенке ----------
    for i in range(n_side):
        az = 2.0 * pi * i / n_side
        nx, ny = cos(az), sin(az)          # нормаль стенки (наружу)
        contact = np.array([cx + R * nx, cy + R * ny, mid_z])
        approach = np.array([nx, ny, 0.0])
        Tg = np.eye(4)
        Tg[:3, :3] = Rotation.from_euler('xyz', [pi / 2.0, 0.0, az]).as_matrix()
        Tg[:3, 3] = contact
        Tp = grasp_pose(contact + approach * approach_offset, approach)
        grasps.append({
            'type': 'SIDE',
            'label': f'боковой подход, azimuth={round(az * 180 / pi)}°',
            'pose': matrix_to_xyzrpy(Tg),
            'pre_grasp': matrix_to_xyzrpy(Tp),
            # боковая присоска хуже: контакт с кривой поверхностью
            'score': round(0.55 * max(0.0, 1.0 - suction_radius / R) + 0.15, 3),
            'contact_normal': [float(nx), float(ny), 0.0],
        })

    # ---------- 3. RIM: ободок верхней грани ----------
    for i in range(n_rim):
        az = 2.0 * pi * i / n_rim
        rr = max(0.0, R - suction_radius)  # центр присоски у края крышки
        contact = np.array([cx + rr * cos(az), cy + rr * sin(az), top_z])
        Tg = np.eye(4)
        Tg[:3, :3] = Rotation.from_euler('xyz', [pi, 0.0, 0.0]).as_matrix()
        Tg[:3, 3] = contact
        grasps.append({
            'type': 'RIM',
            'label': f'ободок верхней грани, azimuth={round(az * 180 / pi)}°',
            'pose': matrix_to_xyzrpy(Tg),
            'pre_grasp': matrix_to_xyzrpy(grasp_pose(
                contact + np.array([0, 0, approach_offset]),
                np.array([0, 0, -1.0]))),
            'score': round(0.5 * (rr / R if R else 0.0) + 0.2, 3),
            'contact_normal': [0.0, 0.0, 1.0],
        })

    grasps.sort(key=lambda g: g['score'], reverse=True)
    return grasps


def pick_best(grasps, camera_pos=None, occlusion_radius=None):
    """Выбрать лучший выполнимый захват (по score; опционально — видимость с камеры)."""
    visible = []
    for g in grasps:
        p = np.array(g['pose'][:3])
        if camera_pos is not None and occlusion_radius is not None:
            # простая проверка: вектор камера->точка не должен быть ниже стола
            d = p - np.asarray(camera_pos, dtype=float)
            if d[2] >= -1e-6:
                continue  # точка ниже камеры — не видна сверху
        visible.append(g)
    pool = visible if visible else grasps
    return pool[0] if pool else None


if __name__ == '__main__':
    import json
    gs = generate_cylinder_grasps()
    print(json.dumps(gs, ensure_ascii=False, indent=2))
    best = pick_best(gs)
    print('BEST:', best['type'], best['score'])
