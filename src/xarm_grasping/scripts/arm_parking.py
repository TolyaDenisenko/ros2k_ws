#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
arm_parking — режим парковки руки (имитация блокировки моторов/тормозов).

Пока парковка включена, траекторный контроллер руки ДЕАКТИВИРОВАН:
  * рука держит последнюю позу (gz_ros2_control продолжает отрабатывать
    последнюю позиционную команду — как тормоз, ничего не провисает);
  * любые команды движения (MoveIt Plan & Execute, arm_goto_tcp.py)
    отклоняются — контроллер не принимает цели, MoveIt пишет ошибку
    исполнения, а не дёргает руку посреди езды.
Выключение парковки снова активирует контроллер; он стартует с текущей позы.

Удержание (hold_controller, по умолчанию arm_hold_controller): в симуляции «выключенный» траекторный
контроллер НЕ держит позу — суставы остаются без управления, и рука складывается под своим весом
(13 кг на высоте ~1 м сдвигают центр масс, тележку кренит на 10–19°). Поэтому парковка переключает
руку на forward-контроллер позиций (config/arm_hold.yaml): он держит запомненные углы, как тормоза
на железе. hold_controller:='' — прежнее поведение (только выключить траекторный).

Запуск запаркованной (park_on_start): рука «не включена, пока нет запроса». Снять парковку —
сервис /arm_parking/set {data: false}; пока она снята вручную, при езде рука паркуется сама.

Интерфейс
---------
  сервис /arm_parking/set   (std_srvs/SetBool)  true — парковка, false — снять
  топик  /arm_parking/state (std_msgs/Bool, transient_local) — включена ли

Автопарковка (auto_park_on_drive, по умолчанию true): пока на /cmd_vel идут
ненулевые команды, парковка включается сама; снимается через release_delay
секунд после последней ненулевой команды — если её не включили вручную.

Параметры
  controller 'xarm6_traj_controller'   controller_manager '/controller_manager'
  auto_park_on_drive true   cmd_vel_topic '/cmd_vel'   release_delay 1.0 (с)
