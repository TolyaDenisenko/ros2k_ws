#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
scene_markers — показывает в RViz «истинную» сцену Gazebo: стол, pick_box,
cylinder_block. Нужна, чтобы сравнивать маркеры grasp_analyzer с реальным
положением объектов (в TF мира Gazebo нет — RViz видит только робота).

Геометрия
---------
  Поза объекта в base_footprint = inv(T_world<-robot) · T_world<-object, где
    T_world<-robot  — /gz/robot_pose (child_frame_id = robot_model_name;
                      корень SDF модели — base_footprint), 10 Гц;
    T_world<-object — подвижные объекты (object_names): /gz/object_pose
                      (10 Гц, запись по child_frame_id = имя модели);
                      стол и static-цилиндр (static_cylinder) — позы из
                      параметров (как в worlds/*.world).
  Маркеры публикуются в base_footprint, поэтому едут вместе с тележкой и
  видны при любом Fixed Frame из TF-дерева робота.

Топик: /scene_markers (visualization_msgs/MarkerArray), 2 Гц.

Параметры (дефолты = cart_table_offline.world; менять синхронно с миром)
  robot_model_name 'cart_xarm6'   root_frame 'base_footprint'
  object_names ['pick_box']   object_shapes ['box'] (box | cylinder)
  object_sizes [0.10, 0.05, 0.05]  (по 3 числа на объект; цилиндр: 2r 2r h)
  static_cylinder true — показывать static-цилиндр из cylinder_pose/size
  table_pose [1.05, 0, 0.90]  table_size [1.2, 0.8, 0.05]   (центр столешницы)
  cylinder_pose [0.80, -0.28, 0.9705]  cylinder_size [0.04, 0.09] (радиус, высота)
"""
import numpy as np
import rclpy
import rclpy.duration
from rclpy.node import Node
from tf2_msgs.msg import TFMessage
from visualization_msgs.msg import Marker, MarkerArray


def tf_to_matrix(tf):
    t, q = tf.transform.translation, tf.transform.rotation
    x, y, z, w = q.x, q.y, q.z, q.w
    T = np.eye(4)
    T[:3, :3] = [
        [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
        [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
        [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]]
    T[:3, 3] = [t.x, t.y, t.z]
    return T


def matrix_to_quat(R):
    # R -> (x, y, z, w), ветка по наибольшему диагональному элементу
    tr = R[0, 0] + R[1, 1] + R[2, 2]
    if tr > 0:
        s = 2.0 * np.sqrt(tr + 1.0)
        return ((R[2, 1] - R[1, 2]) / s, (R[0, 2] - R[2, 0]) / s,
                (R[1, 0] - R[0, 1]) / s, 0.25 * s)
    i = int(np.argmax([R[0, 0], R[1, 1], R[2, 2]]))
    j, k = (i + 1) % 3, (i + 2) % 3
    s = 2.0 * np.sqrt(1.0 + R[i, i] - R[j, j] - R[k, k])
    q = [0.0, 0.0, 0.0]
    q[i] = 0.25 * s
    q[j] = (R[j, i] + R[i, j]) / s
    q[k] = (R[k, i] + R[i, k]) / s
    return (q[0], q[1], q[2], (R[k, j] - R[j, k]) / s)


def translation(x, y, z):
    T = np.eye(4)
    T[:3, 3] = [x, y, z]
    return T


class SceneMarkers(Node):
    def __init__(self):
        super().__init__('scene_markers')
        self.declare_parameter('robot_model_name', 'cart_xarm6')
        self.declare_parameter('root_frame', 'base_footprint')
        self.declare_parameter('object_names', ['pick_box'])
        self.declare_parameter('object_shapes', ['box'])
        self.declare_parameter('object_sizes', [0.10, 0.05, 0.05])
        self.declare_parameter('static_cylinder', True)
        self.declare_parameter('table_pose', [1.05, 0.0, 0.90])
        self.declare_parameter('table_size', [1.2, 0.8, 0.05])
        self.declare_parameter('cylinder_pose', [0.80, -0.28, 0.9705])
        self.declare_parameter('cylinder_size', [0.04, 0.09])

        g = lambda n: self.get_parameter(n).value  # noqa: E731
        self.robot_name = g('robot_model_name')
        self.root_frame = g('root_frame')
        sizes = np.array(g('object_sizes'), float).reshape(-1, 3)
        types = {'box': Marker.CUBE, 'cylinder': Marker.CYLINDER}
        self.objects = {n: (types[sh], list(sz)) for n, sh, sz in
                        zip(g('object_names'), g('object_shapes'), sizes)}
        self.static_cylinder = bool(g('static_cylinder'))
        self.T_world_table = translation(*g('table_pose'))
        self.table_size = [float(v) for v in g('table_size')]
        self.T_world_cyl = translation(*g('cylinder_pose'))
        cr, ch = (float(v) for v in g('cylinder_size'))
        self.cyl_scale = (2 * cr, 2 * cr, ch)

        self.T_world_robot = None
        self.T_world_obj = {}
        for topic in ('/gz/robot_pose', '/gz/object_pose'):
            self.create_subscription(TFMessage, topic, self.on_poses, 5)
        self.pub = self.create_publisher(MarkerArray, '/scene_markers', 1)
        self.create_timer(0.5, self.publish)
        self.get_logger().info(f'scene_markers: /scene_markers в {self.root_frame}')

    def on_poses(self, msg: TFMessage):
        for tf in msg.transforms:
            if tf.child_frame_id == self.robot_name:
                self.T_world_robot = tf_to_matrix(tf)
            elif tf.child_frame_id in self.objects:
                self.T_world_obj[tf.child_frame_id] = tf_to_matrix(tf)

    def _marker(self, ns, type_, T_world, scale, color):
        T = np.linalg.inv(self.T_world_robot) @ T_world
        m = Marker()
        m.header.frame_id = self.root_frame
        m.header.stamp = self.get_clock().now().to_msg()
        m.ns, m.id, m.type, m.action = ns, 0, type_, Marker.ADD
        m.pose.position.x, m.pose.position.y, m.pose.position.z = map(float, T[:3, 3])
        q = matrix_to_quat(T[:3, :3])
        (m.pose.orientation.x, m.pose.orientation.y,
         m.pose.orientation.z, m.pose.orientation.w) = map(float, q)
        m.scale.x, m.scale.y, m.scale.z = map(float, scale)
        m.color.r, m.color.g, m.color.b, m.color.a = map(float, color)
        m.lifetime = rclpy.duration.Duration(seconds=2.0).to_msg()
        return m

    def publish(self):
        if self.T_world_robot is None:
            self.get_logger().warning('нет /gz/robot_pose — сцену не показываю',
                                   throttle_duration_sec=10.0)
            return
        arr = MarkerArray()
        arr.markers.append(self._marker('scene_table', Marker.CUBE, self.T_world_table,
                                        self.table_size, (0.7, 0.5, 0.3, 0.5)))
        if self.static_cylinder:
            arr.markers.append(self._marker('scene_cylinder', Marker.CYLINDER,
                                            self.T_world_cyl, self.cyl_scale,
                                            (0.1, 1.0, 0.1, 0.6)))
        for name, T in self.T_world_obj.items():
            type_, size = self.objects[name]
            arr.markers.append(self._marker(f'scene_{name}', type_, T, size,
                                            (0.1, 1.0, 0.1, 0.6)))
        self.pub.publish(arr)


def main():
    rclpy.init()
    node = SceneMarkers()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
