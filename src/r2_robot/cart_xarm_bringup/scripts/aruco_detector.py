#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
aruco_detector — находит метку ArUco на кадре камеры и публикует её позу (для парковки тележки).

Вход:  кадр (image_topic, rgb8/bgr8/mono8) и camera_info (camera_info_topic).
Выход:
  /aruco/marker_pose   geometry_msgs/PoseStamped — поза метки в оптическом кадре камеры
                       (z — от камеры к метке, ось z метки — наружу из её лицевой стороны);
  /detected_dock_pose  то же — вход Nav2 docking (SimpleChargingDock, external detection);
  /aruco/image         кадр с контуром и осями метки (только если на него кто-то подписан);
  /aruco/status        текст: «метка 0: 1.23 м, угол 4.5°» или «метки не видно».

Как считается поза
  1. углы метки на кадре (cv2.aruco) -> направление на метку и её поворот в плоскости (solvePnP,
     SOLVEPNP_IPPE_SQUARE);
  2. если есть глубина (use_depth, кадр aligned_depth того же момента): по точкам глубины ВНУТРИ
     метки подгоняется плоскость; расстояние = пересечение луча на центр метки с этой плоскостью,
     нормаль (курс и наклон) = нормаль плоскости. По одним углам дальность занижается на 5–15 %
     (размытие краёв на маленькой метке), а курс гуляет на ±5–9° (неоднозначность позы квадрата),
     с глубиной обе ошибки уходят (см. tools/check_aruco_accuracy.py);
  3. нет глубины — остаётся поза по углам (status: «по углам»).
Камера должна видеть всю метку вместе с белой рамкой.

Параметры
  image_topic /front_camera/color/image_raw    camera_info_topic /front_camera/color/camera_info
  dictionary DICT_4X4_50   marker_id 0 (-1 — любая)   marker_size 0.16 (м, сторона чёрного квадрата)
  use_depth true   depth_topic /front_camera/aligned_depth/image_raw (32FC1 в м или 16UC1 в мм)
  min_depth_points 30 — меньше точек глубины в метке — не верим плоскости, берём позу по углам
  max_rate 5.0 — сколько кадров в секунду обрабатывать (камера даёт 15; при 15 Гц нода ест ~24 %
               ядра, для парковки хватает 5)
