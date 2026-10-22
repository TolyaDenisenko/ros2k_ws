#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
scene_manager — модель мира для захвата: Planning Scene MoveIt + зоны установки.

1. Planning Scene (сервис /apply_planning_scene move_group):
   * объекты из /objects (object_perception) -> CollisionObject (id = id объекта):
     MoveIt объезжает их и не сбивает соседей при подходе;
   * стол -> CollisionObject 'table' (верх чуть ниже найденной плоскости);
   * камера гриппера с кронштейном -> AttachedCollisionObject к link_eef: в
     модели MoveIt её нет (moveit_configs_builder строит только руку);
   * захват: /vacuum_gripper/held стал непустым -> ближайший к TCP объект
     прикрепляется к link_tcp (MoveIt учитывает его при переносе); отпустили ->
     открепляется и остаётся в сцене там, где его поставили.
2. Зоны установки (config/place_zones.yaml): маркеры /place_zones/markers и
   action /find_place_pose (xarm_grasping_msgs/action/FindPlacePose) — свободное
   место в зоне для объекта: сетка точек внутри зоны, коробка вдоль или поперёк
   зоны, объект целиком в зоне, до контуров других объектов (прямоугольник с
   поворотом / круг) не меньше clearance; из свободных — плотной укладкой от
   ближнего к базе конца зоны.

Параметры
  base_frame link_base  tcp_frame link_tcp  zones_file ''  rules_file ''
  (пусто — share/xarm_grasping/config/{place_zones,objects}.yaml)
  table_center [0.3, 0.0]  table_size [1.0, 1.2] (стол для MoveIt в base_frame;
  дефолт — arm_table.world)
  attach_radius 0.08 (объект ближе к TCP — тот, что в присоске)
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
from geometry_msgs.msg import Point, Pose, PoseStamped
from moveit_msgs.msg import AttachedCollisionObject, CollisionObject, PlanningScene
from moveit_msgs.srv import ApplyPlanningScene
from rclpy.action import ActionServer
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from shape_msgs.msg import SolidPrimitive
from std_msgs.msg import String
from tf2_ros import Buffer, TransformListener
from visualization_msgs.msg import Marker, MarkerArray

from xarm_grasping_msgs.action import FindPlacePose
from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray

# Камера Gemini 335Le на кронштейне в кадре link_eef (quasi_camera_description,
# gripper_camera.xacro): x 0.046..0.0755, y ±0.062, z 0.003..0.053 (+ запас 5 мм)
CAMERA_BOX_CENTER = (0.061, 0.0, 0.028)
CAMERA_BOX_SIZE = (0.040, 0.134, 0.060)
GRIPPER_TOUCH_LINKS = ['link6', 'link_eef', 'link_tcp', 'xarm_vacuum_gripper_link']


def primitive_of(o: DetectedObject):
    p = SolidPrimitive()
    if o.shape == DetectedObject.SHAPE_BOX:
        p.type = SolidPrimitive.BOX
        p.dimensions = list(o.dimensions)
    else:
        p.type = SolidPrimitive.CYLINDER
        p.dimensions = list(o.dimensions)         # [height, radius]
    return p


def footprint_radius(o: DetectedObject):
    """Радиус описанной окружности объекта в плане."""
    if o.shape == DetectedObject.SHAPE_BOX:
        return 0.5 * math.hypot(o.dimensions[0], o.dimensions[1])
    return o.dimensions[1]


def yaw_of(o: DetectedObject):
    q = o.pose.orientation
    return math.atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))


def footprint(o: DetectedObject, xy=None, yaw=None):
    """Контур в плане: ('rect', центр, полуразмеры, yaw) или ('circle', центр, r)."""
    c = np.array([o.pose.position.x, o.pose.position.y]) if xy is None else np.asarray(xy)
    if o.shape == DetectedObject.SHAPE_BOX:
        return ('rect', c, np.array(o.dimensions[:2]) / 2,
                yaw_of(o) if yaw is None else yaw)
    return ('circle', c, o.dimensions[1])


def _rect_axes(yaw):
    c, s = math.cos(yaw), math.sin(yaw)
    return np.array([[c, s], [-s, c]])        # строки — оси прямоугольника


