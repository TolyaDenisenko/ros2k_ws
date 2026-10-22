#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
object_perception — что лежит на столе: объекты с классом, формой, размером,
позой и позой захвата присоской (xarm_grasping.perception).

Облако камеры гриппера -> base_frame -> стол -> кластеры -> форма (коробка /
цилиндр) -> класс по правилам config/objects.yaml.

Интерфейс
---------
  action  /detect_objects (xarm_grasping_msgs/action/DetectObjects)
          ждёт СВЕЖЕЕ облако (пришедшее после запроса), распознаёт, возвращает
          объекты; update_scene=true — ещё и публикует их в /objects (по нему
          scene_manager обновляет Planning Scene MoveIt).
  топик   /objects (xarm_grasping_msgs/DetectedObjectArray, transient_local) —
          последний результат (и периодического режима, period > 0).
  топик   /objects/markers (visualization_msgs/MarkerArray) — формы и подписи.

Объект в присоске в сцену не попадает: пока /vacuum_gripper/held не пуст,
кластеры, чей центр ближе held_exclude_radius к link_tcp, отбрасываются (без
вакуума — нет: на подходе присоска тоже рядом с объектом).

id объекта (<класс>_<n>) сохраняется между распознаваниями: объект того же
класса в пределах match_radius от прежнего места — тот же id. Объект, который
унесла присоска (ближайший к TCP, когда /vacuum_gripper/held стал непустым),
сохраняет id на новом месте. Невидимые объекты не забываются. (Раньше id
отдавался любому «новому» объекту того же класса — и ошибочно распознанный
кадр забирал id у стоящего на месте объекта.)

Периодический режим публикует /objects только при хорошем обзоре — камера не
ниже min_view_height над столом (она висит над присоской и видит дальше 25 см;
у стола обзор частичный, и объекты пропадали бы из сцены MoveIt посреди
движения). Action /detect_objects при таком обзоре отказывает с причиной.

Параметры (дефолты — сцена «рука на столе»)
  points_topic /gripper_camera/points  base_frame link_base  tcp_frame link_tcp
  table_height 0.0 (ожидаемая высота стола в base_frame)  table_search 0.05
  voxel 0.01 (ячейка сегментации)  max_points 60000  max_range 1.2  max_height 0.30  min_points 20
  held_exclude_radius 0.12  match_radius 0.05
  label_frames 3 (кадров подряд, чтобы объект на месте сменил класс)
  rules_file '' (пусто — share/xarm_grasping/config/objects.yaml)
  period 1.0 (с; 0 — распознавать только по запросу action)
  min_view_height 0.42 (м — высота камеры над столом, с которой сцена публикуется;
                   поза наблюдения pick_place_server — ~0.47)
