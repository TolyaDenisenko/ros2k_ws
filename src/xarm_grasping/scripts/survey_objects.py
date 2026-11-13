#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
survey_objects — осмотр стола с разных сторон перед взятием: рука проводит камеру вокруг всех
предметов под углом ~45°, чтобы ТОЧНО знать, где каждый предмет и какой он.

Зачем: в момент взятия камера слепая — она стоит на ~7 см выше присоски и не видит ближе 0.25 м.
Значит, где предмет, надо знать заранее. Из одной позы наблюдения (почти сверху) видна только
верхняя грань и ближние стенки: центр и размеры получаются с ошибкой, особенно у высоких предметов и
у стоящих вплотную. Осмотр с нескольких сторон даёт облако точек со ВСЕХ стенок предмета, по нему
центр, поворот и размеры считаются заново. Заодно нейросеть голосует по классу в каждом ракурсе
(сверху куб похож на квадратную пластину, сбоку — нет).

Как работает (сервис /survey_objects, std_srvs/Trigger):
  1. берёт последние /objects (облако точек из позы наблюдения: где предметы и их размеры);
  2. считает центр всех предметов и ставит камеру в N точек по дуге вокруг него, под углом
     elevation_deg к столу, на таком расстоянии, чтобы все предметы влезли в кадр. Позу камеры в углы
     суставов переводит MoveIt (/compute_ik), едет /move_action (с объездом стола);
  3. в каждом ракурсе ждёт свежие рамки нейросети (/detections) и сопоставляет их с предметами:
     3D-габарит предмета проецируется в кадр, рамка с наибольшим IoU (≥ min_iou) — его;
  4. возвращает руку в позу наблюдения;
  5. ГЕОМЕТРИЯ (параметр geometry):
     top (по умолчанию) — центр, размеры и точка хвата остаются из вида сверху. Проверено в
       симуляции (tools/check_survey_accuracy.py, 2026-11-13): сверху центр точен до ~2 мм, а
       объединение ракурсов давало 3–7 мм и раздувало размеры на 1.5–4 см — на наклонных видах
       точки на границе предмета и стола («летящие» пиксели) тянутся наружу;
     merge — в каждом ракурсе (и в стартовом, сверху) берётся облако /gripper_camera/points,
     переводится в base_frame; точки над столом делятся между предметами (каждая — ближайшему, не
     дальше margin от его контура); по объединённым точкам предмета заново подгоняются форма, центр,
     поворот, размеры и точка хвата (perception.fit_shape, как в object_perception);
  6. КЛАСС: суммирует уверенность нейросети по классам во всех ракурсах (и сверху); итог — класс с
     наибольшей суммой, уверенность = сумма / число ракурсов, где предмет был в кадре.

Выход
  /objects_refined   xarm_grasping_msgs/DetectedObjectArray (transient_local) — те же предметы (те же
                     id), с уточнёнными pose / dimensions / grasp_pose (если точек хватило) и
                     detector_label / detector_score по голосованию. pick_place_server берёт отсюда
                     геометрию для захвата;
  /survey/status     std_msgs/String — ход осмотра и итог текстом;
  /survey/votes      std_msgs/String — JSON: по каждому предмету голоса по ракурсам (для разбора).

Параметры
  views 5   arc_deg 140 (ширина дуги: за предметы рука не дотягивается)   elevation_deg 45
  min_dist 0.35  max_dist 0.45 (м, камера — центр предметов; дальше рука не достаёт)   settle 1.0 (с после остановки)
  min_iou 0.3   margin 0.015 (м, точки дальше от контура предмета — не его)   voxel 0.005
  min_points 150 (меньше точек у предмета — геометрию не меняем)   points_topic /gripper_camera/points
  observe_joints [0, -0.596, -1.013, 0, 1.609, 0]   group xarm6   tip_link link_tcp
  base_frame link_base   optical_frame gripper_camera_color_optical_frame   detections_topic /detections
