#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
vacuum_gripper_sim — «логическая» вакуумная присоска для Gazebo Sim.

Физической присоски в gz нет. Вместо неё в модель робота добавлены плагины
gz::sim::systems::DetachableJoint — по одному на каждый объект из списка
(quasi_cart_description/urdf/cart-arm.xacro, xacro-аргумент vacuum_targets):
по команде attach плагин жёстко (fixed joint) соединяет звено руки link6 со
своим объектом, по detach — разъединяет. Эта нода решает, КОГДА и КАКОЙ
объект можно приварить, и даёт ROS-интерфейс «включить / выключить вакуум».

Интерфейс
---------
  сервис  /vacuum_gripper/set  (std_srvs/SetBool)
          data=true  — включить вакуум: из объектов, которых касается присоска
                       (см. «условия касания»), берётся ближайший -> attach;
                       если касания нет — отказ (success=false с причиной).
          data=false — выключить: detach, объект падает.
  топик   /vacuum_gripper/cmd (std_msgs/Bool) — то же, что сервис, без ответа
          (для кнопки в RViz, скриптов, обучения); результат — в state/status.
  топик   /vacuum_gripper/state (std_msgs/Bool, transient_local) — true, если
          объект сейчас приварен (подтверждено плагином).
  топик   /vacuum_gripper/held (std_msgs/String, transient_local) — имя
          удерживаемого объекта или '' .
  топик   /vacuum_gripper/status (std_msgs/String) — человекочитаемое
          состояние ~2 Гц: ближайший объект, расстояние, угол, держит ли.
  Готовность к захвату (10 Гц) — что можно взять прямо сейчас:
  топик   /vacuum_gripper/ready (std_msgs/Bool) — true, если присоска касается
          объекта (условия ниже) и вакуум сейчас сработает;
  топик   /vacuum_gripper/contact (std_msgs/String, JSON) — для программ/обучения:
          {"object", "dist", "angle", "level", "ready", "held", "frame",
           "tcp", "point"}; level: none | far | near (< near_distance) |
          ready | held; point — ближайшая точка поверхности объекта, tcp —
          кончик присоски (оба в frame = root_frame);
  топик   /vacuum_gripper/markers (visualization_msgs/MarkerArray) — RViz:
          линия TCP -> ближайшая точка, точка касания, подсветка целевого
          объекта; цвет по level (серый / жёлтый / зелёный).
  (внутри) /vacuum/<объект>/{attach,detach} (Empty) -> gz;
          /vacuum/<объект>/state (String) <- gz;
          /gz/robot_pose, /gz/object_pose (TFMessage) <- gz: позы моделей в
          мире (10 Гц), запись выбирается по child_frame_id = имени модели.

Условия касания (проверка «честного» захвата, документ 09 §8)
-------------------------------------------------------------
  1) расстояние от точки TCP (link_tcp, кончик присоски) до поверхности
     объекта (бокс размера object_sizes, поза из gz) ≤ contact_distance;
     цилиндр приближается описанным боксом (2r x 2r x h);
  2) угол между осью инструмента (Z link_tcp) и ближайшей к ней нормалью грани
     объекта ≤ max_angle_deg — присоска смотрит НА грань, а не вскользь.
  Параметр require_contact:=false отключает проверку (принудительный attach
  ближайшего объекта — только для отладки).

Геометрия
---------
  Мировая поза TCP = T_world<-root · T_root<-tcp, где
    T_world<-root — поза модели робота из gz (/gz/robot_pose, child_frame_id
                    = robot_model_name; корень модели — root_frame),
    T_root<-tcp   — из TF (robot_state_publisher): root_frame -> tcp_frame.

Параметры (значения по умолчанию подходят для cart_xarm6_gz.launch.py)
  robot_model_name 'cart_xarm6'      root_frame 'base_footprint'
  tcp_frame 'link_tcp'
  object_names ['pick_box']          object_sizes [0.10, 0.05, 0.05]
                                     (по 3 числа X Y Z на объект, подряд)
  object_shapes ['box']   (box | cylinder — только для подсветки в RViz)
  contact_distance 0.015  max_angle_deg 20.0  require_contact true
  near_distance 0.05 (с какого расстояния level = near)
  attach_confirm_timeout 1.0 (с, ждать подтверждения плагином)
