#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
nav_tilt_probe — проверка навигации и устойчивости тележки: отправляет цели Nav2 одну за другой и
записывает наклон корпуса, положение суставов руки и состояние парковки. Именно этим я проверял, что
тележка больше не опрокидывается (раньше после цели (-1.87, 2.71) -> (3.87, -1.75) наклон доходил до 69°).

Нужна запущенная навигация (Nav2 active) и мост позы /gz/robot_pose:
  ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_rviz:=false use_octomap:=false
  python3 nav_tilt_probe.py                       # 4 цели по умолчанию (комната cart_nav.world)
  python3 nav_tilt_probe.py --goals "0.15,1.76;3.44,1.15" --limit 120
Печатает при наклоне > --warn градусов: время, крен/тангаж, положение, команду, парковку и руку.
В конце — итог по каждой цели: статус (4 = SUCCEEDED) и максимальный наклон. Норма: наклон ~0.0°.
"""
import argparse
import math
import time

import rclpy
from geometry_msgs.msg import Twist
from nav2_msgs.action import NavigateToPose
from rclpy.action import ActionClient
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from sensor_msgs.msg import JointState
from std_msgs.msg import Bool
from tf2_msgs.msg import TFMessage

DEFAULT = '0.15,1.76;3.44,1.15;-1.87,2.71;3.87,-1.75'


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--goals', default=DEFAULT, help='"x,y;x,y;..." в кадре map')
    ap.add_argument('--limit', type=float, default=150.0, help='лимит времени на цель, с')
    ap.add_argument('--warn', type=float, default=3.0, help='печатать, если наклон больше, град')
    a = ap.parse_args()
    goals = [tuple(float(v) for v in g.split(',')) for g in a.goals.split(';') if g.strip()]
    rclpy.init()
    n = Node('nav_tilt_probe')
    st = {'pose': None, 'cmd': (0.0, 0.0), 'park': None, 'j': {}}

    def on_pose(m):
        for t in m.transforms:
            if t.child_frame_id.endswith('cart_xarm6'):
                st['pose'] = t.transform
    n.create_subscription(TFMessage, '/gz/robot_pose', on_pose, 20)
    n.create_subscription(Twist, '/cmd_vel', lambda m: st.__setitem__('cmd', (m.linear.x, m.angular.z)), 10)
    n.create_subscription(JointState, '/joint_states', lambda m: st['j'].update(
        {k: v for k, v in zip(m.name, m.position) if k.startswith('joint')}), 10)
    n.create_subscription(Bool, '/arm_parking/state', lambda m: st.__setitem__('park', m.data),
                          QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL))
    ac = ActionClient(n, NavigateToPose, 'navigate_to_pose')
    if not ac.wait_for_server(timeout_sec=90):
        raise SystemExit('нет /navigate_to_pose: Nav2 не поднялся')
    t1 = time.time()
    while st['pose'] is None and time.time() - t1 < 20:
        rclpy.spin_once(n, timeout_sec=0.2)
    if st['pose'] is None:
        raise SystemExit('нет /gz/robot_pose')

    def tilt():
        q = st['pose'].rotation
        roll = math.degrees(math.atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y)))
        pitch = math.degrees(math.asin(max(-1.0, min(1.0, 2 * (q.w * q.y - q.z * q.x)))))
        return roll, pitch

    def arm():
        return ' '.join(f'{st["j"].get(f"joint{i}", float("nan")):+.2f}' for i in range(1, 7))

    summary = []
    for k, (x, y) in enumerate(goals, 1):
        g = NavigateToPose.Goal()
        g.pose.header.frame_id = 'map'
        g.pose.pose.position.x, g.pose.pose.position.y, g.pose.pose.orientation.w = x, y, 1.0
        gh = None
        for _ in range(6):
            f = ac.send_goal_async(g)
            rclpy.spin_until_future_complete(n, f, timeout_sec=20)
            if f.done() and f.result().accepted:
                gh = f.result()
                break
            time.sleep(5)
        if gh is None:
            print(f'цель {k} ({x}, {y}): не принята')
            summary.append((k, x, y, 'НЕ ПРИНЯТА', 0.0))
            continue
        res = gh.get_result_async()
        t0, last, mx = time.time(), 0.0, 0.0
        while not res.done() and time.time() - t0 < a.limit:
            rclpy.spin_once(n, timeout_sec=0.05)
            roll, pitch = tilt()
            m = max(abs(roll), abs(pitch))
            mx = max(mx, m)
            if m > a.warn and time.time() - last > 0.7:
                last = time.time()
                p = st['pose'].translation
                print(f'  цель {k} t={time.time() - t0:5.1f} НАКЛОН крен={roll:+5.1f} тангаж={pitch:+5.1f} '
                      f'x={p.x:5.2f} y={p.y:5.2f} cmd=({st["cmd"][0]:+.2f},{st["cmd"][1]:+.2f}) '
                      f'парковка={st["park"]} рука[{arm()}]', flush=True)
        status = res.result().status if res.done() else 'ТАЙМАУТ'
        print(f'цель {k} ({x}, {y}): статус {status}, макс. наклон {mx:.2f}°, рука [{arm()}]', flush=True)
        summary.append((k, x, y, status, mx))
    print('\nСВОДКА')
    for k, x, y, status, mx in summary:
        print(f'  {k}. ({x}, {y}): {"выполнена" if status == 4 else status}, наклон {mx:.2f}°')
    worst = max((s[4] for s in summary), default=0.0)
    print('ИТОГ:', 'устойчиво' if worst < a.warn else f'БЫЛ НАКЛОН до {worst:.1f}°')


if __name__ == '__main__':
    main()