Нужны: MoveIt (use_moveit:=true) и нейросеть (detector:=true).
use_sim_time не нужен (свежесть рамок — по времени прихода), поэтому исполнитель многопоточный дешёвый.
"""
import json
import math
import threading
import time

import numpy as np
import rclpy
from geometry_msgs.msg import PoseStamped
from moveit_msgs.action import MoveGroup
from moveit_msgs.msg import Constraints, JointConstraint, MotionPlanRequest, MoveItErrorCodes
from moveit_msgs.srv import GetPositionIK
from rclpy.action import ActionClient
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from sensor_msgs.msg import CameraInfo, PointCloud2
from std_msgs.msg import Bool, String
from std_srvs.srv import Trigger
from tf2_ros import Buffer, TransformListener
from vision_msgs.msg import Detection2DArray

from xarm_grasping import perception as P
from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray

JOINTS = [f'joint{i}' for i in range(1, 7)]


def quat_rot(x, y, z, w):
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def rot_quat(R):
    t = np.trace(R)
    if t > 0:
        s = math.sqrt(t + 1.0) * 2
        return ((R[2, 1] - R[1, 2]) / s, (R[0, 2] - R[2, 0]) / s, (R[1, 0] - R[0, 1]) / s, 0.25 * s)
    i = int(np.argmax(np.diag(R)))
    j, k = (i + 1) % 3, (i + 2) % 3
    s = math.sqrt(1.0 + R[i, i] - R[j, j] - R[k, k]) * 2
    q = [0.0, 0.0, 0.0, (R[k, j] - R[j, k]) / s]
    q[i], q[j], q[k] = 0.25 * s, (R[j, i] + R[i, j]) / s, (R[k, i] + R[i, k]) / s
    return tuple(q)


def look_at(cam, target):
    """Оптический кадр (z вперёд, x вправо, y вниз), смотрящий из cam на target."""
    z = target - cam
    z = z / np.linalg.norm(z)
    x = np.cross(z, [0.0, 0.0, 1.0])
    x = x / np.linalg.norm(x)
    return np.stack([x, np.cross(z, x), z], 1)


def object_corners(o):
    """Точки габарита предмета в link_base (для проекции в кадр)."""
    p, q = o.pose.position, o.pose.orientation
    R = quat_rot(q.x, q.y, q.z, q.w)
    c = np.array([p.x, p.y, p.z])
    d = list(o.dimensions)
    if o.shape == DetectedObject.SHAPE_CYLINDER and len(d) >= 2:
        h, r = d[0], d[1]
        loc = [(r * math.cos(a), r * math.sin(a), z) for a in np.linspace(0, 2 * math.pi, 16, endpoint=False)
               for z in (-h / 2, h / 2)]
    else:
        sx, sy, sz = d[:3]
        loc = [(a * sx / 2, b * sy / 2, e * sz / 2) for a in (-1, 1) for b in (-1, 1) for e in (-1, 1)]
    return c + np.array(loc) @ R.T


def cloud_xyz(msg):
    """(N, 3) float из PointCloud2 (x, y, z float32), без NaN."""
    off = {f.name: f.offset for f in msg.fields}
    raw = np.frombuffer(msg.data, dtype=np.uint8).reshape(-1, msg.point_step)[:msg.width * msg.height]
    pts = np.column_stack([raw[:, off[c]:off[c] + 4].copy().view(np.float32).ravel() for c in 'xyz'])
    return pts[np.isfinite(pts).all(axis=1)].astype(float)


def footprint_dist(o, xy):
    """Расстояние (м) от точек xy до контура предмета на столе (0 — внутри)."""
    p, q = o.pose.position, o.pose.orientation
    d = list(o.dimensions)
    rel = xy - np.array([p.x, p.y])
    if o.shape == DetectedObject.SHAPE_CYLINDER and len(d) >= 2:
        return np.maximum(np.linalg.norm(rel, axis=1) - d[1], 0.0)
    yaw = 2 * math.atan2(q.z, q.w)
    c, s = math.cos(yaw), math.sin(yaw)
    lx, ly = rel[:, 0] * c + rel[:, 1] * s, -rel[:, 0] * s + rel[:, 1] * c
    ex, ey = np.maximum(np.abs(lx) - d[0] / 2, 0.0), np.maximum(np.abs(ly) - d[1] / 2, 0.0)
    return np.hypot(ex, ey)


def drop_sparse(pts, radius, k):
    """Убрать точки, у которых меньше k соседей в радиусе radius (по сетке, без KD-дерева)."""
    if len(pts) < k + 1:
        return pts
    cell = np.floor(pts / radius).astype(np.int64)
    keys, inv, cnt = np.unique(cell, axis=0, return_inverse=True, return_counts=True)
    # соседей считаем по 27 соседним ячейкам
    lut = {tuple(kk): c for kk, c in zip(keys, cnt)}
    off = [(a, b, c) for a in (-1, 0, 1) for b in (-1, 0, 1) for c in (-1, 0, 1)]
    neigh = np.array([sum(lut.get((kk[0] + a, kk[1] + b, kk[2] + c), 0) for a, b, c in off) for kk in keys])
    return pts[neigh[inv.ravel()] - 1 >= k]


def iou(a, b):
    w = max(0.0, min(a[2], b[2]) - max(a[0], b[0]))
    h = max(0.0, min(a[3], b[3]) - max(a[1], b[1]))
    i = w * h
    return i / ((a[2] - a[0]) * (a[3] - a[1]) + (b[2] - b[0]) * (b[3] - b[1]) - i + 1e-9)


class Survey(Node):
    def __init__(self):
        super().__init__('survey_objects')
        dp = self.declare_parameter
        for k, v in (('views', 5), ('arc_deg', 140.0), ('elevation_deg', 45.0), ('min_dist', 0.35),
                     ('max_dist', 0.45), ('settle', 1.0), ('min_iou', 0.3),
                     ('observe_joints', [0.0, -0.596, -1.013, 0.0, 1.609, 0.0]), ('group', 'xarm6'),
                     ('tip_link', 'link_tcp'), ('base_frame', 'link_base'),
                     ('optical_frame', 'gripper_camera_color_optical_frame'),
                     ('camera_info_topic', '/gripper_camera/color/camera_info'),
                     ('detections_topic', '/detections'), ('velocity_scaling', 0.4),
                     ('points_topic', '/gripper_camera/points'), ('margin', 0.015), ('voxel', 0.005),
                     ('min_points', 150), ('debug_dir', ''), ('geometry', 'top')):
            dp(k, v)
        self.p = lambda k: self.get_parameter(k).value
        cbg = ReentrantCallbackGroup()
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL,
                             reliability=ReliabilityPolicy.RELIABLE)
        self.objects = None
        self.create_subscription(DetectedObjectArray, '/objects', lambda m: setattr(self, 'objects', m),
                                 latched, callback_group=cbg)
        self.dets, self.dets_t = None, 0.0
        self.create_subscription(Detection2DArray, self.p('detections_topic'), self.on_dets, 5,
                                 callback_group=cbg)
        self.cloud, self.cloud_t = None, 0.0
        self.create_subscription(PointCloud2, self.p('points_topic'), self.on_cloud, 1, callback_group=cbg)
        self.k = None
        self.create_subscription(CameraInfo, self.p('camera_info_topic'), self.on_info, 1, callback_group=cbg)
        self.pub = self.create_publisher(DetectedObjectArray, '/objects_refined', latched)
        self.pub_status = self.create_publisher(String, '/survey/status', latched)
        self.pub_votes = self.create_publisher(String, '/survey/votes', latched)
        self.pub_active = self.create_publisher(Bool, '/survey/active', latched)
        self.pub_active.publish(Bool(data=False))
        self.ik = self.create_client(GetPositionIK, '/compute_ik', callback_group=cbg)
        self.move = ActionClient(self, MoveGroup, '/move_action', callback_group=cbg)
        self.buf = Buffer()
        TransformListener(self.buf, self)
        self.busy = threading.Lock()
        self.create_service(Trigger, '/survey_objects', self.on_survey, callback_group=cbg)
        self.say('готов: ros2 service call /survey_objects std_srvs/srv/Trigger')

    # ------------------------------------------------------------ мелочи
    def say(self, text):
        self.get_logger().info(text)
        self.pub_status.publish(String(data=text))

    def on_info(self, m):
        self.k = (m.k[0], m.k[4], m.k[2], m.k[5], m.width, m.height)

    def on_cloud(self, m):
        self.cloud, self.cloud_t = m, time.monotonic()

    def grab_points(self, after, objs, table_z, acc):
        """Свежее облако -> base_frame -> точки над столом раздать предметам (в acc[id])."""
        end = time.monotonic() + 4.0
        while self.cloud_t < after and time.monotonic() < end:
            time.sleep(0.05)
        if self.cloud_t < after:
            return 0
        m = self.cloud
        # TF на момент СНИМКА облака: облако приходит с задержкой, и если рука в тот момент ещё
        # ехала, «последний» TF сдвинул бы точки
        try:
            t = self.buf.lookup_transform(self.p('base_frame'), m.header.frame_id, m.header.stamp,
                                          timeout=rclpy.duration.Duration(seconds=2.0))
        except Exception as e:  # noqa: BLE001
            self.get_logger().warning(f'нет TF на момент облака: {e}')
            return 0
        r, tr = t.transform.rotation, t.transform.translation
        R, T = quat_rot(r.x, r.y, r.z, r.w), np.array([tr.x, tr.y, tr.z])
        pts = cloud_xyz(m)
        pts = pts[np.linalg.norm(pts, axis=1) < 1.2] @ R.T + T
        pts = pts[(pts[:, 2] > table_z + 0.004) & (pts[:, 2] < table_z + 0.30)]
        if not len(pts):
            return 0
        D = np.stack([footprint_dist(o, pts[:, :2]) for o in objs], 1)     # (N, объекты)
        best = D.argmin(1)
        ok = D[np.arange(len(pts)), best] <= float(self.p('margin'))
        n = 0
        for k, o in enumerate(objs):
            sel = pts[ok & (best == k)]
            if len(sel):
                acc[o.id].append(sel)
                n += len(sel)
        if self.p('debug_dir'):                  # для разбора: точки ракурса и поза камеры
            import os
            os.makedirs(self.p('debug_dir'), exist_ok=True)
            np.savez(os.path.join(self.p('debug_dir'), f'view_{len(os.listdir(self.p("debug_dir"))):02d}.npz'),
                     pts=pts, R=R, T=T, ids=np.array([o.id for o in objs]), best=best, ok=ok)
        return n

    def on_dets(self, m):
        boxes = []
        for d in m.detections:
            if not d.results:
                continue
            h = d.results[0].hypothesis
            cx, cy = d.bbox.center.position.x, d.bbox.center.position.y
            boxes.append((cx - d.bbox.size_x / 2, cy - d.bbox.size_y / 2, cx + d.bbox.size_x / 2,
                          cy + d.bbox.size_y / 2, float(h.score), h.class_id))
        self.dets, self.dets_t = boxes, time.monotonic()

    @staticmethod
    def wait(fut, timeout):
        end = time.monotonic() + timeout
        while not fut.done() and time.monotonic() < end:
            time.sleep(0.02)
        return fut.result() if fut.done() else None

    def tf(self, target, source):
        t = self.buf.lookup_transform(target, source, rclpy.time.Time(), timeout=rclpy.duration.Duration(seconds=2.0))
        r, p = t.transform.rotation, t.transform.translation
        return quat_rot(r.x, r.y, r.z, r.w), np.array([p.x, p.y, p.z])

    # ------------------------------------------------------------ движение
    def goto_joints(self, values):
        if not self.move.wait_for_server(timeout_sec=5.0):
            return False
        req = MotionPlanRequest()
        req.group_name = self.p('group')
        req.num_planning_attempts, req.allowed_planning_time = 3, 5.0
        req.max_velocity_scaling_factor = req.max_acceleration_scaling_factor = float(self.p('velocity_scaling'))
        c = Constraints()
        for n, v in zip(JOINTS, values):
            c.joint_constraints.append(JointConstraint(joint_name=n, position=float(v), tolerance_above=0.005,
                                                       tolerance_below=0.005, weight=1.0))
        req.goal_constraints = [c]
        goal = MoveGroup.Goal(request=req)
        gh = self.wait(self.move.send_goal_async(goal), 10.0)
        if gh is None or not gh.accepted:
            return False
        r = self.wait(gh.get_result_async(), 60.0)
        return r is not None and r.result.error_code.val == MoveItErrorCodes.SUCCESS

    def camera_to(self, cam, target):
        """Камера в точку cam (base_frame), смотрит на target. True — доехала."""
        Rtc, ttc = self.tf(self.p('tip_link'), self.p('optical_frame'))
        Rc = look_at(cam, target)
        Rt = Rc @ Rtc.T
        pt = cam - Rt @ ttc
        if not self.ik.wait_for_service(timeout_sec=5.0):
            return False
        req = GetPositionIK.Request()
        req.ik_request.group_name = self.p('group')
        req.ik_request.ik_link_name = self.p('tip_link')
        req.ik_request.avoid_collisions = True
        req.ik_request.robot_state.is_diff = True      # начальное приближение — текущее состояние робота
        req.ik_request.timeout.sec = 1
        ps = PoseStamped()
        ps.header.frame_id = self.p('base_frame')
        ps.pose.position.x, ps.pose.position.y, ps.pose.position.z = map(float, pt)
        (ps.pose.orientation.x, ps.pose.orientation.y, ps.pose.orientation.z,
         ps.pose.orientation.w) = rot_quat(Rt)
        req.ik_request.pose_stamped = ps
        r = self.wait(self.ik.call_async(req), 5.0)
        if r is None or r.error_code.val != MoveItErrorCodes.SUCCESS:
            return False
        js = r.solution.joint_state
        return self.goto_joints([js.position[js.name.index(n)] for n in JOINTS])

    # ------------------------------------------------------------ осмотр
    def on_survey(self, req, res):
        if not self.busy.acquire(blocking=False):
            res.success, res.message = False, 'осмотр уже идёт'
            return res
        self.pub_active.publish(Bool(data=True))     # object_perception не публикует сцену с наклонных ракурсов
        try:
            res.success, res.message = self.survey()
        except Exception as e:  # noqa: BLE001
            res.success, res.message = False, f'ошибка осмотра: {e}'
            self.get_logger().error(res.message)
        finally:
            self.pub_active.publish(Bool(data=False))
            self.busy.release()
        self.say(res.message)
        return res

    def survey(self):
        msg = self.objects
        if msg is None or not msg.objects:
            return False, 'нет /objects: сначала распознавание из позы наблюдения'
        if self.k is None:
            return False, 'нет camera_info камеры запястья'
        objs = list(msg.objects)
        xy = np.array([[o.pose.position.x, o.pose.position.y] for o in objs])
        ctr = xy.mean(0)
        reach = max(float(np.max(np.linalg.norm(xy - ctr, axis=1))) + 0.08, 0.10)
        target = np.array([ctr[0], ctr[1], 0.03])
        fx, fy, cx, cy, W, H = self.k
        half_v = math.atan2(H / 2, fy)                    # половина вертикального угла обзора
        dist = min(max(reach / math.tan(0.8 * half_v), float(self.p('min_dist'))), float(self.p('max_dist')))
        el = math.radians(float(self.p('elevation_deg')))
        n = int(self.p('views'))
        base_dir = math.atan2(-ctr[1], -ctr[0])          # от центра предметов к основанию руки
        arc = math.radians(float(self.p('arc_deg')))
        azs = [base_dir + (arc * (i / (n - 1) - 0.5) if n > 1 else 0.0) for i in range(n)]
        acc = {o.id: [] for o in objs}
        if not self.goto_joints(self.p('observe_joints')):        # стартовый вид — из позы наблюдения
            return False, 'рука не встала в позу наблюдения перед осмотром'
        t_obs = time.monotonic() + float(self.p('settle'))
        while time.monotonic() < t_obs:
            time.sleep(0.05)
        n0 = self.grab_points(t_obs, objs, msg.table_z, acc)
        self.say(f'осмотр: точек сверху {n0}')
        votes = {o.id: {} for o in objs}
        seen = {o.id: 0 for o in objs}
        log = {o.id: [] for o in objs}
        for o in objs:                                     # вид сверху (поза наблюдения) — тоже голос
            if o.detector_label:
                votes[o.id][o.detector_label] = o.detector_score
                seen[o.id] += 1
                log[o.id].append(('сверху', o.detector_label, round(o.detector_score, 2)))
        done_views = 0
        for i, az in enumerate(azs):
            self.say(f'осмотр: ракурс {i + 1}/{n} (азимут {math.degrees(az - base_dir):+.0f}°, '
                     f'наклон {math.degrees(el):.0f}°, {dist:.2f} м)')
            ok = False
            for dd, da in ((0.0, 0.0), (0.05, 0.0), (-0.05, 0.0), (0.0, 0.15), (0.0, -0.15)):
                d = min(max(dist + dd, 0.28), 0.7)
                a = az + da
                cam = target + d * np.array([math.cos(el) * math.cos(a), math.cos(el) * math.sin(a), math.sin(el)])
                if self.camera_to(cam, target):
                    ok = True
                    break
            if not ok:
                self.say(f'ракурс {i + 1}: рука туда не достаёт — пропускаю')
                continue
            t0 = time.monotonic() + float(self.p('settle'))
            while time.monotonic() < t0:
                time.sleep(0.05)
            end = time.monotonic() + 4.0
            while self.dets_t < t0 and time.monotonic() < end:
                time.sleep(0.05)
            if self.dets_t < t0:
                self.say(f'ракурс {i + 1}: нет свежих рамок нейросети (/detections) — пропускаю')
                continue
            self.grab_points(t0, objs, msg.table_z, acc)
            boxes = list(self.dets)
            R, T = self.tf(self.p('optical_frame'), self.p('base_frame'))
            done_views += 1
            used = set()
            # ожидаемые рамки предметов в этом кадре
            pairs = []
            for o in objs:
                pc = object_corners(o) @ R.T + T
                if (pc[:, 2] <= 0.05).any():
                    continue
                u = fx * pc[:, 0] / pc[:, 2] + cx
                v = fy * pc[:, 1] / pc[:, 2] + cy
                eb = (max(u.min(), 0), max(v.min(), 0), min(u.max(), W), min(v.max(), H))
                if eb[2] - eb[0] < 6 or eb[3] - eb[1] < 6:
                    continue
                seen[o.id] += 1
                for j, b in enumerate(boxes):
                    s = iou(eb, b[:4])
                    if s >= float(self.p('min_iou')):
                        pairs.append((s, o.id, j))
            for s, oid, j in sorted(pairs, reverse=True):     # жадно: рамка — одному предмету
                if j in used or any(e[0] == f'ракурс {i + 1}' for e in log[oid]):
                    continue
                used.add(j)
                lab, sc = boxes[j][5], boxes[j][4]
                votes[oid][lab] = votes[oid].get(lab, 0.0) + sc
                log[oid].append((f'ракурс {i + 1}', lab, round(sc, 2)))
        self.say('осмотр: возвращаю руку в позу наблюдения')
        if not self.goto_joints(self.p('observe_joints')):
            self.get_logger().warning('рука не вернулась в позу наблюдения')
        out = DetectedObjectArray()
        out.header, out.table_z = msg.header, msg.table_z
        lines = []
        coef = np.array([0.0, 0.0, msg.table_z])
        for o in objs:
            r = DetectedObject()
            for f in ('id', 'label', 'confidence', 'shape', 'dimensions', 'pose', 'grasp_pose', 'num_points'):
                setattr(r, f, getattr(o, f))
            geo = ''
            if self.p('geometry') == 'merge' and acc[o.id]:
                pts = P.voxelize(np.concatenate(acc[o.id]), float(self.p('voxel')))
                pts = drop_sparse(pts, 0.012, 6)      # одиночные точки и «летящие» пиксели на краях
                fit = P.fit_shape(pts, coef) if len(pts) >= int(self.p('min_points')) else None
                if fit is not None:
                    old = np.array([o.pose.position.x, o.pose.position.y])
                    r.shape = fit['shape']
                    r.dimensions = [float(x) for x in fit['dims']]
                    cz, sz = math.cos(fit['yaw'] / 2), math.sin(fit['yaw'] / 2)
                    r.pose.position.x, r.pose.position.y, r.pose.position.z = map(float, fit['center'])
                    r.pose.orientation.x = r.pose.orientation.y = 0.0
                    r.pose.orientation.z, r.pose.orientation.w = sz, cz
                    r.grasp_pose.position.x, r.grasp_pose.position.y = map(float, fit['center'][:2])
                    r.grasp_pose.position.z = float(fit['top_z'])
                    (r.grasp_pose.orientation.x, r.grasp_pose.orientation.y, r.grasp_pose.orientation.z,
                     r.grasp_pose.orientation.w) = P.grasp_quat(fit['yaw'])
                    r.num_points = int(len(pts))
                    shift = float(np.linalg.norm(np.array(fit['center'][:2]) - old)) * 1000
                    dims = ' × '.join(f'{x * 100:.1f}' for x in fit['dims'])
                    geo = f', центр сдвинут на {shift:.0f} мм, размеры {dims} см ({len(pts)} точек)'
                else:
                    geo = ', геометрия прежняя (мало точек)'
            v = votes[o.id]
            if v:
                lab = max(v, key=v.get)
                agree = sum(1 for e in log[o.id] if e[1] == lab)
                r.detector_label, r.detector_score = lab, float(v[lab] / max(seen[o.id], 1))
                lines.append(f'{o.id}: {lab} ({agree} из {seen[o.id]} видов, {r.detector_score:.2f}){geo}')
            else:
                lines.append(f'{o.id}: класс не узнан ни в одном виде{geo}')
            out.objects.append(r)
        self.pub.publish(out)
        self.pub_votes.publish(String(data=json.dumps(log, ensure_ascii=False)))
        return True, f'осмотр: {done_views} из {n} ракурсов; ' + '; '.join(lines)


def main():
    rclpy.init()
    node = Survey()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(node)
    try:
        ex.spin()
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
