#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
show_objects — печатает таблицу объектов на столе: что нашло облако точек и что сказала нейросеть.

Зачем: `ros2 topic echo /objects` выводит всё одной огромной строкой, в ней ничего не найти.
Этот скрипт читает /objects один раз и печатает по строке на предмет:

  id              — имя предмета, как его назвало облако точек (по размерам) + номер
  размеры         — длина × ширина × высота, см (цилиндр: высота, радиус)
  положение       — x, y в метрах в системе руки (link_base)
  по картинке     — название от нейросети (detector_label) и уверенность 0..1

Запуск (в терминале, где выполнено `source ~/ros2k_ws/install/setup.bash`, и тот же ROS_DOMAIN_ID,
что у симуляции):

  python3 ~/ros2k_ws/src/r2_robot/cart_xarm_bringup/tools/show_objects.py            # один раз
  python3 ~/ros2k_ws/src/r2_robot/cart_xarm_bringup/tools/show_objects.py --watch    # обновлять

Код возврата 0 — объекты получены; 1 — за --timeout секунд ничего не пришло.
"""
import argparse
import sys
import time

import rclpy
from rclpy.node import Node
from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray


def fmt(o):
    d = list(o.dimensions)
    if o.shape == DetectedObject.SHAPE_CYLINDER and len(d) >= 2:
        size = f'цилиндр h={d[0] * 100:4.1f} r={d[1] * 100:4.1f}'
    elif len(d) >= 3:
        size = f'{d[0] * 100:4.1f} × {d[1] * 100:4.1f} × {d[2] * 100:4.1f}'
    else:
        size = '?'
    pos = f'({o.pose.position.x:5.2f}, {o.pose.position.y:5.2f})'
    pic = f'{o.detector_label:15s} {o.detector_score:4.2f}' if o.detector_label else '— (нейросеть не нашла)'
    return f'{o.id:18s} {size:24s} {pos:16s} {pic}'


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--watch', action='store_true', help='обновлять таблицу, пока не нажмёте Ctrl+C')
    ap.add_argument('--timeout', type=float, default=40.0, help='сколько секунд ждать первых данных')
    ap.add_argument('--topic', default='/objects')
    a = ap.parse_args()
    rclpy.init()
    node = Node('show_objects')
    last = {}
    node.create_subscription(DetectedObjectArray, a.topic, lambda m: last.update(msg=m), 1)
    head = f'{"id (по облаку)":18s} {"размеры, см":24s} {"x, y, м":16s} по картинке: название и уверенность'
    t0 = time.time()
    shown = None
    while rclpy.ok():
        rclpy.spin_once(node, timeout_sec=0.3)
        m = last.get('msg')
        if m is not None and m is not shown:
            shown = m
            print('\n' + head + '\n' + '-' * len(head))
            for o in sorted(m.objects, key=lambda o: o.id):
                print(fmt(o))
            named = sum(1 for o in m.objects if o.detector_label)
            print(f'предметов: {len(m.objects)}, названо нейросетью: {named}', flush=True)
            if not a.watch:
                return 0
        if m is None and time.time() - t0 > a.timeout:
            print(f'За {a.timeout:.0f} с на {a.topic} ничего не пришло. Симуляция уже запущена и прогрелась '
                  '(1–2 минуты)? Тот же ROS_DOMAIN_ID? Выполнено `source ~/ros2k_ws/install/setup.bash`?')
            return 1
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        pass
