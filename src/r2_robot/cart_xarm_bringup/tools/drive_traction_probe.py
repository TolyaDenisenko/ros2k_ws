#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
drive_traction_probe — проверка колёсного привода: подаёт команды скорости напрямую в /cmd_vel и
сравнивает путь тележки с оборотами колёс (проскальзывание) и наклоном. Так я нашёл, что на STL-сетках
колёс тележка подпрыгивала (проскальзывание 37–40% вперёд), а на цилиндрах — 0%.

Нужна запущенная сим тележки (без Nav2 удобнее, чтобы никто не перебивал команды):
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py headless:=true use_rviz:=false use_moveit:=false \
      manipulation_nodes:=false world:=$(ros2 pkg prefix cart_xarm_bringup)/share/cart_xarm_bringup/worlds/cart_nav.world \
      spawn_x:=-3.0 spawn_y:=-0.5
  python3 drive_traction_probe.py
Норма: проскальзывание ≈ 0%, тангаж/крен < 1° (только по прямой: при повороте на месте или упоре в
стену «путь» центра ≈ 0, и процент ничего не значит). Тележка едет по комнате: места хватает на
~6 м вперёд от спавна (-3, -0.5), дальше стена.
"""
import argparse
import math
import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node
from sensor_msgs.msg import JointState
from tf2_msgs.msg import TFMessage

PHASES = [(0.2, 0.0, 6, 'вперёд 0.2 м/с'), (-0.2, 0.0, 6, 'назад 0.2 м/с'),
          (0.4, 0.0, 6, 'вперёд 0.4 м/с'), (-0.4, 0.0, 5, 'назад 0.4 м/с')]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--radius', type=float, default=0.065, help='радиус ведущего колеса, м')
    ap.add_argument('--phases', default='', help='"vx,wz,секунд;..." вместо стандартных')
    a = ap.parse_args()
    phases = ([tuple(float(v) for v in p.split(',')) + (f'{p}',) for p in a.phases.split(';') if p]
              if a.phases else PHASES)
    rclpy.init()
    n = Node('drive_traction_probe')
    log, wheels = [], {}

    def on_pose(m):
        for t in m.transforms:
            if t.child_frame_id.endswith('cart_xarm6'):
                s = t.header.stamp.sec + t.header.stamp.nanosec * 1e-9
                q = t.transform.rotation
                pitch = math.degrees(math.asin(max(-1.0, min(1.0, 2 * (q.w * q.y - q.z * q.x)))))
                roll = math.degrees(math.atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y)))
                log.append((s, t.transform.translation.x, t.transform.translation.y, pitch, roll))
    n.create_subscription(TFMessage, '/gz/robot_pose', on_pose, 50)
    n.create_subscription(JointState, '/cart/wheel_joint_states', lambda m: wheels.update(zip(m.name, m.position)), 10)
    pub = n.create_publisher(Twist, '/cmd_vel', 10)
    t = time.time()
    while (not log or not wheels) and time.time() - t < 40:
        rclpy.spin_once(n, timeout_sec=0.1)
    if not log or not wheels:
        raise SystemExit('нет /gz/robot_pose или /cart/wheel_joint_states: запущена ли сим тележки?')
    time.sleep(1)
    print(f'{"манёвр":22s} {"путь, м":>8s} {"по колёсам":>11s} {"проскальз.":>10s} {"тангаж":>14s} {"крен":>6s}')
    for vx, wz, dur, name in phases:
        log.clear()
        w0 = dict(wheels)
        t0 = time.time()
        while time.time() - t0 < dur:
            m = Twist()
            m.linear.x, m.angular.z = float(vx), float(wz)
            pub.publish(m)
            rclpy.spin_once(n, timeout_sec=0.05)
        t2 = time.time()
        while time.time() - t2 < 2.5:
            pub.publish(Twist())
            rclpy.spin_once(n, timeout_sec=0.05)
        d = math.hypot(log[-1][1] - log[0][1], log[-1][2] - log[0][2])
        dth = sum(abs(wheels[k] - w0[k]) for k in ('drive_wheel_left_joint', 'drive_wheel_right_joint')) / 2
        by_wheels = dth * a.radius
        slip = 100 * (1 - d / by_wheels) if by_wheels > 1e-6 else 0.0
        print(f'{name:22s} {d:8.2f} {by_wheels:11.2f} {slip:9.0f}% '
              f'{min(l[3] for l in log):+6.1f}..{max(l[3] for l in log):+5.1f}° {max(abs(l[4]) for l in log):5.1f}°',
              flush=True)


if __name__ == '__main__':
    main()
