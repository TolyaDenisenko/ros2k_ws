#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
fake_xarm_vacuum — подмена драйвера xarm_api для проверки vacuum_gripper_xarm.py БЕЗ руки.

Поднимает сервисы /xarm/set_vacuum_gripper и /xarm/get_vacuum_gripper. Состояние датчика
(-1 выключен / 0 включён, предмета нет / 1 предмет взят) управляется параметром `object`:
  ros2 param set /fake_xarm_vacuum object true     # «подложили предмет» — при включённом
                                                   # вакууме датчик станет 1
  ros2 param set /fake_xarm_vacuum object false    # «предмет убрали»

Запуск (отдельный ROS_DOMAIN_ID, чтобы не мешать настоящей системе):
  ROS_DOMAIN_ID=78 python3 fake_xarm_vacuum.py
  ROS_DOMAIN_ID=78 python3 ../../../xarm_grasping/scripts/vacuum_gripper_xarm.py
  ROS_DOMAIN_ID=78 ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
"""
import rclpy
from rclpy.node import Node
from xarm_msgs.srv import GetInt16, VacuumGripperCtrl


class Fake(Node):
    def __init__(self):
        super().__init__('fake_xarm_vacuum')
        self.declare_parameter('object', False)
        self.on = False
        self.create_service(VacuumGripperCtrl, '/xarm/set_vacuum_gripper', self.set)
        self.create_service(GetInt16, '/xarm/get_vacuum_gripper', self.get)

    def set(self, req, res):
        self.on = req.on
        self.get_logger().info(f'set_vacuum_gripper on={req.on}')
        res.ret, res.message = 0, 'ok'
        return res

    def get(self, req, res):
        obj = self.get_parameter('object').value
        res.ret, res.message = 0, 'ok'
        res.data = -1 if not self.on else (1 if obj else 0)
        return res


def main():
    rclpy.init()
    rclpy.spin(Fake())


if __name__ == '__main__':
    main()