def gap(a, b):
    """Расстояние между контурами a и b (<0 — пересекаются), приближённо точное:
    круг-круг и круг-прямоугольник точно; прямоугольник-прямоугольник — по
    теореме о разделяющей оси (наибольший зазор по 4 осям)."""
    if a[0] == 'circle' and b[0] == 'circle':
        return np.linalg.norm(a[1] - b[1]) - a[2] - b[2]
    if a[0] == 'circle':
        a, b = b, a
    if b[0] == 'circle':                      # a — прямоугольник
        p = _rect_axes(a[3]) @ (b[1] - a[1])
        d = np.maximum(np.abs(p) - a[2], 0.0)
        inside = np.all(np.abs(p) <= a[2])
        return (-b[2] if inside else np.linalg.norm(d) - b[2])
    best = -np.inf
    for axes in (_rect_axes(a[3]), _rect_axes(b[3])):
        for ax in axes:
            ra = sum(abs(ax @ v) * h for v, h in zip(_rect_axes(a[3]), a[2]))
            rb = sum(abs(ax @ v) * h for v, h in zip(_rect_axes(b[3]), b[2]))
            best = max(best, abs(ax @ (b[1] - a[1])) - ra - rb)
    return best


def height_of(o: DetectedObject):
    return o.dimensions[2] if o.shape == DetectedObject.SHAPE_BOX else o.dimensions[0]