"""
import time

import rclpy
from controller_manager_msgs.srv import SwitchController
from geometry_msgs.msg import Twist
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from sensor_msgs.msg import JointState
from std_msgs.msg import Bool, Float64MultiArray
from std_srvs.srv import SetBool


class ArmParking(Node):
    def __init__(self):
        super().__init__('arm_parking')
        self.declare_parameter('controller', 'xarm6_traj_controller')
        self.declare_parameter('controller_manager', '/controller_manager')
        self.declare_parameter('auto_park_on_drive', True)
        self.declare_parameter('cmd_vel_topic', '/cmd_vel')
        self.declare_parameter('release_delay', 1.0)
        self.declare_parameter('hold_controller', 'arm_hold_controller')
        self.declare_parameter('joints', ['joint1', 'joint2', 'joint3', 'joint4', 'joint5', 'joint6'])
        self.declare_parameter('park_on_start', False)
        self.controller = self.get_parameter('controller').value
        self.release_delay = float(self.get_parameter('release_delay').value)
        self.hold = self.get_parameter('hold_controller').value
        self.joints = list(self.get_parameter('joints').value)
        self.joint_pos = {}
        self.hold_cmd = None         # углы, которые держит тормоз

        cbg = ReentrantCallbackGroup()
        self.switch_cli = self.create_client(
            SwitchController,
            self.get_parameter('controller_manager').value + '/switch_controller',
            callback_group=cbg)
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.pub_state = self.create_publisher(Bool, '/arm_parking/state', latched)
        self.create_service(SetBool, '/arm_parking/set', self.on_set, callback_group=cbg)

        self.create_subscription(JointState, '/joint_states', self.on_joints, 10,
                                 callback_group=cbg)
        if self.hold:
            self.pub_hold = self.create_publisher(
                Float64MultiArray, f'/{self.hold}/commands', 10)
            self.create_timer(0.05, self.on_hold_timer, callback_group=cbg)
        self.parked = False
        self.manual = False          # парковка включена вручную — авто её не снимает
        self.last_drive = None       # время последней ненулевой /cmd_vel
        if self.get_parameter('auto_park_on_drive').value:
            self.create_subscription(Twist, self.get_parameter('cmd_vel_topic').value,
                                     self.on_cmd_vel, 10, callback_group=cbg)
            self.create_timer(0.2, self.on_timer, callback_group=cbg)
        self.pub_state.publish(Bool(data=False))
        self.start_tries = 0
        if self.get_parameter('park_on_start').value:
            self.startup_timer = self.create_timer(1.0, self.on_startup, callback_group=cbg)
        self.get_logger().info(
            'arm_parking готов. Парковка: ros2 service call /arm_parking/set '
            'std_srvs/srv/SetBool "{data: true}"')

    def on_joints(self, msg: JointState):
        for name, pos in zip(msg.name, msg.position):
            if name in self.joints:
                self.joint_pos[name] = pos

    def on_hold_timer(self):
        # пока рука запаркована, постоянно шлём запомненные углы в контроллер удержания
        if self.parked and self.hold_cmd is not None:
            self.pub_hold.publish(Float64MultiArray(data=self.hold_cmd))

    def on_startup(self):
        # запуск запаркованной: ждём, пока контроллеры руки поднимутся (spawner стартует позже ноды)
        self.start_tries += 1
        ok, msg = (False, '')
        if len(self.joint_pos) == len(self.joints):
            self.manual = True       # снять парковку может только запрос
            ok, msg = self.switch(True)
        if ok or self.start_tries > 90:
            self.startup_timer.cancel()
            if not ok:
                self.manual = False
                self.get_logger().error('не удалось запарковать руку при старте: ' + msg)

    def switch(self, park: bool) -> tuple[bool, str]:
        if park == self.parked:
            return True, 'уже ' + ('запаркована' if park else 'активна')
        if not self.switch_cli.wait_for_service(timeout_sec=2.0):
            return False, 'нет сервиса controller_manager/switch_controller'
        if park and self.hold:
            if len(self.joint_pos) < len(self.joints):
                return False, 'нет /joint_states руки — не знаю, какую позу держать'
            # запоминаем позу ДО переключения; тормоз держит именно её
            self.hold_cmd = [self.joint_pos[j] for j in self.joints]
            for _ in range(3):
                self.pub_hold.publish(Float64MultiArray(data=self.hold_cmd))
        req = SwitchController.Request()
        if park:
            req.deactivate_controllers = [self.controller]
            if self.hold:
                req.activate_controllers = [self.hold]
        else:
            req.activate_controllers = [self.controller]
            if self.hold:
                req.deactivate_controllers = [self.hold]
        req.strictness = SwitchController.Request.STRICT
        req.activate_asap = True
        fut = self.switch_cli.call_async(req)
        # ответ придёт в другом потоке MultiThreadedExecutor (см. main())
        deadline = time.monotonic() + 3.0
        while not fut.done() and time.monotonic() < deadline:
            time.sleep(0.02)
        if not fut.done() or not fut.result().ok:
            return False, f'switch_controller не выполнен ({self.controller})'
        self.parked = park
        self.pub_state.publish(Bool(data=park))
        if not park:
            self.hold_cmd = None
        msg = (('рука запаркована: тормоз (' + self.hold + ') держит позу') if self.hold else
               'рука запаркована: контроллер выключен, поза удерживается'
               ) if park else 'парковка снята: контроллер активен'
        self.get_logger().info(msg)
        return True, msg

    def on_set(self, req: SetBool.Request, res: SetBool.Response):
        self.manual = req.data
        res.success, res.message = self.switch(req.data)
        return res

    def on_cmd_vel(self, msg: Twist):
        moving = any(abs(v) > 1e-3 for v in (msg.linear.x, msg.linear.y, msg.angular.z))
        if not moving:
            return
        self.last_drive = self.get_clock().now()
        if not self.parked:
            self.get_logger().info('тележка едет — автопарковка руки')
            self.switch(True)

    def on_timer(self):
        if not self.parked or self.manual or self.last_drive is None:
            return
        idle = (self.get_clock().now() - self.last_drive).nanoseconds * 1e-9
        if idle > self.release_delay:
            self.last_drive = None
            self.get_logger().info('тележка стоит — снимаю автопарковку')
            self.switch(False)


def main():
    rclpy.init()
    node = ArmParking()
    ex = MultiThreadedExecutor()
    ex.add_node(node)
    try:
        ex.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