"""
import json
import math
import time
import traceback

import numpy as np
import rclpy
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.duration import Duration
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from geometry_msgs.msg import Point
from scipy.spatial.transform import Rotation
from std_msgs.msg import Bool, Empty, String
from std_srvs.srv import SetBool
from tf2_msgs.msg import TFMessage
from visualization_msgs.msg import Marker, MarkerArray
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
        self.declare_parameter('root_frame', 'base_footprint')
        self.declare_parameter('tcp_frame', 'link_tcp')
        self.declare_parameter('object_names', ['pick_box'])
        self.declare_parameter('object_sizes', [0.10, 0.05, 0.05])
        self.declare_parameter('object_shapes', [''])   # пусто -> все box
        self.declare_parameter('near_distance', 0.05)
        self.declare_parameter('contact_distance', 0.015)
        self.declare_parameter('max_angle_deg', 20.0)
        self.declare_parameter('require_contact', True)
        self.declare_parameter('attach_confirm_timeout', 1.0)

        self.robot_name = self.get_parameter('robot_model_name').value
        self.root_frame = self.get_parameter('root_frame').value
        self.tcp_frame = self.get_parameter('tcp_frame').value
        names = list(self.get_parameter('object_names').value)
        sizes = np.array(self.get_parameter('object_sizes').value, float).reshape(-1, 3)
        if sizes.shape[0] != len(names):
            raise ValueError(f'object_sizes: нужно по 3 числа на объект '
                             f'({len(names)} объектов, {sizes.shape[0]} троек)')
        self.half = {n: 0.5 * s for n, s in zip(names, sizes)}
        shapes = [sh for sh in self.get_parameter('object_shapes').value if sh]
        self.shape = {n: (shapes[i] if i < len(shapes) else 'box')
                      for i, n in enumerate(names)}
        self.status_tick = 0

        # Последние известные мировые позы (матрицы 4x4) из gz.
        # None до первого сообщения — тогда attach отклоняется с понятной причиной.
        self.T_world_robot = None
        self.T_world_obj = {n: None for n in names}
        self.attached = {n: False for n in names}   # по подтверждению плагина
        self.requested = None      # объект, который пользователь велел держать
        # Сколько раз ещё разослать стартовый detach (см. on_status_timer)
        self.startup_detach_left = 6

        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.pub_state = self.create_publisher(Bool, '/vacuum_gripper/state', latched)
        self.pub_held = self.create_publisher(String, '/vacuum_gripper/held', latched)
        self.pub_status = self.create_publisher(String, '/vacuum_gripper/status', 10)
        self.pub_ready = self.create_publisher(Bool, '/vacuum_gripper/ready', 10)
        self.pub_contact = self.create_publisher(String, '/vacuum_gripper/contact', 10)
        self.pub_markers = self.create_publisher(MarkerArray, '/vacuum_gripper/markers', 10)

        # Reentrant: сервис ждёт подтверждения из подписки на /vacuum/<объект>/state,
        # поэтому они должны выполняться параллельно (одна mutually-exclusive
        # группа заблокировала бы сама себя).
        cbg = ReentrantCallbackGroup()
        self.pub_attach, self.pub_detach = {}, {}
        for n in names:
            self.pub_attach[n] = self.create_publisher(Empty, f'/vacuum/{n}/attach', 10)
            self.pub_detach[n] = self.create_publisher(Empty, f'/vacuum/{n}/detach', 10)
            self.create_subscription(
                String, f'/vacuum/{n}/state',
                lambda msg, n=n: self.on_gz_state(n, msg), 10, callback_group=cbg)
        for topic in ('/gz/robot_pose', '/gz/object_pose'):
            self.create_subscription(TFMessage, topic, self.on_poses, 10,
                                     callback_group=cbg)
        self.create_service(SetBool, '/vacuum_gripper/set', self.on_set,
                            callback_group=cbg)
        self.create_subscription(Bool, '/vacuum_gripper/cmd', self.on_cmd, 10,
                                 callback_group=cbg)
        self.create_timer(0.1, self.on_status_timer, callback_group=cbg)

        self.publish_state()
        self.get_logger().info(
            f'vacuum_gripper_sim готов: робот="{self.robot_name}", объекты={names}, '
            f'TCP={self.root_frame}->{self.tcp_frame}. Включить вакуум: '
            f'ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{{data: true}}"')

    # ------------------------------------------------------------------ gz
    def on_poses(self, msg: TFMessage):
        # child_frame_id = имя модели в gz, 10 Гц
        for tf in msg.transforms:
            if tf.child_frame_id == self.robot_name:
                self.T_world_robot = tf_to_matrix(tf)
            elif tf.child_frame_id in self.T_world_obj:
                self.T_world_obj[tf.child_frame_id] = tf_to_matrix(tf)

    def on_gz_state(self, name, msg: String):
        attached = 'attached' in msg.data.lower() and 'detached' not in msg.data.lower()
        if attached != self.attached[name]:
            self.attached[name] = attached
            self.get_logger().info(f'DetachableJoint {name}: {msg.data}')
            self.publish_state()
        # ВАЖНО: плагин DetachableJoint сам приваривает объект при загрузке (как только
        # робот заспавнен и модель найдена) — без команды attach. Если держать этот
        # объект никто не велел, сразу отпускаем, иначе он поедет вместе с рукой.
        if attached and self.requested != name:
            self.get_logger().warning(f'плагин приварил {name} без команды — отпускаю')
            self.pub_detach[name].publish(Empty())

    def held(self):
        return next((n for n, a in self.attached.items() if a), '')

    def publish_state(self):
        h = self.held()
        self.pub_state.publish(Bool(data=bool(h)))
        self.pub_held.publish(String(data=h))

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

    def contact_metrics(self, name, T_w_tcp):
        """(dist, angle_deg) TCP относительно объекта name или (None, None).

        Точка TCP переводится в систему объекта; расстояние до бокса — стандартное
        расстояние от точки до OBB (0 внутри). Угол — между осью инструмента (+Z TCP)
        и направлением «в объект», т.е. -n ближайшей грани (n — внешняя нормаль).
        """
        T_obj = self.T_world_obj[name]
        if T_obj is None:
            return None, None
        T_obj_tcp = np.linalg.inv(T_obj) @ T_w_tcp
        p = T_obj_tcp[:3, 3]
        over = np.abs(p) - self.half[name]      # >0 — снаружи по этой оси
        dist = float(np.linalg.norm(np.maximum(over, 0.0)))
        # Нормаль грани, к которой точка ближе всего (по оси с максимальным "over").
        axis = int(np.argmax(over))
        n = np.zeros(3)
        n[axis] = math.copysign(1.0, p[axis])
        tool_z = T_obj_tcp[:3, 2]               # ось Z инструмента в системе объекта
        cosang = float(np.clip(np.dot(tool_z, -n), -1.0, 1.0))
        return dist, math.degrees(math.acos(cosang))

    def closest_point_world(self, name, T_w_tcp):
        """Ближайшая к TCP точка поверхности объекта (бокс) в мире, (3,)."""
        T_obj = self.T_world_obj[name]
        p = (np.linalg.inv(T_obj) @ T_w_tcp)[:3, 3]
        q = np.clip(p, -self.half[name], self.half[name])
        return (T_obj @ np.append(q, 1.0))[:3]

    def nearest(self, T_w_tcp=None):
        """(имя, dist, angle, причина) ближайшего к TCP объекта."""
        why = ''
        if T_w_tcp is None:
            T_w_tcp, why = self.tcp_in_world()
        if T_w_tcp is None:
            return None, None, None, why
        best = (None, None, None, 'нет поз объектов из gz (/gz/object_pose)')
        for name in self.T_world_obj:
            dist, ang = self.contact_metrics(name, T_w_tcp)
            if dist is not None and (best[1] is None or dist < best[1]):
                best = (name, dist, ang, '')
        return best

    # ------------------------------------------------------------ команды
    def vacuum_on(self):
        """(ok, сообщение)."""
        if self.held():
            return True, f'уже держит {self.held()}'
        name, dist, ang, why = self.nearest()
        if name is None:
            return False, f'отказ: {why}'
        if self.get_parameter('require_contact').value:
            dmax = self.get_parameter('contact_distance').value
            amax = self.get_parameter('max_angle_deg').value
            if dist > dmax or ang > amax:
                return False, (f'отказ: нет касания — до {name} {dist * 1000:.1f} мм '
                               f'(допуск {dmax * 1000:.0f}), угол {ang:.1f}° '
                               f'(допуск {amax:.0f}°)')
            self.get_logger().info(
                f'касание {name} ок: {dist * 1000:.1f} мм, {ang:.1f}° — шлю attach')
        else:
            self.get_logger().warning(
                f'require_contact=false — attach {name} без проверки касания')

        self.requested = name
        self.pub_attach[name].publish(Empty())
        # Ждём подтверждения плагина: on_gz_state выполняется в другом потоке
        # MultiThreadedExecutor (см. main()), здесь спим короткими шагами.
        deadline = time.monotonic() + self.get_parameter('attach_confirm_timeout').value
        while time.monotonic() < deadline and not self.attached[name]:
            time.sleep(0.02)
        if self.attached[name]:
            return True, f'объект {name} захвачен (DetachableJoint: attached)'
        self.requested = None
        return False, (f'attach {name} отправлен, но плагин не подтвердил — проверь, '
                       f'что модель есть в мире, не static и её имя есть в '
                       f'vacuum_targets (cart-arm.xacro)')

    def vacuum_off(self):
        self.requested = None
        for n, pub in self.pub_detach.items():
            pub.publish(Empty())
        return True, 'вакуум выключен (detach отправлен)'

    def on_set(self, req: SetBool.Request, res: SetBool.Response):
        res.success, res.message = self.vacuum_on() if req.data else self.vacuum_off()
        (self.get_logger().info if res.success else self.get_logger().warning)(res.message)
        return res

    def on_cmd(self, msg: Bool):
        ok, text = self.vacuum_on() if msg.data else self.vacuum_off()
        (self.get_logger().info if ok else self.get_logger().warning)(text)

    # ------------------------------------------------------------ статус
    def on_status_timer(self):
        # Плагин приваривает все объекты при загрузке модели и сообщает об этом
        # ОДИН раз (/vacuum/<объект>/state) — часто раньше, чем поднимется мост,
        # и сообщение теряется: объект остаётся приваренным, а нода считает, что
        # ничего не держит, и первое движение руки тащит объект за собой. Поэтому
        # после появления позы робота (модель заспавнена) несколько раз подряд
        # отпускаем всё, что держать не велели. detach свободного объекта плагин
        # игнорирует.
        if self.startup_detach_left > 0 and self.T_world_robot is not None:
            self.startup_detach_left -= 1
            for n, pub in self.pub_detach.items():
                if n != self.requested:
                    pub.publish(Empty())
        T_w_tcp, why = self.tcp_in_world()
        name, dist, ang, why2 = self.nearest(T_w_tcp) if T_w_tcp is not None else (
            None, None, None, why)
        self.publish_contact(T_w_tcp, name, dist, ang)

        # текст для человека — 2 Гц (таймер 10 Гц)
        self.status_tick = (self.status_tick + 1) % 5
        if self.status_tick:
            return
        if name is None:
            txt = f'нет данных: {why2}'
        else:
            txt = (f'ближайший {name}: {dist * 1000:.1f} мм, угол {ang:.1f}°; '
                   f'вакуум={"вкл" if self.requested else "выкл"}, '
                   f'держит={self.held() or "нет"}')
        self.pub_status.publish(String(data=txt))

    # ------------------------------------------------------ готовность
    def level(self, dist, ang):
        if self.held():
            return 'held'
        if dist is None:
            return 'none'
        if (dist <= self.get_parameter('contact_distance').value
                and ang <= self.get_parameter('max_angle_deg').value):
            return 'ready'
        return 'near' if dist <= self.get_parameter('near_distance').value else 'far'

    def publish_contact(self, T_w_tcp, name, dist, ang):
        """/vacuum_gripper/{ready,contact,markers} — что можно взять прямо сейчас."""
        lvl = self.level(dist, ang)
        held = self.held()
        self.pub_ready.publish(Bool(data=(lvl == 'ready')))
        info = {'object': held or name or '', 'dist': dist, 'angle': ang,
                'level': lvl, 'ready': lvl == 'ready', 'held': held,
                'frame': self.root_frame, 'tcp': None, 'point': None}
        markers = MarkerArray()
        clear = Marker()
        clear.action = Marker.DELETEALL
        markers.markers.append(clear)
        if T_w_tcp is not None and name is not None and self.T_world_robot is not None:
            T_root_w = np.linalg.inv(self.T_world_robot)
            target = held or name
            tcp = (T_root_w @ T_w_tcp)[:3, 3]
            pt = (T_root_w @ np.append(self.closest_point_world(target, T_w_tcp), 1.0))[:3]
            info['tcp'] = [float(v) for v in tcp]
            info['point'] = [float(v) for v in pt]
            markers.markers += self.contact_markers(target, T_root_w @ self.T_world_obj[target],
                                                    tcp, pt, lvl)
        self.pub_contact.publish(String(data=json.dumps(info, ensure_ascii=False)))
        self.pub_markers.publish(markers)

    # серый — далеко, жёлтый — подводим, светло-зелёный — можно брать, зелёный — держит
    COLORS = {'far': (0.6, 0.6, 0.6), 'near': (1.0, 0.8, 0.0),
              'ready': (0.3, 1.0, 0.3), 'held': (0.0, 0.8, 0.0)}

    def contact_markers(self, name, T_root_obj, tcp, pt, lvl):
        r, g, b = self.COLORS.get(lvl, (0.6, 0.6, 0.6))
        stamp = self.get_clock().now().to_msg()

        def mk(ns, type_):
            m = Marker()
            m.header.frame_id = self.root_frame
            m.header.stamp = stamp
            m.ns, m.id, m.type, m.action = ns, 0, type_, Marker.ADD
            m.pose.orientation.w = 1.0
            m.color.r, m.color.g, m.color.b, m.color.a = r, g, b, 1.0
            return m

        line = mk('vacuum_contact_line', Marker.LINE_STRIP)
        line.scale.x = 0.003
        line.points = [Point(x=float(tcp[0]), y=float(tcp[1]), z=float(tcp[2])),
                       Point(x=float(pt[0]), y=float(pt[1]), z=float(pt[2]))]
        dot = mk('vacuum_contact_point', Marker.SPHERE)
        dot.pose.position = Point(x=float(pt[0]), y=float(pt[1]), z=float(pt[2]))
        dot.scale.x = dot.scale.y = dot.scale.z = 0.012
        # подсветка цели: полупрозрачная оболочка на 6 мм больше объекта
        hl = mk('vacuum_target', Marker.CYLINDER if self.shape[name] == 'cylinder'
                else Marker.CUBE)
        hl.pose.position = Point(x=float(T_root_obj[0, 3]), y=float(T_root_obj[1, 3]),
                                 z=float(T_root_obj[2, 3]))
        q = Rotation.from_matrix(T_root_obj[:3, :3]).as_quat()
        (hl.pose.orientation.x, hl.pose.orientation.y,
         hl.pose.orientation.z, hl.pose.orientation.w) = map(float, q)
        hl.scale.x, hl.scale.y, hl.scale.z = map(float, 2 * self.half[name] + 0.006)
        hl.color.a = 0.45
        return [line, dot, hl]


def main():
    rclpy.init()
    node = VacuumGripperSim()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(node)
    try:
        # Исключение в любом колбэке останавливает spin и роняет процесс (раньше
        # нода однажды умерла с exit 1, а трейсбек остался только в терминале).
        # Пишем трейсбек в лог ROS (~/.ros/log) и продолжаем работать.
        while rclpy.ok():
            try:
                ex.spin()
            except (KeyboardInterrupt, ExternalShutdownException):
                break
            except Exception:  # noqa: BLE001 — нода вакуума не должна падать
                node.get_logger().error(
                    'исключение в колбэке, продолжаю работу:\n' + traceback.format_exc())
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
