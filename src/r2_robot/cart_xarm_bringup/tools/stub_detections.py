#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
stub_detections — «детектор-заглушка» для сценария «рука на столе»: рамки считаются НЕЗАВИСИМО от
облака, из поз и габаритов объектов (/objects) через camera_info и TF, и публикуются как /detections
с классом `<prefix><id объекта>`. Если проекция в object_perception верна, каждый объект в /objects
получит detector_label = prefix+id (себя), без перепутанных соседей.

  # окно 1:  ros2 launch cart_xarm_bringup arm_table_gz.launch.py detector:=stub \
  #               use_moveit:=false headless:=true use_rviz:=false
  # окно 2 (подождать ~30 с, пока появятся объекты):
  python3 stub_detections.py --duration 40
  # проверка:
  ros2 topic echo --once /objects --field objects | grep -E "id=|detector_label="

`detector:=stub` включает у object_perception подписку на /detections, но настоящий детектор не
запускает (иначе у топика было бы два издателя).
"""
import argparse
import itertools
import time

import numpy as np
import rclpy
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from sensor_msgs.msg import CameraInfo
from tf2_ros import Buffer, TransformListener
from vision_msgs.msg import Detection2D, Detection2DArray, ObjectHypothesisWithPose
from xarm_grasping_msgs.msg import DetectedObjectArray


def quat_rot(x, y, z, w):
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--prefix', default='stub_')
    ap.add_argument('--duration', type=float, default=40.0)
    ap.add_argument('--camera-info', default='/gripper_camera/color/camera_info')
    ap.add_argument('--optical-frame', default='gripper_camera_color_optical_frame')
    ap.add_argument('--base-frame', default='link_base')
    a = ap.parse_args()
    rclpy.init()
    n = Node('stub_detections')
    buf = Buffer()
    TransformListener(buf, n)
    st = {}
    n.create_subscription(CameraInfo, a.camera_info,
                          lambda m: st.__setitem__('k', (m.k[0], m.k[4], m.k[2], m.k[5], m.width, m.height)), 1)
    n.create_subscription(DetectedObjectArray, '/objects', lambda m: st.__setitem__('o', m),
                          QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL))
    pub = n.create_publisher(Detection2DArray, '/detections', 5)
    t0, last = time.time(), 0
    while time.time() - t0 < a.duration and rclpy.ok():
        rclpy.spin_once(n, timeout_sec=0.2)
        if 'k' not in st or 'o' not in st:
            continue
        try:
            tf = buf.lookup_transform(a.optical_frame, a.base_frame, rclpy.time.Time())
        except Exception:  # noqa: BLE001
            continue
        r = tf.transform.rotation
        R = quat_rot(r.x, r.y, r.z, r.w)
        T = np.array([tf.transform.translation.x, tf.transform.translation.y, tf.transform.translation.z])
        fx, fy, cx, cy, W, H = st['k']
        out = Detection2DArray()
        out.header.frame_id = a.optical_frame
        out.header.stamp = n.get_clock().now().to_msg()
        for o in st['o'].objects:
            d = np.array(o.dimensions) if o.shape == 1 else np.array(
                [2 * o.dimensions[1], 2 * o.dimensions[1], o.dimensions[0]])   # коробка XYZ; цилиндр [h, r]
            q = o.pose.orientation
            Rz = quat_rot(q.x, q.y, q.z, q.w)
            c = np.array([o.pose.position.x, o.pose.position.y, o.pose.position.z])
            corners = np.array([c + Rz @ (np.array(s) * d / 2) for s in itertools.product((-1, 1), repeat=3)])
            pc = corners @ R.T + T
            if (pc[:, 2] <= 0.05).any():
                continue
            u, v = fx * pc[:, 0] / pc[:, 2] + cx, fy * pc[:, 1] / pc[:, 2] + cy
            x1, x2, y1, y2 = u.min(), u.max(), v.min(), v.max()
            if x2 < 0 or y2 < 0 or x1 > W or y1 > H:
                continue
            det = Detection2D(header=out.header, id=o.id)
            h = ObjectHypothesisWithPose()
            h.hypothesis.class_id, h.hypothesis.score = a.prefix + o.id, 0.9
            det.results.append(h)
            det.bbox.center.position.x, det.bbox.center.position.y = float((x1 + x2) / 2), float((y1 + y2) / 2)
            det.bbox.size_x, det.bbox.size_y = float(x2 - x1), float(y2 - y1)
            out.detections.append(det)
        pub.publish(out)
        last = len(out.detections)
    print(f'опубликовано рамок в последнем кадре: {last}')


if __name__ == '__main__':
    main()