Время симуляции не нужно: метки времени берутся из кадра (поэтому use_sim_time не включаем —
в симуляции с ним rclpy-нода зря тратит процессор на /clock).
"""
import math

import cv2
import numpy as np
import rclpy
from rclpy.executors import ExternalShutdownException
from geometry_msgs.msg import PoseStamped
from rclpy.node import Node
from sensor_msgs.msg import CameraInfo, Image
from std_msgs.msg import String


def rot_to_quat(R):
    """Матрица поворота 3×3 -> кватернион (x, y, z, w)."""
    t = np.trace(R)
    if t > 0:
        s = math.sqrt(t + 1.0) * 2
        return ((R[2, 1] - R[1, 2]) / s, (R[0, 2] - R[2, 0]) / s, (R[1, 0] - R[0, 1]) / s, 0.25 * s)
    i = int(np.argmax(np.diag(R)))
    j, k = (i + 1) % 3, (i + 2) % 3
    s = math.sqrt(1.0 + R[i, i] - R[j, j] - R[k, k]) * 2
    q = [0.0, 0.0, 0.0, (R[k, j] - R[j, k]) / s]
    q[i] = 0.25 * s
    q[j] = (R[j, i] + R[i, j]) / s
    q[k] = (R[k, i] + R[i, k]) / s
    return tuple(q)


class ArucoDetector(Node):
    def __init__(self):
        super().__init__('aruco_detector')
        dp = self.declare_parameter
        dp('image_topic', '/front_camera/color/image_raw')
        dp('camera_info_topic', '/front_camera/color/camera_info')
        dp('dictionary', 'DICT_4X4_50')
        dp('marker_id', 0)
        dp('marker_size', 0.16)
        dp('use_depth', True)
        dp('depth_topic', '/front_camera/aligned_depth/image_raw')
        dp('min_depth_points', 30)
        dp('max_rate', 5.0)
        self.last_t = 0.0
        d = cv2.aruco.getPredefinedDictionary(getattr(cv2.aruco, self.get_parameter('dictionary').value))
        new_api = hasattr(cv2.aruco, 'ArucoDetector')      # OpenCV ≥ 4.7; на Ubuntu 24.04 — 4.6
        prm = cv2.aruco.DetectorParameters() if new_api else cv2.aruco.DetectorParameters_create()
        # Метка издалека мелкая (на 1.7 м ~34 px, клетка ~6 px), белое отрисовано серым: при чтении
        # клеток с 4 px на клетку (по умолчанию) декодирование срывается; 8 px — читается.
        prm.perspectiveRemovePixelPerCell = 8
        prm.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX   # точнее углы -> точнее поза
        if new_api:
            det = cv2.aruco.ArucoDetector(d, prm)
            self.detect = lambda g: det.detectMarkers(g)
        else:
            self.detect = lambda g: cv2.aruco.detectMarkers(g, d, parameters=prm)
        s = float(self.get_parameter('marker_size').value) / 2
        # углы метки в её системе (как выдаёт detectMarkers: лев-верх, прав-верх, прав-низ, лев-низ),
        # ось z — из лицевой стороны к камере
        self.obj = np.array([[-s, s, 0], [s, s, 0], [s, -s, 0], [-s, -s, 0]], np.float32)
        self.K = self.D = None
        self.pub_pose = self.create_publisher(PoseStamped, '/aruco/marker_pose', 10)
        self.pub_dock = self.create_publisher(PoseStamped, '/detected_dock_pose', 10)
        self.pub_img = self.create_publisher(Image, '/aruco/image', 1)
        self.pub_status = self.create_publisher(String, '/aruco/status', 10)
        self.create_subscription(CameraInfo, self.get_parameter('camera_info_topic').value,
                                 self.on_info, 1)
        self.create_subscription(Image, self.get_parameter('image_topic').value, self.on_image, 1)
        self.depth = None
        if self.get_parameter('use_depth').value:
            self.create_subscription(Image, self.get_parameter('depth_topic').value,
                                     lambda m: setattr(self, 'depth', m), 1)
        self.seen = None
        self.get_logger().info(
            f'ищу {self.get_parameter("dictionary").value} id {self.get_parameter("marker_id").value}, '
            f'метка {2 * s:.3f} м, кадр {self.get_parameter("image_topic").value}')

    def on_info(self, m):
        self.K = np.array(m.k, np.float64).reshape(3, 3)
        self.D = np.array(m.d, np.float64) if len(m.d) else np.zeros(5)

    def on_image(self, m):
        if self.K is None:
            return
        t_img = m.header.stamp.sec + m.header.stamp.nanosec * 1e-9
        rate = float(self.get_parameter('max_rate').value)
        if rate > 0 and 0 <= t_img - self.last_t < 1.0 / rate:
            return
        self.last_t = t_img
        ch = {'rgb8': 3, 'bgr8': 3, 'mono8': 1}.get(m.encoding)
        if ch is None:
            return
        a = np.frombuffer(m.data, np.uint8).reshape(m.height, m.step // ch, ch)[:, :m.width]
        gray = a[:, :, 0] if ch == 1 else cv2.cvtColor(
            a, cv2.COLOR_RGB2GRAY if m.encoding == 'rgb8' else cv2.COLOR_BGR2GRAY)
        corners, ids, _ = self.detect(gray)
        want = int(self.get_parameter('marker_id').value)
        found = None
        if ids is not None:
            for c, i in zip(corners, ids.ravel()):
                if want < 0 or int(i) == want:
                    found = (c.reshape(4, 2).astype(np.float32), int(i))
                    break
        if found is None:
            self.pub_status.publish(String(data='метки не видно'))
            if self.seen is not False:
                self.get_logger().info('метки не видно')
            self.seen = False
            self.debug(m, a, ch, None, None, None)
            return
        c, mid = found
        ok, rvec, tvec = cv2.solvePnP(self.obj, c, self.K, self.D, flags=cv2.SOLVEPNP_IPPE_SQUARE)
        if not ok:
            return
        R, _ = cv2.Rodrigues(rvec)
        t = tvec.ravel().astype(np.float64)
        src = 'по углам'
        refined = self.refine_with_depth(m, c, R)
        if refined is not None:
            t, R = refined
            src = 'по глубине'
        q = rot_to_quat(R)
        p = PoseStamped()
        p.header = m.header
        p.pose.position.x, p.pose.position.y, p.pose.position.z = map(float, t)
        p.pose.orientation.x, p.pose.orientation.y, p.pose.orientation.z, p.pose.orientation.w = q
        self.pub_pose.publish(p)
        self.pub_dock.publish(p)
        dist = float(np.linalg.norm(t))
        # угол между осью камеры и нормалью метки (0° — метка смотрит прямо в камеру)
        ang = math.degrees(math.acos(max(-1.0, min(1.0, -R[2, 2]))))
        self.pub_status.publish(String(data=f'метка {mid}: {dist:.2f} м, угол {ang:.1f}° ({src})'))
        if self.seen is not True:
            self.get_logger().info(f'метка {mid} найдена: {dist:.2f} м')
        self.seen = True
        self.debug(m, a, ch, c, rvec, tvec)

    def refine_with_depth(self, m, c, R_pnp):
        """Плоскость по глубине внутри метки -> (центр, поворот) или None."""
        d = self.depth
        if d is None or abs((d.header.stamp.sec + d.header.stamp.nanosec * 1e-9)
                            - (m.header.stamp.sec + m.header.stamp.nanosec * 1e-9)) > 0.15:
            return None
        if d.encoding == '32FC1':
            z = np.frombuffer(d.data, np.float32).reshape(d.height, d.step // 4)[:, :d.width]
        elif d.encoding == '16UC1':
            z = np.frombuffer(d.data, np.uint16).reshape(d.height, d.step // 2)[:, :d.width] * 1e-3
        else:
            return None
        if z.shape != (m.height, m.width):
            return None
        # внутри метки, с отступом 15 % от краёв (на краях глубина смешивается с фоном)
        ctr = c.mean(0)
        inner = (ctr + 0.7 * (c - ctr)).astype(np.int32)
        mask = np.zeros(z.shape, np.uint8)
        cv2.fillConvexPoly(mask, inner, 1)
        v, u = np.nonzero(mask)
        zz = z[v, u].astype(np.float64)
        ok = np.isfinite(zz) & (zz > 0.05)
        if ok.sum() < int(self.get_parameter('min_depth_points').value):
            return None
        u, v, zz = u[ok], v[ok], zz[ok]
        fx, fy, cx, cy = self.K[0, 0], self.K[1, 1], self.K[0, 2], self.K[1, 2]
        P = np.stack([(u - cx) * zz / fx, (v - cy) * zz / fy, zz], 1)
        c0 = P.mean(0)
        _, _, vt = np.linalg.svd(P - c0)
        n = vt[2]
        if n @ c0 > 0:          # нормаль — к камере (из лица метки наружу)
            n = -n
        ray = np.array([(ctr[0] - cx) / fx, (ctr[1] - cy) / fy, 1.0])
        t = ray * (n @ c0) / (n @ ray)          # луч на центр метки ∩ плоскость
        # оси: z = нормаль плоскости, x — от PnP, спроецированная на плоскость
        x = R_pnp[:, 0] - (R_pnp[:, 0] @ n) * n
        x /= np.linalg.norm(x)
        R = np.stack([x, np.cross(n, x), n], 1)
        return t, R

    def debug(self, m, a, ch, c, rvec, tvec):
        if self.pub_img.get_subscription_count() == 0:
            return
        img = cv2.cvtColor(a, cv2.COLOR_GRAY2BGR) if ch == 1 else a.copy()
        if c is not None:
            cv2.polylines(img, [c.astype(np.int32)], True, (255, 0, 0) if m.encoding == 'rgb8' else (0, 0, 255), 3)
            cv2.drawFrameAxes(img, self.K, self.D, rvec, tvec, float(self.get_parameter('marker_size').value) * 0.6, 3)
        out = Image()
        out.header, out.height, out.width = m.header, img.shape[0], img.shape[1]
        out.encoding = 'bgr8' if ch == 1 else m.encoding
        out.step, out.data = img.shape[1] * 3, np.ascontiguousarray(img).tobytes()
        self.pub_img.publish(out)


def main():
    rclpy.init()
    node = ArucoDetector()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):     # Ctrl+C / остановка launch — тихо
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
