#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
vacuum_gripper_sim — «логическая» вакуумная присоска для Gazebo Sim.

Физической присоски в gz нет. Вместо неё в модель робота добавлен плагин
gz::sim::systems::DetachableJoint (quasi_cart_description/urdf/cart-arm.xacro):
по команде attach он жёстко (fixed joint) соединяет звено руки link6 с объектом
(pick_box), по команде detach — разъединяет. Эта нода решает, КОГДА attach
допустим, и предоставляет ROS-интерфейс «включить / выключить вакуум».

Интерфейс
---------
  сервис  /vacuum_gripper/set  (std_srvs/SetBool)
          data=true  — включить вакуум: если присоска у поверхности объекта
                       (см. «условия касания») — шлёт attach; иначе отказ
                       (success=false с причиной в message).
          data=false — выключить: detach, объект падает.
  топик   /vacuum_gripper/state (std_msgs/Bool, transient_local) — true, если
          объект сейчас приварен (подтверждено плагином через /vacuum/state).
  топик   /vacuum_gripper/status (std_msgs/String) — человекочитаемое
          состояние ~2 Гц: расстояние TCP до объекта, угол, держит ли.
  (внутри) /vacuum/attach, /vacuum/detach (Empty) -> gz; /vacuum/state (String) <- gz;
          /gz/robot_pose, /gz/object_pose (TFMessage) <- gz: позы моделей в мире (10 Гц).

Условия касания (проверка «честного» захвата, документ 09 §8)
-------------------------------------------------------------
  1) расстояние от точки TCP (link_tcp, кончик присоски) до поверхности
     объекта (бокс размера object_size, поза из gz) ≤ contact_distance;
  2) угол между осью инструмента (Z link_tcp) и ближайшей к ней нормалью грани
     объекта ≤ max_angle_deg — присоска смотрит НА грань, а не вскользь.
  Параметр require_contact:=false отключает проверку (принудительный attach —
  только для отладки).

Геометрия
---------
  Мировая поза TCP = T_world<-root · T_root<-tcp, где
    T_world<-root — поза модели робота из gz (/gz/robot_pose, child_frame_id
                    = robot_model_name; корень SDF — base_footprint),
    T_root<-tcp   — из TF (robot_state_publisher): root_frame -> tcp_frame.
  Поза объекта — из того же сообщения (child_frame_id = object_model_name).

Параметры (значения по умолчанию подходят для cart_xarm6_gz.launch.py)
  robot_model_name 'cart_xarm6'  object_model_name 'pick_box'
  root_frame 'base_footprint'    tcp_frame 'link_tcp'
  object_size [0.10, 0.05, 0.05] (X, Y, Z локальные оси бокса, м)
  contact_distance 0.015  max_angle_deg 20.0  require_contact true
  attach_confirm_timeout 1.0 (с, ждать подтверждения плагином)