class SceneManager(Node):
    def __init__(self):
        super().__init__('scene_manager')
        dp = self.declare_parameter
        dp('base_frame', 'link_base')
        dp('tcp_frame', 'link_tcp')
        dp('zones_file', '')
        dp('rules_file', '')
        dp('table_center', [0.3, 0.0])
        dp('table_size', [1.0, 1.2])
        dp('attach_radius', 0.08)
        g = lambda n: self.get_parameter(n).value  # noqa: E731
        self.base_frame, self.tcp_frame = g('base_frame'), g('tcp_frame')
        share = get_package_share_directory('xarm_grasping')
        with open(g('zones_file') or os.path.join(share, 'config', 'place_zones.yaml')) as f:
            z = yaml.safe_load(f)
        self.zones = z['zones']
        self.search_step = float(z.get('search_step', 0.01))
        self.clearance = float(z.get('clearance', 0.03))
        with open(g('rules_file') or os.path.join(share, 'config', 'objects.yaml')) as f:
            self.rules = yaml.safe_load(f)['objects']

        self.lock = threading.Lock()
        self.objects = {}            # id -> DetectedObject (последний /objects)
        self.table_z = None
        self.scene_ids = set()       # объекты, которые сейчас в мире Planning Scene
        self.attached_id = ''        # объект в присоске (прикреплён в сцене)
        self.camera_added = False

        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)
        cbg = ReentrantCallbackGroup()
        self.scene_cli = self.create_client(ApplyPlanningScene, '/apply_planning_scene',
                                            callback_group=cbg)
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.create_subscription(DetectedObjectArray, '/objects', self.on_objects, latched,
                                 callback_group=cbg)
        self.create_subscription(String, '/vacuum_gripper/held', self.on_held, latched,
                                 callback_group=cbg)
        self.pub_zones = self.create_publisher(MarkerArray, '/place_zones/markers', latched)
        ActionServer(self, FindPlacePose, '/find_place_pose', self.on_find_place,
                     callback_group=cbg)
        self.create_timer(1.0, self.on_timer, callback_group=cbg)
        self.publish_zones()
        self.get_logger().info(f'scene_manager: зоны {list(self.zones)}')

    # ------------------------------------------------------- Planning Scene
    def apply(self, scene: PlanningScene, what=''):
        if not self.scene_cli.service_is_ready():
            return False
        scene.is_diff = True
        scene.robot_state.is_diff = True
        fut = self.scene_cli.call_async(ApplyPlanningScene.Request(scene=scene))
        t0 = time.monotonic()
        while not fut.done() and time.monotonic() - t0 < 3.0:
            time.sleep(0.01)
        ok = fut.done() and fut.result() is not None and fut.result().success
        if not ok:
            self.get_logger().warning(f'Planning Scene не обновлена ({what})')
        return ok

    def co(self, oid, primitive=None, pose=None, op=CollisionObject.ADD):
        c = CollisionObject()
        c.header.frame_id = self.base_frame
        c.id, c.operation = oid, op
        if primitive is not None:
            c.primitives, c.primitive_poses = [primitive], [pose]
        return c

    def on_timer(self):
        if not self.camera_added and self.scene_cli.service_is_ready():
            self.add_camera()
        self.publish_zones()

    def add_camera(self):
        """Камера с кронштейном — прикреплённое тело на link_eef (нет в модели MoveIt)."""
        box = SolidPrimitive(type=SolidPrimitive.BOX, dimensions=list(CAMERA_BOX_SIZE))
        pose = Pose()
        pose.position = Point(x=CAMERA_BOX_CENTER[0], y=CAMERA_BOX_CENTER[1],
                              z=CAMERA_BOX_CENTER[2])
        pose.orientation.w = 1.0
        a = AttachedCollisionObject(link_name='link_eef', touch_links=GRIPPER_TOUCH_LINKS)
        a.object = self.co('gripper_camera', box, pose)
        a.object.header.frame_id = 'link_eef'
        s = PlanningScene()
        s.robot_state.attached_collision_objects = [a]
        self.camera_added = self.apply(s, 'камера гриппера')
        if self.camera_added:
            self.get_logger().info('камера гриппера добавлена в Planning Scene (link_eef)')

    def on_objects(self, msg: DetectedObjectArray):
        with self.lock:
            self.objects = {o.id: o for o in msg.objects}
            self.table_z = msg.table_z
            attached = self.attached_id
        s = PlanningScene()
        # стол: верх на 3 мм ниже плоскости — база руки стоит на столе и не должна
        # «касаться» его в проверке столкновений
        tc, ts = self.get_parameter('table_center').value, self.get_parameter('table_size').value
        tp = Pose()
        tp.position = Point(x=float(tc[0]), y=float(tc[1]), z=msg.table_z - 0.003 - 0.025)
        tp.orientation.w = 1.0
        s.world.collision_objects.append(self.co(
            'table', SolidPrimitive(type=SolidPrimitive.BOX,
                                    dimensions=[float(ts[0]), float(ts[1]), 0.05]), tp))
        ids = set()
        for o in msg.objects:
            if o.id == attached:
                continue
            s.world.collision_objects.append(self.co(o.id, primitive_of(o), o.pose))
            ids.add(o.id)
        for old in self.scene_ids - ids - {attached}:
            s.world.collision_objects.append(self.co(old, op=CollisionObject.REMOVE))
        if self.apply(s, 'объекты'):
            self.scene_ids = ids

    def on_held(self, msg: String):
        if msg.data and not self.attached_id:
            self.attach_nearest()
        elif not msg.data and self.attached_id:
            self.detach()

    def tcp_position(self):
        try:
            t = self.tf_buffer.lookup_transform(self.base_frame, self.tcp_frame,
                                                rclpy.time.Time()).transform.translation
            return np.array([t.x, t.y, t.z])
        except Exception:  # noqa: BLE001
            return None

    def attach_nearest(self):
        tcp = self.tcp_position()
        with self.lock:
            objs = list(self.objects.values())
        if tcp is None or not objs:
            self.get_logger().warning('вакуум держит объект, но прикрепить в сцене нечего '
                                      '(нет /objects или TF)')
            return
        d = [np.linalg.norm(np.array([o.grasp_pose.position.x, o.grasp_pose.position.y,
                                      o.grasp_pose.position.z]) - tcp) for o in objs]
        i = int(np.argmin(d))
        if d[i] > self.get_parameter('attach_radius').value:
            self.get_logger().warning(f'ближайший объект {objs[i].id} в {d[i]:.3f} м от TCP — '
                                      f'не прикрепляю')
            return
        o = objs[i]
        a = AttachedCollisionObject(link_name=self.tcp_frame, touch_links=GRIPPER_TOUCH_LINKS)
        a.object = self.co(o.id, primitive_of(o), o.pose)   # поза в base_frame — MoveIt
        s = PlanningScene()                                  # пересчитает к link_tcp
        s.robot_state.attached_collision_objects = [a]       # из мира MoveIt уберёт сам
        if self.apply(s, f'прикрепить {o.id}'):
            self.attached_id = o.id
            self.scene_ids.discard(o.id)
            self.get_logger().info(f'{o.id} прикреплён к {self.tcp_frame}')

    def detach(self):
        a = AttachedCollisionObject(link_name=self.tcp_frame)
        a.object = self.co(self.attached_id, op=CollisionObject.REMOVE)
        s = PlanningScene()
        s.robot_state.attached_collision_objects = [a]
        if self.apply(s, f'открепить {self.attached_id}'):
            # MoveIt возвращает объект в мир там, где он отпущен; уточнит следующий /objects
            self.get_logger().info(f'{self.attached_id} откреплён')
            self.scene_ids.add(self.attached_id)
            self.attached_id = ''

    # ------------------------------------------------------------- зоны
    def zone_for(self, o: DetectedObject):
        cat = self.rules.get(o.label, {}).get('category', o.label)
        for name, z in self.zones.items():
            acc = z.get('accepts', [])
            if o.label in acc or cat in acc:
                return name
        return None

    def on_find_place(self, goal_handle):
        req = goal_handle.request
        res = FindPlacePose.Result()
        ok, msg, zone, pose = self.find_place(req.object_id, req.zone)
        res.success, res.message, res.zone = ok, msg, zone
        if ok:
            res.place_pose = pose
            goal_handle.succeed()
        else:
            goal_handle.abort()
        return res

    def find_place(self, object_id, zone=''):
        """(ok, сообщение, зона, PoseStamped центра объекта на месте)."""
        with self.lock:
            o = self.objects.get(object_id)
            others = [v for k, v in self.objects.items() if k != object_id]
            table_z = self.table_z
        if o is None:
            return False, f'объект {object_id} не найден в /objects', '', None
        zone = zone or self.zone_for(o)
        if not zone or zone not in self.zones:
            return False, f'нет зоны для {o.label} (place_zones.yaml)', zone or '', None
        z = self.zones[zone]
        cx, cy = z['center']
        sx, sy = z['size']
        # коробку — вдоль X зоны (yaw 0) или поперёк (yaw π/2), что лучше; цилиндр — как есть
        yaws = [0.0, math.pi / 2] if o.shape == DetectedObject.SHAPE_BOX else [0.0]
        others_fp = [footprint(v) for v in others]
        free, free_yaw = [], []
        for yaw in yaws:
            half = (np.array(o.dimensions[:2]) / 2 if o.shape == DetectedObject.SHAPE_BOX
                    else np.array([o.dimensions[1]] * 2))
            if yaw:
                half = half[::-1]
            if np.any(2 * half > [sx, sy]):
                continue
            for x in np.arange(cx - sx / 2 + half[0], cx + sx / 2 - half[0] + 1e-9,
                               self.search_step):
                for y in np.arange(cy - sy / 2 + half[1], cy + sy / 2 - half[1] + 1e-9,
                                   self.search_step):
                    fp = footprint(o, (x, y), yaw)
                    if all(gap(fp, f) > self.clearance for f in others_fp):
                        free.append((x, y))
                        free_yaw.append(yaw)
        if not free:
            return False, f'в {zone} нет свободного места для {o.id}', zone, None
        free = np.array(free)
        # плотная укладка рядами от ближнего к базе конца зоны (по X), в ряду —
        # от края зоны, обращённого к базе (по |Y|); без поворота — при прочих
        # равных (штраф 1 см). Раньше ставили ближе к центру/удобной полосе —
        # первый объект занимал середину, и у края оставалось неиспользуемое место.
        cost = ((free[:, 0] - (cx - sx / 2))
                + 0.3 * np.abs(np.abs(free[:, 1]) - (abs(cy) - sy / 2))
                + 0.01 * (np.array(free_yaw) > 0))
        k = int(np.argmin(cost))
        best, best_yaw = free[k], free_yaw[k]
        ps = PoseStamped()
        ps.header.frame_id = self.base_frame
        ps.pose.position.x, ps.pose.position.y = float(best[0]), float(best[1])
        ps.pose.position.z = float(table_z + height_of(o) / 2)
        ps.pose.orientation.z = math.sin(best_yaw / 2)
        ps.pose.orientation.w = math.cos(best_yaw / 2)
        return (True, f'{o.id} -> {zone} ({best[0]:.3f}, {best[1]:.3f}, '
                      f'поворот {math.degrees(best_yaw):.0f}°)', zone, ps)

    def publish_zones(self):
        arr = MarkerArray()
        z0 = (self.table_z if self.table_z is not None else 0.0) + 0.001
        for i, (name, z) in enumerate(self.zones.items()):
            col = z.get('color', [0.3, 0.8, 0.3])
            m = Marker()
            m.header.frame_id = self.base_frame
            m.ns, m.id, m.type = 'place_zones', i, Marker.CUBE
            m.pose.position = Point(x=float(z['center'][0]), y=float(z['center'][1]), z=z0)
            m.pose.orientation.w = 1.0
            m.scale.x, m.scale.y, m.scale.z = float(z['size'][0]), float(z['size'][1]), 0.002
            m.color.r, m.color.g, m.color.b, m.color.a = col[0], col[1], col[2], 0.35
            t = Marker()
            t.header.frame_id = self.base_frame
            t.ns, t.id, t.type = 'place_zone_labels', i, Marker.TEXT_VIEW_FACING
            t.pose.position = Point(x=float(z['center'][0]), y=float(z['center'][1]), z=z0 + 0.03)
            t.pose.orientation.w = 1.0
            t.scale.z = 0.025
            t.color.r = t.color.g = t.color.b = t.color.a = 1.0
            t.text = f"{name}: {', '.join(z.get('accepts', []))}"
            arr.markers += [m, t]
        self.pub_zones.publish(arr)


def main():
    rclpy.init()
    node = SceneManager()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(node)
    try:
        while rclpy.ok():
            try:
                ex.spin()
            except (KeyboardInterrupt, ExternalShutdownException):
                break
            except Exception:  # noqa: BLE001
                node.get_logger().error('исключение, продолжаю:\n' + traceback.format_exc())
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
