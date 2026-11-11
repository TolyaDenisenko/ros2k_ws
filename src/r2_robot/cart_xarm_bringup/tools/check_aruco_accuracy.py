#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
check_aruco_accuracy — насколько точно aruco_detector.py определяет позу метки (только симуляция).

Переставляет тележку в Gazebo в несколько поз перед столом (сервис set_pose), в каждой ждёт и
усредняет /aruco/marker_pose, и сравнивает с ПРАВДОЙ: поза метки в мире (модель aruco_dock) +
поза тележки из Gazebo + положение камеры на тележке (TF base_footprint -> оптический кадр).

  # симуляция cart_nav_gz.launch.py + aruco_detector.py уже запущены, тот же ROS_DOMAIN_ID/GZ_PARTITION
  python3 check_aruco_accuracy.py [--world cart_xarm_world_offline] [--samples 15]

Печатает таблицу: поза тележки, расстояние до метки, ошибка положения (мм) и ошибка курса (°)
— курс = поворот метки вокруг вертикали, именно он важен для ровной парковки.
"""
import argparse
import math
import re
import subprocess
import time

import numpy as np
import rclpy
from geometry_msgs.msg import PoseStamped
from rclpy.node import Node
from tf2_ros import Buffer, TransformListener


def qmat(x, y, z, w):
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def rpy_mat(r, p, y):
    cr, sr, cp, sp, cy, sy = math.cos(r), math.sin(r), math.cos(p), math.sin(p), math.cos(y), math.sin(y)
    return np.array([[cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr],
                     [sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr],
                     [-sp, cp * sr, cp * cr]])


def gz_pose(model):
    out = subprocess.run(['gz', 'model', '-m', model, '-p'], capture_output=True, text=True, timeout=20).stdout
    nums = re.findall(r'\[([-\d.e ]+)\]', out.split('Pose')[-1])
    xyz = [float(v) for v in nums[0].split()]
    rpy = [float(v) for v in nums[1].split()]
    return np.array(xyz), rpy_mat(*rpy)


def gz_set_pose(world, model, x, y, yaw):
    q = (0.0, 0.0, math.sin(yaw / 2), math.cos(yaw / 2))
    req = (f'name: "{model}", position: {{x: {x}, y: {y}, z: 0.0}}, '
           f'orientation: {{x: {q[0]}, y: {q[1]}, z: {q[2]}, w: {q[3]}}}')
    subprocess.run(['gz', 'service', '-s', f'/world/{world}/set_pose', '--reqtype', 'gz.msgs.Pose',
                    '--reptype', 'gz.msgs.Boolean', '--timeout', '5000', '--req', req],
                   capture_output=True, timeout=20)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--world', default='cart_xarm_world_offline')
    ap.add_argument('--cart', default='cart_xarm6')
    ap.add_argument('--marker', default='aruco_dock')
    ap.add_argument('--samples', type=int, default=15)
    ap.add_argument('--quick', action='store_true', help='только 3 позы')
    ap.add_argument('--verbose', action='store_true', help='печатать правду и найденное в кадре камеры')
    a = ap.parse_args()
    rclpy.init()
    n = Node('check_aruco_accuracy')
    buf = Buffer()
    TransformListener(buf, n)
    got = []
    n.create_subscription(PoseStamped, '/aruco/marker_pose', lambda m: got.append(m), 10)

    def spin(t):
        e = time.time() + t
        while time.time() < e:
            rclpy.spin_once(n, timeout_sec=0.05)

    spin(2.0)
    cam = None
    while cam is None:
        try:
            tf = buf.lookup_transform('base_footprint', 'front_camera_color_optical_frame', rclpy.time.Time())
            t, q = tf.transform.translation, tf.transform.rotation
            cam = (np.array([t.x, t.y, t.z]), qmat(q.x, q.y, q.z, q.w))
        except Exception:  # noqa: BLE001
            spin(0.5)
    m_p, m_R = gz_pose(a.marker)
    # лицевая плоскость метки — на 0.5 мм перед моделью (см. gen_aruco_marker.py); ось z метки
    # (как у детектора) смотрит из лица наружу = -x модели
    m_p = m_p + m_R @ np.array([-0.0005, 0, 0])
    m_z = m_R @ np.array([-1.0, 0, 0])
    poses = [(-1.4, 0.0, 0.0), (-1.0, 0.0, 0.0), (-2.0, 0.0, 0.0), (-3.0, 0.0, 0.0),
             (-1.4, 0.25, 0.0), (-1.4, -0.25, 0.0), (-1.4, 0.0, 0.25), (-1.4, 0.0, -0.25),
             (-2.0, 0.3, -0.2)]
    if a.quick:
        poses = poses[:3]
    print(f'{"тележка x, y, курс°":24s} {"до метки, м":>11s} {"кадров":>6s} {"ошибка, мм":>10s} '
          f'{"вбок, мм":>9s} {"курс, °":>8s}')
    for x, y, yaw in poses:
        gz_set_pose(a.world, a.cart, x, y, yaw)
        spin(3.0)
        got.clear()
        spin(max(2.0, a.samples / 10))
        c_p, c_R = gz_pose(a.cart)
        W_p = c_p + c_R @ cam[0]            # камера в мире (правда)
        W_R = c_R @ cam[1]
        truth = W_R.T @ (m_p - W_p)         # центр метки в камере (правда)
        tz = W_R.T @ m_z                    # нормаль метки в камере (правда)
        if not got:
            print(f'{x:5.2f} {y:5.2f} {math.degrees(yaw):6.1f}°        {np.linalg.norm(truth):11.2f}      0  метки не видно')
            continue
        P = np.array([[m.pose.position.x, m.pose.position.y, m.pose.position.z] for m in got])
        Z = np.array([qmat(m.pose.orientation.x, m.pose.orientation.y, m.pose.orientation.z,
                           m.pose.orientation.w)[:, 2] for m in got])
        p = P.mean(0)
        z = Z.mean(0)
        err = p - truth
        # курс: угол нормали в горизонтальной плоскости мира
        zw, tzw = W_R @ z, W_R @ tz
        dyaw = math.degrees(math.atan2(zw[1], zw[0]) - math.atan2(tzw[1], tzw[0]))
        dyaw = (dyaw + 180) % 360 - 180
        side = (W_R @ err)[1]               # ошибка вдоль y мира (вбок у стола)
        if a.verbose:
            print("   правда в камере", truth.round(3), "найдено", p.round(3))
        print(f'{x:5.2f} {y:5.2f} {math.degrees(yaw):6.1f}°        {np.linalg.norm(truth):11.2f} {len(got):6d} '
              f'{np.linalg.norm(err) * 1000:10.1f} {side * 1000:9.1f} {dyaw:8.2f}')
    rclpy.shutdown()


if __name__ == '__main__':
    main()
