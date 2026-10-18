#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
grasp_analyzer.py

Анализирует сцену перед вакуумным гриппером xArm6 по данным камеры на ЭФ
(Realsense D435i из симуляции Gazebo Harmonic, topics gz/camera/*) и строит
ВАРИАНТЫ ЗАХВАТА цилиндрического блока, стоящего на столе:

  * подписка на цветное изображение и topic облака точек /camera/depth/color/points;
  * поиск "столешницы" (горизонтальная плоскость) в облаке RANSAC-подобным методом;
  * сегментация объектов над столом по высоте, оценка центра/радиуса/высоты цилиндра;
  * генерация вариантов захвата (TOP / SIDE / RIM) через xarm_grasping.grasp_generator;
  * публикация маркеров в RViz (/xarm_grasping/grasp_markers): блок, нормаль,
    стрелки pre-grasp -> grasp для каждого варианта, лучший вариант выделен;
  * дублирование изображения камеры на /camera/image_display (для rqt / RViz);
  * JSON-список вариантов — на /xarm_grasping/grasp_candidates (std_msgs/String).

Запуск:  ros2 run xarm_grasping grasp_analyzer.py --ros-args -p use_sim_time:=true
"""
import json
import math

import numpy as np

import rclpy
import rclpy.duration
import rclpy.time
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, DurabilityPolicy

from sensor_msgs.msg import Image, PointCloud2
from geometry_msgs.msg import PoseStamped
from std_msgs.msg import String
from visualization_msgs.msg import Marker, MarkerArray

try:
    from cv_bridge import CvBridge
    HAS_CV_BRIDGE = True
except Exception:
    HAS_CV_BRIDGE = False

from xarm_grasping.grasp_generator import generate_cylinder_grasps


def points_from_msg(msg: PointCloud2) -> np.ndarray:
    """Быстро вытащить (N,3) float32 из PointCloud2 без dependency на pc2_to_numpy."""
    step = msg.point_step
    offsets = {f.name: f.offset for f in msg.fields}
    if 'x' not in offsets:
        return np.zeros((0, 3), dtype=np.float32)
    arr = np.frombuffer(msg.data, dtype=np.uint8)
    arr = arr.reshape(-1, msg.point_step)[:msg.width * msg.height]
    def col(name):
        o = offsets[name]
        return arr[:, o:o + 4].copy().view(np.float32).ravel()
    pts = np.column_stack((col('x'), col('y'), col('z')))
    mask = np.isfinite(pts).all(axis=1)
    return pts[mask]


class GraspAnalyzer(Node):
    def __init__(self):
        super().__init__('grasp_analyzer')

        # ---------------- параметры ----------------
        # use_sim_time объявляет сам rclpy — повторный declare_parameter падает
        # с ParameterAlreadyDeclaredException; задаётся из launch (-p use_sim_time:=true).
        self.declare_parameter('color_topic', '/camera/color/image_raw')
        self.declare_parameter('points_topic', '/camera/depth/color/points')
        self.declare_parameter('camera_frame', 'camera_color_optical_frame')
        self.declare_parameter('base_frame', 'link_base')
        self.declare_parameter('table_height', 1.0)      # z столешницы в base_frame
        self.declare_parameter('block_radius', 0.04)     # ожидаемый радиус блока (априори)
        self.declare_parameter('block_height', 0.09)
        self.declare_parameter('suction_radius', 0.02)
        self.declare_parameter('approach_offset', 0.10)
        self.declare_parameter('analyze_period', 2.0)    # сек
        self.declare_parameter('voxel_size', 0.01)
        self.declare_parameter('min_points', 150)

        self.color_topic = self.get_parameter('color_topic').value
        self.points_topic = self.get_parameter('points_topic').value
        self.camera_frame = self.get_parameter('camera_frame').value
        self.base_frame = self.get_parameter('base_frame').value
        self.table_z = float(self.get_parameter('table_height').value)
        self.block_R = float(self.get_parameter('block_radius').value)
        self.block_H = float(self.get_parameter('block_height').value)
        self.suction_r = float(self.get_parameter('suction_radius').value)
        self.approach_off = float(self.get_parameter('approach_offset').value)
        self.voxel = float(self.get_parameter('voxel_size').value)
        self.min_points = int(self.get_parameter('min_points').value)

        # априорная поза блока (совпадает с worlds/models/cylinder_block/model.config)
        self.declare_parameter('block_x', 0.35)
        self.declare_parameter('block_y', -0.75)
        self.default_block_xy = (float(self.get_parameter('block_x').value),
                                 float(self.get_parameter('block_y').value))

        qos = QoSProfile(depth=2, reliability=ReliabilityPolicy.BEST_EFFORT)

        # ---------------- подписки ----------------
        self.last_image = None
        self.bridge = CvBridge() if HAS_CV_BRIDGE else None

        # image (для показа) + points (для анализа)
        self.image_only_sub = self.create_subscription(
            Image, self.color_topic, self._image_cb, qos)
        self.points_only_sub = self.create_subscription(
            PointCloud2, self.points_topic, self._points_cb, qos)

        # ---------------- публикуемое ----------------
        self.marker_pub = self.create_publisher(
            MarkerArray, '/xarm_grasping/grasp_markers',
            QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL))
        self.candidates_pub = self.create_publisher(String, '/xarm_grasping/grasp_candidates', 1)
        self.display_pub = self.create_publisher(Image, '/camera/image_display', qos)
        self.pose_pub = self.create_publisher(PoseStamped, '/xarm_grasping/best_grasp_pose', 1)

        period = float(self.get_parameter('analyze_period').value)
        self.timer = self.create_timer(period, self.analyze_scene)

        self.latest_points = None   # (N,3) в camera_frame
        self.get_logger().info(
            f'GraspAnalyzer запущен. points={self.points_topic}, image={self.color_topic}')

    # ---------- callbacks ----------
    def _image_cb(self, msg: Image):
        self.last_image = msg
        if self.bridge is not None:
            try:
                cv_img = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
                out = self.bridge.cv2_to_imgmsg(cv_img, encoding='bgr8')
                out.header = msg.header
                self.display_pub.publish(out)
            except Exception as e:
                self.get_logger().debug(f'cv_bridge: {e}')

    def _points_cb(self, msg: PointCloud2):
        pts = points_from_msg(msg)
        if pts.shape[0] > 0:
            self.latest_points = (pts, msg.header.frame_id)

    # ---------- анализ ----------
    def analyze_scene(self):
        if self.latest_points is None:
            self.get_logger().warn('Облако точек ещё не получено (ждём камеру из Gazebo)...')
            return
        pts, frame = self.latest_points
        if frame != self.camera_frame:
            # gz-сенсор может публиковать в *_optical_frame или camera_link — примем как есть
            pass

        # воксельная сетка для скорости
        keys = np.floor(pts / self.voxel).astype(np.int64)
        _, idx = np.unique(keys, axis=0, return_index=True)
        v = pts[idx]

        # камера смотрит вниз (оптическая система: z вперёд). Точки с z>0 — перед камерой.
        front = v[v[:, 2] > 0.05]
        if front.shape[0] < self.min_points:
            self.get_logger().warn(f'Мало точек ({front.shape[0]}), анализ пропущен')
            return

        # --- поиск столешницы: самая многочисленная горизонтальная плоскость.
        # В optical-frame стола почти перпендикулярна оптической оси => много точек
        # с похожими z. Оценим гистограмму z.
        zs = front[:, 2]
        hist, edges = np.histogram(zs, bins=60)
        b = int(np.argmax(hist))
        z_plane = 0.5 * (edges[b] + edges[b + 1])
        table_mask = np.abs(front[:, 2] - z_plane) < 0.02
        table_pts = front[table_mask]
        obj_mask = (~table_mask) & (front[:, 2] < z_plane)  # ближе к камере = выше стола
        obj_pts = front[obj_mask]

        result = {'frame': self.camera_frame, 'n_table': int(table_pts.shape[0]),
                  'n_object': int(obj_pts.shape[0])}

        cx = cy = cz = None
        if obj_pts.shape[0] >= self.min_points // 2:
            # центр и "экваториальный" радиус объекта
            center = obj_pts.mean(axis=0)
            cx_cam, cy_cam, cz_cam = center
            # радиус по max расстоянию в плоскости, перпендикулярной подходу
            dxy = np.linalg.norm(obj_pts[:, :2] - center[:2], axis=1)
            r_est = float(np.percentile(dxy, 95))
            result.update(center_cam=[float(cx_cam), float(cy_cam), float(cz_cam)],
                          radius_est=r_est)
            cx, cy, cz = cx_cam, cy_cam, cz_cam

        # --- центр блока в base_frame: по возможности из облака точек (tf),
        #     иначе — априорная поза модели cylinder_block из мира ---
        block_xy = self.default_block_xy
        try:
            from tf2_ros import Buffer, TransformListener
            if not hasattr(self, 'tf_buffer'):
                self.tf_buffer = Buffer()
                self.tf_listener = TransformListener(self.tf_buffer, self)
            if cx is not None:
                tfs = self.tf_buffer.lookup_transform(
                    self.base_frame, frame, rclpy.time.Time())
                q = tfs.transform.rotation
                p = np.array([cx, cy, cz])
                rot = self._quat_rot(q.x, q.y, q.z, q.w)
                pb = rot @ p + np.array([tfs.transform.translation.x,
                                         tfs.transform.translation.y,
                                         tfs.transform.translation.z])
                # центр кластера = центр объёма; низ цилиндра на столе =>
                # xy берём напрямую, z-смещение игнорируем (стоит на столе)
                block_xy = (float(pb[0]), float(pb[1]))
                result['center_base'] = [float(v) for v in pb]
        except Exception as e:
            self.get_logger().debug(f'tf недоступен ({e}), использую априорную позу блока')

        # --- генерация вариантов захвата ---
        grasps = generate_cylinder_grasps(
            center_xy=block_xy, radius=self.block_R, height=self.block_H,
            table_z=self.table_z, suction_radius=self.suction_r,
            approach_offset=self.approach_off)
        result['grasps'] = grasps
        result['best'] = grasps[0]['type'] if grasps else None

        self.candidates_pub.publish(String(data=json.dumps(result, ensure_ascii=False)))
        self.publish_markers(grasps, block_xy)
        if grasps:
            ps = PoseStamped()
            ps.header.frame_id = self.base_frame
            ps.header.stamp = self.get_clock().now().to_msg()
            x, y, z, r, p_, w = grasps[0]['pose']
            ps.pose.position.x, ps.pose.position.y, ps.pose.position.z = map(float, (x, y, z))
            q = self._rpy_to_quat(r, p_, w)
            ps.pose.orientation = q
            self.pose_pub.publish(ps)

        self.get_logger().info(
            f'Вариантов захвата: {len(grasps)}; лучший: '
            f'{grasps[0]["type"]} (score={grasps[0]["score"]}) у блока '
            f'({block_xy[0]:.3f}, {block_xy[1]:.3f})')

    # ---------- helpers ----------
    @staticmethod
    def _quat_rot(x, y, z, w):
        # rotation matrix from quaternion
        n = math.sqrt(x * x + y * y + z * z + w * w) or 1.0
        x, y, z, w = x / n, y / n, z / n, w / n
        return np.array([
            [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
            [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
            [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])

    @staticmethod
    def _rpy_to_quat(roll, pitch, yaw):
        from geometry_msgs.msg import Quaternion
        cr, sr = math.cos(roll / 2), math.sin(roll / 2)
        cp, sp = math.cos(pitch / 2), math.sin(pitch / 2)
        cy, sy = math.cos(yaw / 2), math.sin(yaw / 2)
        q = Quaternion()
        q.x = sr * cp * cy - cr * sp * sy
        q.y = cr * sp * cy + sr * cp * sy
        q.z = cr * cp * sy - sr * sp * cy
        q.w = cr * cp * cy + sr * sp * sy
        return q

    def _marker(self, ns, id_, type_, pos, orient=None, scale=(0.01, 0.01, 0.01),
                color=(0, 1, 0, 1), lifetime_sec=5.0):
        m = Marker()
        m.header.frame_id = self.base_frame
        m.ns = ns
        m.id = id_
        m.type = type_
        m.action = Marker.ADD
        # rosidl принимает только float: int (scale=(0, 0, 0.03)) и numpy-типы
        # дают assert в C-конвертере и abort всего процесса
        m.pose.position.x, m.pose.position.y, m.pose.position.z = map(float, pos)
        if orient is None:
            m.pose.orientation.w = 1.0
        else:
            m.pose.orientation = orient
        m.scale.x, m.scale.y, m.scale.z = map(float, scale)
        m.color.r, m.color.g, m.color.b, m.color.a = map(float, color)
        # поле msg — builtin_interfaces/Duration; rclpy.duration.Duration сюда
        # нельзя (assert в rosidl -> abort процесса), нужен .to_msg()
        m.lifetime = rclpy.duration.Duration(seconds=lifetime_sec).to_msg()
        return m

    def publish_markers(self, grasps, block_xy):
        arr = MarkerArray()
        cx, cy = block_xy
        top_z = self.table_z + self.block_H

        # сам блок (цилиндр) — wireframe
        cyl = self._marker('object', 0, Marker.CYLINDER, (cx, cy, self.table_z + self.block_H / 2),
                           scale=(2 * self.block_R, 2 * self.block_R, self.block_H),
                           color=(1.0, 0.5, 0.1, 0.35))
        arr.markers.append(cyl)

        colors = {'TOP': (0.1, 0.9, 0.1, 1.0), 'SIDE': (0.2, 0.5, 1.0, 1.0),
                  'RIM': (1.0, 0.8, 0.1, 1.0)}
        for i, g in enumerate(grasps):
            col = colors.get(g['type'], (1, 1, 1, 1))
            if i == 0:
                col = (0.0, 1.0, 0.0, 1.0)  # лучший — ярко-зелёный
            x, y, z, r, p_, w = g['pose']
            px, py, pz = g['pre_grasp'][:3]
            q = self._rpy_to_quat(r, p_, w)
            # сфера = точка касания присоски
            arr.markers.append(self._marker(
                'grasps', 10 + i, Marker.SPHERE, (x, y, z), orient=q,
                scale=(0.02, 0.02, 0.02), color=col))
            # стрелка pre_grasp -> grasp
            arrow = self._marker('arrows', 50 + i, Marker.ARROW, (px, py, pz),
                                 scale=(0.005, 0.012, 0.012), color=col)
            arrow.points = [self._vec(px, py, pz), self._vec(x, y, z)]
            arr.markers.append(arrow)
            # подпись
            txt = self._marker('labels', 100 + i, Marker.TEXT_VIEW_FACING,
                               (x, y, z + 0.05), scale=(0, 0, 0.03), color=col)
            txt.text = f'#{i} {g["type"]} s={g["score"]}'
            arr.markers.append(txt)

        self.marker_pub.publish(arr)

    @staticmethod
    def _vec(x, y, z):
        from geometry_msgs.msg import Point
        p = Point()
        p.x, p.y, p.z = float(x), float(y), float(z)
        return p


def main():
    rclpy.init()
    node = GraspAnalyzer()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
