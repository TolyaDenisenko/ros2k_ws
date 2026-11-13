#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
test_parking — проверка команды «паркуйся» в симуляции cart_nav_gz: вызывает /park, ждёт итог в
/park/status и сравнивает настоящую позу тележки (gz) с идеальной стоянкой.

  ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true      # дождаться Nav2
  GZ_PARTITION=<как у симуляции> python3 test_parking.py [--along 0.3 --gap 0.07 --side 1]

Идеал для cart_nav.world: метка в центре торца (x=0.444, y=0), ось стола +x, ширина 0.8.
Печатает: время, ошибку вдоль кромки и вбок (мм), курса (°), фактический зазор борт—стол (мм).
"""
import argparse
import math
import re
import subprocess
import time

import rclpy
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from std_msgs.msg import String
from std_srvs.srv import Trigger

ARM_X, HALF_WIDTH = 0.1125, 0.32


def gz_pose(model):
    out = subprocess.run(['gz', 'model', '-m', model, '-p'], capture_output=True, text=True, timeout=20).stdout
    nums = re.findall(r'\[([-\d.e ]+)\]', out.split('Pose')[-1])
    xyz = [float(v) for v in nums[0].split()]
    rpy = [float(v) for v in nums[1].split()]
    return xyz[0], xyz[1], rpy[2]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--model', default='cart_xarm6')
    ap.add_argument('--marker-x', type=float, default=0.444)
    ap.add_argument('--width', type=float, default=0.8)
    ap.add_argument('--along', type=float, default=0.3)
    ap.add_argument('--gap', type=float, default=0.07)
    ap.add_argument('--side', type=int, default=1)
    ap.add_argument('--timeout', type=float, default=300)
    a = ap.parse_args()

    rclpy.init()
    n = Node('test_parking')
    st = {'last': ''}
    lat = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)

    def on_status(m):
        if m.data != st['last']:
            print('  ', m.data, flush=True)
        st['last'] = m.data
    n.create_subscription(String, '/park/status', on_status, lat)
    cli = n.create_client(Trigger, '/park')
    if not cli.wait_for_service(timeout_sec=30):
        raise SystemExit('нет сервиса /park')
    t0 = time.time()
    f = cli.call_async(Trigger.Request())
    rclpy.spin_until_future_complete(n, f, timeout_sec=10)
    print('/park:', f.result().message if f.result() else 'ответ не дошёл за 10 с — смотрю /park/status')
    st['last'] = ''
    while time.time() - t0 < a.timeout:
        rclpy.spin_once(n, timeout_sec=0.2)
        if st['last'].startswith(('6/6', 'ошибка', 'отменено')):
            break
    dt = time.time() - t0
    x, y, yaw = gz_pose(a.model)
    s = 1 if a.side >= 0 else -1
    want_yaw = 0.0 if s > 0 else math.pi
    hx = math.cos(want_yaw)
    want_x = a.marker_x + a.along - s * ARM_X
    want_y = s * (a.width / 2 + a.gap + HALF_WIDTH)
    e_along = (x - want_x) * hx
    e_lat = (y - want_y)
    e_yaw = math.degrees(math.atan2(math.sin(yaw - want_yaw), math.cos(yaw - want_yaw)))
    gap = abs(y) - HALF_WIDTH - a.width / 2
    print(f'итог: {st["last"]}')
    print(f'gz: ({x:.3f}, {y:.3f}, {math.degrees(yaw):.1f}°), идеал ({want_x:.3f}, {want_y:.3f}); время {dt:.0f} с')
    print(f'ошибка (правда gz): вдоль {e_along * 1000:+.0f} мм, вбок {e_lat * 1000:+.0f} мм, '
          f'курс {e_yaw:+.1f}°; зазор борт—стол {gap * 1000:.0f} мм')
    rclpy.shutdown()


if __name__ == '__main__':
    main()