"""
import math
import os
import threading
import time
import traceback

import numpy as np
import rclpy
import yaml
from ament_index_python.packages import get_package_share_directory
from geometry_msgs.msg import Pose
from rclpy.action import ActionServer
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from sensor_msgs.msg import PointCloud2
from std_msgs.msg import String
from tf2_ros import Buffer, TransformListener
from visualization_msgs.msg import Marker, MarkerArray

from xarm_grasping import perception as P
from xarm_grasping_msgs.action import DetectObjects
from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray


def quat_to_rot(x, y, z, w):
    return np.array([
        [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
        [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
        [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def cloud_xyz(msg: PointCloud2) -> np.ndarray:
    """(N, 3) float из PointCloud2 (поля x, y, z float32), без NaN."""
    off = {f.name: f.offset for f in msg.fields}
    raw = np.frombuffer(msg.data, dtype=np.uint8).reshape(-1, msg.point_step)
    raw = raw[:msg.width * msg.height]
    pts = np.column_stack([raw[:, off[c]:off[c] + 4].copy().view(np.float32).ravel()
                           for c in 'xyz']).astype(float)
    return pts[np.isfinite(pts).all(axis=1)]


class ObjectPerception(Node):
    def __init__(self):
        super().__init__('object_perception')
        dp = self.declare_parameter
        dp('points_topic', '/gripper_camera/points')
        dp('base_frame', 'link_base')
        dp('tcp_frame', 'link_tcp')
        dp('table_height', 0.0)
        dp('table_search', 0.05)
        dp('voxel', 0.01)            # ячейка сегментации, м
        dp('max_points', 60000)      # случайное прореживание облака
        dp('max_range', 1.2)
        dp('max_height', 0.30)
        dp('min_points', 20)
        dp('held_exclude_radius', 0.12)
        dp('match_radius', 0.05)
        dp('label_frames', 3)
        dp('rules_file', '')
        dp('period', 1.0)
        dp('min_view_height', 0.42)
        g = lambda n: self.get_parameter(n).value  # noqa: E731
        self.base_frame = g('base_frame')
        self.tcp_frame = g('tcp_frame')

        rules_file = g('rules_file') or os.path.join(
            get_package_share_directory('xarm_grasping'), 'config', 'objects.yaml')
        with open(rules_file) as f:
            self.rules = yaml.safe_load(f)['objects']
        self.get_logger().info(f'правила классов: {list(self.rules)} ({rules_file})')

        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

        self.lock = threading.Lock()
        self.cloud = None            # последнее облако
        self.cloud_time = 0.0        # time.monotonic() прихода
        self.known = {}              # id -> (label, center xy) — все когда-либо виденные
        self.counters = {}
        self.held = ''               # /vacuum_gripper/held
        self.moving = set()          # id, унесённые присоской (ждут нового места)
        self.votes = {}              # id -> (кандидат в новый класс, кадров подряд)
        self.view_height = 0.0       # высота камеры над столом в последнем кадре

        cbg = ReentrantCallbackGroup()
        # RELIABLE: облако 1280x800 ~32 МБ, BEST_EFFORT теряет его целиком
        self.create_subscription(
            PointCloud2, g('points_topic'), self.on_cloud,
            QoSProfile(depth=1, reliability=ReliabilityPolicy.RELIABLE),
            callback_group=cbg)
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.create_subscription(String, '/vacuum_gripper/held', self.on_held, latched,
                                 callback_group=cbg)
        self.pub = self.create_publisher(DetectedObjectArray, '/objects', latched)
        self.pub_markers = self.create_publisher(MarkerArray, '/objects/markers', 10)
        ActionServer(self, DetectObjects, '/detect_objects', self.on_detect,
                     callback_group=cbg)
        if g('period') > 0:
            self.create_timer(g('period'), self.on_timer, callback_group=cbg)

    # ---------------------------------------------------------------- входы
    def on_cloud(self, msg):
        with self.lock:
            self.cloud = msg
            self.cloud_time = time.monotonic()

    def on_held(self, msg):
        if msg.data and not self.held:
            # захват: объект у присоски — тот, что сейчас уедет на новое место
            tcp = self.tcp_position()
            if tcp is not None and self.known:
                kid, (_, kc) = min(self.known.items(),
                                   key=lambda kv: np.linalg.norm(kv[1][1] - tcp[:2]))
                if np.linalg.norm(kc - tcp[:2]) < 0.08:
                    self.moving.add(kid)
                    self.get_logger().info(f'{kid} в присоске — id сохранится на новом месте')
        self.held = msg.data

    def on_timer(self):
        res, why = self.detect(max_age=3.0)
        if res is None and why.startswith('обзор частичный'):
            return                    # рука работает низко — сцену не трогаем
        if res is not None:
            self.publish(res)
        else:
            self.get_logger().warning(f'объекты не распознаны: {why}',
                                      throttle_duration_sec=10.0)

    def on_detect(self, goal_handle):
        fb = DetectObjects.Feedback(stage='жду свежее облако')
        goal_handle.publish_feedback(fb)
        t0 = time.monotonic()
        while time.monotonic() - t0 < 5.0:      # облако, пришедшее после запроса
            with self.lock:
                fresh = self.cloud is not None and self.cloud_time > t0
            if fresh:
                break
            time.sleep(0.05)
        goal_handle.publish_feedback(DetectObjects.Feedback(stage='распознаю'))
        res, why = self.detect(max_age=5.0)
        result = DetectObjects.Result()
        if res is None:
            goal_handle.abort()
            result.success, result.message = False, why
            return result
        if goal_handle.request.update_scene:
            self.publish(res)
        goal_handle.succeed()
        result.success = True
        result.message = f'объектов: {len(res.objects)}'
        result.objects = res
        return result

    # ---------------------------------------------------------- распознавание
    def detect(self, max_age):
        """(DetectedObjectArray, '') или (None, причина)."""
        g = lambda n: self.get_parameter(n).value  # noqa: E731
        with self.lock:
            msg, t = self.cloud, self.cloud_time
        if msg is None:
            return None, 'облако ещё не приходило'
        if time.monotonic() - t > max_age:
            return None, f'облако устарело ({time.monotonic() - t:.1f} с)'
        try:
            tf = self.tf_buffer.lookup_transform(self.base_frame, msg.header.frame_id,
                                                 rclpy.time.Time())
        except Exception as e:  # noqa: BLE001 — tf2 бросает разные типы
            return None, f'нет TF {self.base_frame} <- {msg.header.frame_id}: {e}'
        q, tr = tf.transform.rotation, tf.transform.translation
        cam = np.array([tr.x, tr.y, tr.z])
        pts = cloud_xyz(msg)
        pts = pts[np.linalg.norm(pts, axis=1) < g('max_range')]
        pts = P.decimate(pts, g('max_points'))
        pts = pts @ quat_to_rot(q.x, q.y, q.z, q.w).T + cam

        table = P.fit_table(pts, g('table_height'), g('table_search'))
        if table is None:
            return None, (f'стол не найден около table_height={g("table_height"):.3f} '
                          f'(камера не смотрит на стол?)')
        coef, sigma, _ = table
        self.view_height = float(cam[2] - (coef[0] * cam[0] + coef[1] * cam[1] + coef[2]))
        vmin = g('min_view_height')
        if self.view_height < vmin:
            # до классификации и assign_ids: частичные кадры портили память id
            return None, (f'обзор частичный: камера {self.view_height:.2f} м над столом '
                          f'(< {vmin:.2f}), нужна поза наблюдения')
        tol = max(0.01, 3.0 * sigma)
        clusters = P.segment(pts, coef, tol, g('max_height'), g('voxel'), g('min_points'))

        tcp = self.tcp_position() if self.held else None
        out = DetectedObjectArray()
        out.header.frame_id = self.base_frame
        out.header.stamp = msg.header.stamp
        out.table_z = float(coef[2])
        found = []
        for c in clusters:
            f = P.fit_shape(c, coef)
            if f is None:
                continue
            if tcp is not None and np.linalg.norm(f['center'] - tcp) < g('held_exclude_radius'):
                continue                       # объект в присоске
            label, conf = P.classify(f, self.rules)
            found.append((f, label, conf, c.shape[0]))
        for f, label, conf, n in found:
            out.objects.append(self.to_msg(f, label, conf, n))
        self.assign_ids(out.objects)
        return out, ''

    def tcp_position(self):
        try:
            t = self.tf_buffer.lookup_transform(self.base_frame, self.tcp_frame,
                                                rclpy.time.Time()).transform.translation
            return np.array([t.x, t.y, t.z])
        except Exception:  # noqa: BLE001
            return None

    @staticmethod
    def to_msg(f, label, conf, n):
        o = DetectedObject()
        o.label, o.confidence, o.num_points = label, float(conf), int(n)
        o.shape = f['shape']
        o.dimensions = [float(v) for v in f['dims']]
        cz, sz = math.cos(f['yaw'] / 2), math.sin(f['yaw'] / 2)
        o.pose.position.x, o.pose.position.y, o.pose.position.z = map(float, f['center'])
        o.pose.orientation.z, o.pose.orientation.w = sz, cz
        o.grasp_pose.position.x = float(f['center'][0])
        o.grasp_pose.position.y = float(f['center'][1])
        o.grasp_pose.position.z = float(f['top_z'])
        (o.grasp_pose.orientation.x, o.grasp_pose.orientation.y,
         o.grasp_pose.orientation.z, o.grasp_pose.orientation.w) = P.grasp_quat(f['yaw'])
        return o

    def assign_ids(self, objs):
        """Устойчивые id и классы.

        (1) Объект в пределах match_radius от известного места — тот же id и тот
            же класс, даже если в этом кадре класс другой (гистерезис: класс
            меняется, только если новый держится label_frames кадров подряд;
            разовая ошибка — маленький цилиндр как коробка — иначе уводила id).
        (2) Иначе — id того же класса, унесённый присоской (self.moving).
        (3) Иначе новый id.
        """
        r = self.get_parameter('match_radius').value
        need = self.get_parameter('label_frames').value
        used, pending = set(), []
        for o in objs:
            c = np.array([o.pose.position.x, o.pose.position.y])
            cand = [(np.linalg.norm(c - kc), kid) for kid, (kl, kc) in self.known.items()
                    if kid not in used and np.linalg.norm(c - kc) < r]
            if not cand:
                pending.append((o, c))
                continue
            kid = min(cand)[1]
            klabel = self.known[kid][0]
            if o.label != klabel:
                vl, vn = self.votes.get(kid, (o.label, 0))
                vn = vn + 1 if vl == o.label else 1
                if vn < need:                     # пока считаем прежним классом
                    self.votes[kid] = (o.label, vn)
                    o.label, o.confidence = klabel, 0.5 * o.confidence
                else:                             # новый класс устоялся — новый объект
                    self.votes.pop(kid, None)
                    self.known.pop(kid, None)
                    pending.append((o, c))
                    continue
            else:
                self.votes.pop(kid, None)
            o.id = kid
            used.add(kid)
        for o, c in pending:                      # (2) переставленный / (3) новый
            cand = [kid for kid in self.moving
                    if kid in self.known and self.known[kid][0] == o.label
                    and kid not in used]
            if cand and not self.held:            # объект уже поставлен
                o.id = cand[0]
                self.moving.discard(o.id)
            else:
                k = self.counters.get(o.label, 0)
                self.counters[o.label] = k + 1
                o.id = f'{o.label}_{k}'
            used.add(o.id)
        for o in objs:
            self.known[o.id] = (o.label, np.array([o.pose.position.x, o.pose.position.y]))

    # -------------------------------------------------------------- вывод
    def publish(self, res):
        self.pub.publish(res)
        arr = MarkerArray()
        clear = Marker()
        clear.action = Marker.DELETEALL
        arr.markers.append(clear)
        for i, o in enumerate(res.objects):
            m = Marker()
            m.header = res.header
            m.ns, m.id = 'objects', i
            m.type = Marker.CUBE if o.shape == P.SHAPE_BOX else Marker.CYLINDER
            m.pose = o.pose
            if o.shape == P.SHAPE_BOX:
                m.scale.x, m.scale.y, m.scale.z = o.dimensions
            else:
                m.scale.x = m.scale.y = 2 * o.dimensions[1]
                m.scale.z = o.dimensions[0]
            known = o.label in self.rules
            m.color.r, m.color.g, m.color.b = (0.2, 0.6, 1.0) if known else (1.0, 0.3, 0.3)
            m.color.a = 0.5
            t = Marker()
            t.header = res.header
            t.ns, t.id, t.type = 'object_labels', i, Marker.TEXT_VIEW_FACING
            t.pose = Pose()
            t.pose.position.x, t.pose.position.y = o.pose.position.x, o.pose.position.y
            t.pose.position.z = o.grasp_pose.position.z + 0.04
            t.pose.orientation.w = 1.0
            t.scale.z = 0.022
            t.color.r = t.color.g = t.color.b = t.color.a = 1.0
            dims = ' x '.join(f'{v * 100:.0f}' for v in o.dimensions)
            t.text = f'{o.id} ({o.confidence:.0%})\n{dims} см'
            arr.markers += [m, t]
        self.pub_markers.publish(arr)


def main():
    rclpy.init()
    node = ObjectPerception()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(node)
    try:
        while rclpy.ok():
            try:
                ex.spin()
            except (KeyboardInterrupt, ExternalShutdownException):
                break
            except Exception:  # noqa: BLE001 — восприятие не должно падать
                node.get_logger().error('исключение, продолжаю:\n' + traceback.format_exc())
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
