#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
lidar_blackout_filter — вырезает из скана лидара сектора, закрытые стойками
тележки (симуляция).

На железе это делает драйвер ydlidar_ros2_driver параметром blackout_angles
(quasi_ros_c1: quasi_cart_bringup/param/ydlidar.yaml, для отдельных моделей —
<robot_model>.quasi_common.yaml). В gz драйвера нет, поэтому то же правило
применяем здесь: лучи внутри секторов -> inf (как invalid_range_is_inf: true).
Стойки неподвижны относительно лидара, так что сектора постоянные.

Интерфейс
  вход   /scan_raw  (sensor_msgs/LaserScan) — мост gz
  выход  /scan      (sensor_msgs/LaserScan)

Параметры
  blackout_angles  [a0, a1, b0, b1, ...] — пары границ секторов, рад, в кадре
                   лидара, отсчёт 0..2pi (формат драйвера ydlidar)
"""
import math

import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import LaserScan


class LidarBlackoutFilter(Node):
    def __init__(self):
        super().__init__('lidar_blackout_filter')
        angles = self.declare_parameter('blackout_angles', [0.0]).value
        if len(angles) % 2:
            raise ValueError('blackout_angles: нужно чётное число значений (пары границ)')
        self.sectors = [(angles[i], angles[i + 1]) for i in range(0, len(angles), 2)]
        self.mask = None  # индексы лучей в секторах; считаются по первому скану
        self.mask_key = None
        # reliable, как у моста gz: подходит и reliable (RViz), и best-effort подписчикам
        self.pub = self.create_publisher(LaserScan, 'scan', 10)
        self.create_subscription(LaserScan, 'scan_raw', self.on_scan, qos_profile_sensor_data)
        self.get_logger().info('сектора blackout, град: ' + ', '.join(
            f'{math.degrees(a):.1f}..{math.degrees(b):.1f}' for a, b in self.sectors))

    def blacked_out(self, a):
        a %= 2.0 * math.pi
        return any(lo <= a <= hi for lo, hi in self.sectors)

    def on_scan(self, msg):
        key = (msg.angle_min, msg.angle_increment, len(msg.ranges))
        if key != self.mask_key:
            self.mask = [i for i in range(len(msg.ranges))
                         if self.blacked_out(msg.angle_min + i * msg.angle_increment)]
            self.mask_key = key
        ranges = list(msg.ranges)
        for i in self.mask:
            ranges[i] = math.inf
        msg.ranges = ranges
        if msg.intensities:
            intensities = list(msg.intensities)
            for i in self.mask:
                intensities[i] = 0.0
            msg.intensities = intensities
        self.pub.publish(msg)


def main():
    rclpy.init()
    node = LidarBlackoutFilter()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
