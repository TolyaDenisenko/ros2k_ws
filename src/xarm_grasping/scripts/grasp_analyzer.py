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
        # Защита от мусора (камера смотрит мимо стола): точки дальше max_range от
        # камеры отбрасываются; объект — кластер на высоте table_height..+
        # object_max_height в base_frame, шире max_object_radius — отказ; облако старше max_cloud_age (с) не используется.
        # use_prior_pose=true — по-старому брать block_x/y, если объект не найден.
        self.declare_parameter('max_range', 1.2)
        self.declare_parameter('max_object_radius', 0.15)
        self.declare_parameter('table_tolerance', 0.01)    # мин. порог «над столом», м
        self.declare_parameter('table_search', 0.05)       # окно поиска стола ± table_height
        self.declare_parameter('object_max_height', 0.30)  # выше стола — не объект
        self.declare_parameter('min_object_points', 20)    # вокселей в кластере
        self.declare_parameter('max_cloud_age', 3.0)
        self.declare_parameter('use_prior_pose', False)

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
        self.max_range = float(self.get_parameter('max_range').value)
        self.max_obj_r = float(self.get_parameter('max_object_radius').value)
        self.table_tol = float(self.get_parameter('table_tolerance').value)
        self.table_search = float(self.get_parameter('table_search').value)
        self.obj_max_h = float(self.get_parameter('object_max_height').value)
        self.min_obj_points = int(self.get_parameter('min_object_points').value)
        self.max_cloud_age = float(self.get_parameter('max_cloud_age').value)
        self.use_prior = bool(self.get_parameter('use_prior_pose').value)

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
        # Облако — RELIABLE: 1280x800 xyzrgb ≈ 32 МБ на сообщение, при BEST_EFFORT
        # потеря одного UDP-фрагмента выбрасывает сообщение целиком, и до ноды
        # не доходило ни одного облака (depth_image_proc публикует RELIABLE).
        self.points_only_sub = self.create_subscription(
            PointCloud2, self.points_topic, self._points_cb,
            QoSProfile(depth=1, reliability=ReliabilityPolicy.RELIABLE))

        # ---------------- публикуемое ----------------
        self.marker_pub = self.create_publisher(
            MarkerArray, '/xarm_grasping/grasp_markers',
            QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL))
        self.candidates_pub = self.create_publisher(String, '/xarm_grasping/grasp_candidates', 1)
        self.display_pub = self.create_publisher(Image, '/camera/image_display', qos)
        self.pose_pub = self.create_publisher(PoseStamped, '/xarm_grasping/best_grasp_pose', 1)

        period = float(self.get_parameter('analyze_period').value)
        self.timer = self.create_timer(period, self.analyze_scene)

        self.latest_points = None   # ((N,3) в camera_frame, frame_id)
        self.latest_points_time = None  # когда пришло (часы ноды)
        self.markers_shown = False
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
        # Пустое облако тоже запоминаем: камера смотрит в пустоту — старые
        # точки уже не про текущую сцену.
        self.latest_points = (points_from_msg(msg), msg.header.frame_id)
        self.latest_points_time = self.get_clock().now()

    def _reject(self, reason):
        """Объект не найден/недостоверен: убрать маркеры, не публиковать захват."""
        self.get_logger().warning(f'Захват не построен: {reason}',
                               throttle_duration_sec=10.0)
        self.candidates_pub.publish(String(data=json.dumps(
            {'frame': self.camera_frame, 'grasps': [], 'best': None,
             'rejected': reason}, ensure_ascii=False)))
        if self.markers_shown:
            m = Marker()
            m.header.frame_id = self.base_frame
            m.action = Marker.DELETEALL
            self.marker_pub.publish(MarkerArray(markers=[m]))
            self.markers_shown = False

    # ---------- анализ ----------
    def analyze_scene(self):
        if self.latest_points is None:
            self.get_logger().warning('Облако точек ещё не получено (ждём камеру из Gazebo)...')
            return
        age = (self.get_clock().now() - self.latest_points_time).nanoseconds * 1e-9
        if age > self.max_cloud_age:
            self._reject(f'облако устарело ({age:.1f} с)')
            return
        pts, frame = self.latest_points
        # дальше max_range — пол/стены, а не объект на столе
        pts = pts[np.linalg.norm(pts, axis=1) < self.max_range]

        # воксельная сетка для скорости
        keys = np.floor(pts / self.voxel).astype(np.int64)
        _, idx = np.unique(keys, axis=0, return_index=True)
        v = pts[idx]
        if v.shape[0] < self.min_points:
            self._reject(f'мало точек ближе {self.max_range} м ({v.shape[0]}) — '
                         'камера не смотрит на стол')
            return

        # --- облако -> base_frame. Сегментация по высоте над столом в base_frame
        #     не зависит от наклона камеры (раньше стол искался как плоскость
        #     поперёк оптической оси, и при наклоне его ближний край попадал в объект).
        try:
            from tf2_ros import Buffer, TransformListener
            if not hasattr(self, 'tf_buffer'):
                self.tf_buffer = Buffer()
                self.tf_listener = TransformListener(self.tf_buffer, self)
            tfs = self.tf_buffer.lookup_transform(self.base_frame, frame, rclpy.time.Time())
        except Exception as e:
            self._reject(f'нет TF {self.base_frame} <- {frame} ({e})')
            return
        q = tfs.transform.rotation
        t = tfs.transform.translation
        vb = v @ self._quat_rot(q.x, q.y, q.z, q.w).T + np.array([t.x, t.y, t.z])

        # --- стол: плоскость z = a·x + b·y + c, уточняется по точкам в окне
        #     table_height ± table_search (несколько итераций с отбросом выбросов).
        #     Порог «над столом» = max(table_tolerance, 3σ шума глубины): у
        #     симулированной глубины σ≈5 мм на 0.5 м, и при фиксированном ±1 см
        #     хвосты шума по всей столешнице сливались в один «объект» r≈0.4 м.
        win = vb[np.abs(vb[:, 2] - self.table_z) < self.table_search]
        if win.shape[0] < self.min_points:
            self._reject(f'стол не найден около table_height={self.table_z:.3f} '
                         f'({win.shape[0]} точек)')
            return
        A = np.c_[win[:, 0], win[:, 1], np.ones(win.shape[0])]
        zt = win[:, 2]
        for _ in range(4):
            coef = np.linalg.lstsq(A, zt, rcond=None)[0]
            res = zt - A @ coef
            keep = np.abs(res) < max(0.004, 2.0 * float(np.std(res)))
            A, zt = A[keep], zt[keep]
        sigma = float(np.std(zt - A @ coef))
        tol = max(self.table_tol, 3.0 * sigma)
        dz = vb[:, 2] - (coef[0] * vb[:, 0] + coef[1] * vb[:, 1] + coef[2])
        table_pts = vb[np.abs(dz) < tol]
        obj_mask = (dz > tol) & (dz < self.obj_max_h)
        result = {'frame': self.base_frame, 'n_table': int(table_pts.shape[0]),
                  'n_object': int(obj_mask.sum()), 'table_z': float(coef[2]),
                  'table_sigma': sigma}

        cluster = None
        if obj_mask.sum() >= self.min_obj_points:
            # --- кластеры объектов на сетке XY (шаг voxel). Ячейка — «предмет»,
            #     если МЕДИАНА высоты её точек выше порога: хвосты шума столешницы
            #     дают отдельные точки над порогом, но медиана ячейки остаётся
            #     у стола. Иначе ореол шума склеивал коробку в «объект» r≈0.3 м.
            from scipy import ndimage
            near = (dz > -tol) & (dz < self.obj_max_h)
            w, wd = vb[near], dz[near]
            ij = np.floor(w[:, :2] / self.voxel).astype(np.int64)
            ij -= ij.min(axis=0)
            shape = tuple(ij.max(axis=0) + 1)
            flat = np.ravel_multi_index(ij.T, shape)
            order = np.argsort(flat)
            cells, start, cnt = np.unique(flat[order], return_index=True, return_counts=True)
            ds = wd[order]
            med = np.array([np.median(ds[i:i + n]) for i, n in zip(start, cnt)])
            grid = np.zeros(int(np.prod(shape)), dtype=bool)
            grid[cells[med > tol]] = True
            grid = ndimage.binary_opening(grid.reshape(shape))  # одиночные ячейки — шум
            labels, n_lab = ndimage.label(ndimage.binary_dilation(grid))
            lab = labels[ij[:, 0], ij[:, 1]]
            # цель — кластер, ближайший к точке, куда смотрит центр кадра:
            # пересечение оптической оси (z камеры) с плоскостью стола
            rot = self._quat_rot(q.x, q.y, q.z, q.w)
            cam = np.array([t.x, t.y, t.z])
            axis = rot[:, 2]
            n_up = np.array([-coef[0], -coef[1], 1.0])
            denom = float(axis @ n_up)
            if denom < -1e-3:   # ось смотрит вниз, на стол
                s_hit = (coef[2] - float(cam @ n_up)) / denom
                aim_xy = (cam + s_hit * axis)[:2]
            else:
                aim_xy = cam[:2]
            result['aim_xy'] = [float(aim_xy[0]), float(aim_xy[1])]
            best_d = None
            for k in range(1, n_lab + 1):
                c = w[(lab == k) & (wd > tol)]
                if c.shape[0] < self.min_obj_points:
                    continue
                d = float(np.linalg.norm(np.median(c[:, :2], axis=0) - aim_xy))
                if best_d is None or d < best_d:
                    best_d, cluster = d, c
            result['n_clusters'] = int(n_lab)

        if cluster is None:
            if not self.use_prior:
                self._reject('объект над столом не найден (стол на table_height='
                             f'{self.table_z:.3f}: {table_pts.shape[0]} точек)')
                return
            self.get_logger().warning('объект не найден — априорная поза block_x/y',
                                      throttle_duration_sec=10.0)
            block_xy = self.default_block_xy
        else:
            center_xy = np.median(cluster[:, :2], axis=0)
            r_est = float(np.percentile(
                np.linalg.norm(cluster[:, :2] - center_xy, axis=1), 95))
            result.update(center_base=[float(center_xy[0]), float(center_xy[1]),
                                       float(cluster[:, 2].max())],
                          radius_est=r_est, n_cluster=int(cluster.shape[0]))
            if r_est > self.max_obj_r:
                self._reject(f'«объект» шириной r={r_est:.2f} м '
                             f'(> {self.max_obj_r}) — это не предмет на столе')
                return
            block_xy = (float(center_xy[0]), float(center_xy[1]))

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
        self.markers_shown = True

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
