#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
test_pick_by_name — проверка pick_by_name.py БЕЗ симуляции: подменяет /objects (два куба, пластина) и
сервер /pick_place, шлёт команды в /pick_command и печатает ответы /pick_status.

  ROS_DOMAIN_ID=80 python3 ~/ros2k_ws/src/xarm_grasping/scripts/pick_by_name.py &
  ROS_DOMAIN_ID=80 python3 test_pick_by_name.py
Код возврата 0 — все проверки прошли.
"""
import sys
import time

import rclpy
from rclpy.action import ActionServer
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from std_msgs.msg import String
from xarm_grasping_msgs.action import PickPlace
from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray


def obj(i, label, x, y):
    o = DetectedObject()
    o.id, o.label, o.detector_label, o.detector_score = i, label, label, 0.9
    o.pose.position.x, o.pose.position.y = x, y
    o.grasp_pose.position.x, o.grasp_pose.position.y, o.grasp_pose.position.z = x, y, 0.06
    return o


class T(Node):
    def __init__(self):
        super().__init__('test_pick_by_name')
        lat = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL,
                         reliability=ReliabilityPolicy.RELIABLE)
        self.pub_obj = self.create_publisher(DetectedObjectArray, '/objects', lat)
        self.pub_cmd = self.create_publisher(String, '/pick_command', 10)
        self.status = []
        self.create_subscription(String, '/pick_status', lambda m: self.status.append(m.data), lat)
        self.goals = []
        ActionServer(self, PickPlace, '/pick_place', self.on_goal)

    def on_goal(self, gh):
        self.goals.append(gh.request.object_id)
        gh.succeed()
        r = PickPlace.Result()
        r.success, r.message = True, 'ok'
        return r

    def say(self, text, wait=1.0):
        n = len(self.status)
        self.pub_cmd.publish(String(data=text))
        t = time.time()
        while time.time() - t < wait:
            time.sleep(0.05)
        return self.status[n:]


def main():
    rclpy.init()
    t = T()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(t)
    import threading
    threading.Thread(target=ex.spin, daemon=True).start()
    time.sleep(2)
    msg = DetectedObjectArray()
    # cube_0 стоит правее (y=0.10), cube_1 левее (y=-0.10): номера идут от меньшего y
    msg.objects = [obj('cube_0', 'cube', 0.40, 0.10), obj('cube_1', 'cube', 0.45, -0.10),
                   obj('rect_plate_0', 'rect_plate', 0.25, 0.0)]
    t.pub_obj.publish(msg)
    time.sleep(1)
    fails = []

    def check(name, cond, got):
        print(('OK   ' if cond else 'FAIL ') + name + '  ->  ' + str(got))
        if not cond:
            fails.append(name)

    r = t.say('пластина');            check('одна пластина — берёт сразу', t.goals == ['rect_plate_0'], (r, t.goals))
    time.sleep(1)
    t.goals.clear()
    r = t.say('куб 2');               check('«куб 2» = второй по порядку (y больше) = cube_0', t.goals == ['cube_0'], (r, t.goals))
    time.sleep(1)
    t.goals.clear()
    r = t.say('cube 1');              check('«cube 1» (синоним) = cube_1', t.goals == ['cube_1'], t.goals)
    time.sleep(1)
    t.goals.clear()
    r = t.say('куб 5');               check('номера нет — отказ', not t.goals and 'только 2' in ' '.join(r), r)
    r = t.say('шар');                 check('неизвестное название', 'не понял' in ' '.join(r), r)
    r = t.say('куб');                 check('два куба — вопрос «этот?»', 'этот?' in ' '.join(r) and not t.goals, r)
    r = t.say('нет');                 check('«нет» — следующий куб', 'куб 2 из 2' in ' '.join(r), r)
    r = t.say('нет');                 check('«нет» на последнем — больше нет', 'больше таких' in ' '.join(r), r)
    r = t.say('куб');                 r = t.say('да', 1.5)
    check('«да» на первом — берёт cube_1', t.goals == ['cube_1'], (r, t.goals))
    time.sleep(1)
    r = t.say('куб');                 r = t.say('стоп')
    check('«стоп» отменяет диалог', 'отменено' in ' '.join(r), r)
    r = t.say('список');              check('список', 'куб — 2' in ' '.join(r) and 'пластина — 1' in ' '.join(r), r)
    print('\nИтог:', 'все проверки прошли' if not fails else f'провалено: {fails}')
    rclpy.shutdown()
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
