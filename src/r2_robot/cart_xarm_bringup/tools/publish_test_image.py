#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
publish_test_image — публикует картинку как кадр камеры: проверить openvino_detector без симуляции.

  python3 publish_test_image.py ~/.local/share/quasi/test_images/000000039769.jpg
  python3 publish_test_image.py фото.jpg --topic /gripper_camera/color/image_raw --rate 10

Рядом запустить детектор:
  ~/.venvs/openvino/bin/python ~/ros2k_ws/install/xarm_grasping/lib/xarm_grasping/openvino_detector.py \
      --ros-args --params-file ~/ros2k_ws/install/xarm_grasping/share/xarm_grasping/config/openvino_detector.yaml \
      -p conf:=0.4 -p device:=CPU
  ros2 topic echo /detections --field detections          # рамки; ros2 run rqt_image_view ... /detections/image
"""
import argparse
import os
import time

import cv2
import rclpy
from cv_bridge import CvBridge
from rclpy.node import Node
from sensor_msgs.msg import Image


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('image')
    ap.add_argument('--topic', default='/gripper_camera/color/image_raw')
    ap.add_argument('--rate', type=float, default=10.0)
    a = ap.parse_args()
    img = cv2.imread(os.path.expanduser(a.image))
    if img is None:
        raise SystemExit(f'не прочитать {a.image}')
    rclpy.init()
    node = Node('publish_test_image')
    pub = node.create_publisher(Image, a.topic, 10)
    msg = CvBridge().cv2_to_imgmsg(cv2.cvtColor(img, cv2.COLOR_BGR2RGB), 'rgb8')
    msg.header.frame_id = 'gripper_camera_color_optical_frame'
    node.get_logger().info(f'публикую {a.image} ({img.shape[1]}x{img.shape[0]}) в {a.topic}, {a.rate:g} Гц')
    try:
        while rclpy.ok():
            msg.header.stamp = node.get_clock().now().to_msg()
            pub.publish(msg)
            time.sleep(1.0 / a.rate)
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