"""
import math
import time

import numpy as np
import rclpy
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.duration import Duration
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from std_msgs.msg import Bool, Empty, String
from std_srvs.srv import SetBool
from tf2_msgs.msg import TFMessage
from tf2_ros import Buffer, TransformListener


def quat_to_rot(q):
    """Кватернион (x, y, z, w) -> матрица поворота 3x3."""
    x, y, z, w = q
    return np.array([
        [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
        [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
        [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)],
    ])


def pose_matrix(t, q):
    """Поза (трансляция, кватернион) -> однородная матрица 4x4."""
    m = np.eye(4)
    m[:3, :3] = quat_to_rot(q)
    m[:3, 3] = t
    return m


def tf_to_matrix(tf):
    t, q = tf.transform.translation, tf.transform.rotation
    return pose_matrix([t.x, t.y, t.z], [q.x, q.y, q.z, q.w])


class VacuumGripperSim(Node):
    def __init__(self):
        super().__init__('vacuum_gripper_sim')

        self.declare_parameter('robot_model_name', 'cart_xarm6')
        self.declare_parameter('object_model_name', 'pick_box')
        self.declare_parameter('root_frame', 'base_footprint')
        self.declare_parameter('tcp_frame', 'link_tcp')
        self.declare_parameter('object_size', [0.10, 0.05, 0.05])
        self.declare_parameter('contact_distance', 0.015)
        self.declare_parameter('max_angle_deg', 20.0)
        self.declare_parameter('require_contact', True)
        self.declare_parameter('attach_confirm_timeout', 1.0)

        self.robot_name = self.get_parameter('robot_model_name').value
        self.object_name = self.get_parameter('object_model_name').value
        self.root_frame = self.get_parameter('root_frame').value
        self.tcp_frame = self.get_parameter('tcp_frame').value
        self.half = 0.5 * np.array(self.get_parameter('object_size').value, float)

        # Последние известные мировые позы (матрицы 4x4) из gz.
        # None до первого сообщения — тогда attach отклоняется с понятной причиной.
        self.T_world_robot = None
        self.T_world_object = None
        self.attached = False      # по подтверждению плагина (/vacuum/state)
        self.requested = False     # вакуум включён пользователем (для status)

        # TF создаём в __init__ (а не по требованию), чтобы буфер уже копился.
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

        self.pub_attach = self.create_publisher(Empty, '/vacuum/attach', 10)
        self.pub_detach = self.create_publisher(Empty, '/vacuum/detach', 10)
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.pub_state = self.create_publisher(Bool, '/vacuum_gripper/state', latched)
        self.pub_status = self.create_publisher(String, '/vacuum_gripper/status', 10)

        # Reentrant: сервис on_set ждёт подтверждения из подписки on_gz_state,
        # поэтому они должны выполняться параллельно (одна mutually-exclusive
        # группа заблокировала бы сама себя).
        cbg = ReentrantCallbackGroup()
        for topic in ('/gz/robot_pose', '/gz/object_pose'):
            self.create_subscription(TFMessage, topic, self.on_poses, 5,
                                     callback_group=cbg)
        self.create_subscription(String, '/vacuum/state', self.on_gz_state, 10,
                                 callback_group=cbg)
        self.create_service(SetBool, '/vacuum_gripper/set', self.on_set,
                            callback_group=cbg)
        self.create_timer(0.5, self.on_status_timer, callback_group=cbg)

        self.publish_state()
        self.get_logger().info(
            f'vacuum_gripper_sim готов: робот="{self.robot_name}", объект='
            f'"{self.object_name}", TCP={self.root_frame}->{self.tcp_frame}. '
            f'Включить вакуум: ros2 service call /vacuum_gripper/set '
            f'std_srvs/srv/SetBool "{{data: true}}"')

    # ------------------------------------------------------------------ gz
    def on_poses(self, msg: TFMessage):
        # /gz/robot_pose и /gz/object_pose: по одной записи (child_frame_id = имя
        # модели в gz), 10 Гц. Берём позу из записи с нужным именем.
        for tf in msg.transforms:
            if tf.child_frame_id == self.robot_name:
                self.T_world_robot = tf_to_matrix(tf)
            elif tf.child_frame_id == self.object_name:
                self.T_world_object = tf_to_matrix(tf)

    def on_gz_state(self, msg: String):
        attached = 'attached' in msg.data.lower() and 'detached' not in msg.data.lower()
        if attached != self.attached:
            self.attached = attached
            self.get_logger().info(f'DetachableJoint: {msg.data} -> attached={attached}')
            self.publish_state()
        # ВАЖНО: плагин DetachableJoint сам приваривает объект при загрузке (как только
        # робот заспавнен и модель pick_box найдена) — без команды attach. Если вакуум
        # никто не включал, сразу отпускаем, иначе коробка поедет вместе с рукой.
        if attached and not self.requested:
            self.get_logger().warn('плагин приварил объект при старте — отпускаю (detach)')
            self.pub_detach.publish(Empty())

    def publish_state(self):
        self.pub_state.publish(Bool(data=self.attached))

    # --------------------------------------------------------- геометрия
    def tcp_in_world(self):
        """Поза TCP в мире (4x4) или (None, причина)."""
        if self.T_world_robot is None:
            return None, 'нет позы робота из gz (/gz/robot_pose не приходит?)'
        try:
            tf = self.tf_buffer.lookup_transform(
                self.root_frame, self.tcp_frame, rclpy.time.Time(),
                timeout=Duration(seconds=0.5))
        except Exception as e:  # tf2 бросает несколько типов исключений
            return None, f'нет TF {self.root_frame}->{self.tcp_frame}: {e}'
        return self.T_world_robot @ tf_to_matrix(tf), ''

    def contact_metrics(self):
        """(dist, angle_deg, причина). dist — от TCP до поверхности бокса, м.

        Точка TCP переводится в систему объекта; расстояние до бокса — стандартное
        расстояние от точки до OBB (0 внутри). Угол — между осью инструмента (+Z TCP)
        и направлением «в объект», т.е. -n ближайшей грани (n — внешняя нормаль).
        """
        if self.T_world_object is None:
            return None, None, 'нет позы объекта из gz'
        T_w_tcp, why = self.tcp_in_world()
        if T_w_tcp is None:
            return None, None, why
        T_obj_tcp = np.linalg.inv(self.T_world_object) @ T_w_tcp
        p = T_obj_tcp[:3, 3]
        over = np.abs(p) - self.half            # >0 — снаружи по этой оси
        dist = float(np.linalg.norm(np.maximum(over, 0.0)))
        # Нормаль грани, к которой точка ближе всего (по оси с максимальным "over").
        axis = int(np.argmax(over))
        n = np.zeros(3)
        n[axis] = math.copysign(1.0, p[axis])
        tool_z = T_obj_tcp[:3, 2]               # ось Z инструмента в системе объекта
        cosang = float(np.clip(np.dot(tool_z, -n), -1.0, 1.0))
        return dist, math.degrees(math.acos(cosang)), ''

    # ------------------------------------------------------------ сервис
    def on_set(self, req: SetBool.Request, res: SetBool.Response):
        if not req.data:
            self.requested = False
            self.pub_detach.publish(Empty())
            res.success = True
            res.message = 'вакуум выключен (detach отправлен)'
            self.get_logger().info(res.message)
            return res

        if self.attached:
            res.success = True
            res.message = 'уже держит'
            return res

        if self.get_parameter('require_contact').value:
            dist, ang, why = self.contact_metrics()
            if dist is None:
                res.success = False
                res.message = f'отказ: {why}'
                self.get_logger().warn(res.message)
                return res
            dmax = self.get_parameter('contact_distance').value
            amax = self.get_parameter('max_angle_deg').value
            if dist > dmax or ang > amax:
                res.success = False
                res.message = (f'отказ: нет касания — расстояние {dist * 1000:.1f} мм '
                               f'(допуск {dmax * 1000:.0f}), угол {ang:.1f}° '
                               f'(допуск {amax:.0f}°)')
                self.get_logger().warn(res.message)
                return res
            self.get_logger().info(
                f'касание ок: {dist * 1000:.1f} мм, {ang:.1f}° — шлю attach')
        else:
            self.get_logger().warn('require_contact=false — attach без проверки касания')

        self.requested = True
        self.pub_attach.publish(Empty())
        # Ждём подтверждения плагина: on_gz_state выполняется в другом потоке
        # MultiThreadedExecutor (см. main()), здесь спим короткими шагами.
        deadline = time.monotonic() + self.get_parameter('attach_confirm_timeout').value
        while time.monotonic() < deadline and not self.attached:
            time.sleep(0.02)
        res.success = self.attached
        res.message = ('объект захвачен (DetachableJoint: attached)' if self.attached else
                       'attach отправлен, но плагин не подтвердил — проверь, что '
                       'модель объекта существует и плагин загружен (docs/STATUS.md)')
        (self.get_logger().info if res.success else self.get_logger().error)(res.message)
        return res

    # ------------------------------------------------------------ статус
    def on_status_timer(self):
        dist, ang, why = self.contact_metrics()
        if dist is None:
            txt = f'нет данных: {why}'
        else:
            txt = (f'TCP->объект: {dist * 1000:.1f} мм, угол {ang:.1f}°, '
                   f'вакуум={"вкл" if self.requested else "выкл"}, '
                   f'держит={"да" if self.attached else "нет"}')
        self.pub_status.publish(String(data=txt))


def main():
    rclpy.init()
    node = VacuumGripperSim()
    ex = MultiThreadedExecutor(num_threads=4)
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
