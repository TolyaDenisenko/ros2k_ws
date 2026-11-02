#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
openvino_detector — нода детекции объектов по цветному кадру на OpenVINO.

  вход   image_topic (sensor_msgs/Image)  — по умолчанию /gripper_camera/color/image_raw
  выход  detections  (vision_msgs/Detection2DArray) — рамки в пикселях, header как у кадра;
         results[0].hypothesis.class_id = имя класса, .score = уверенность
         detections/image (sensor_msgs/Image, только при publish_debug_image) — кадр с рамками

Ядро — xarm_grasping/detector.py (без ROS, проверяется отдельно). Нода работает на системном времени
(use_sim_time=false): ей не нужен /clock, а метка кадра берётся из самого сообщения.

OpenVINO ставится в отдельное окружение (pip install openvino), поэтому запускать эту ноду надо его
интерпретатором, например ~/.venvs/openvino/bin/python (launch делает это сам).

Параметры
  model_path  ~/.local/share/quasi/models/yolox/yolox_s.onnx   (ONNX или IR .xml)
  model_type  yolox          labels_path '' (COCO, 80 классов)
  device      AUTO           (CPU | GPU | NPU | AUTO; на NUC — что окажется быстрее)
  conf 0.4   nms 0.45   max_rate_hz 5.0 (0 — без ограничения)   publish_debug_image false
  num_threads 0   (потоков вывода на CPU; 0 — все ядра, это 150–170% CPU у yolox_s: на NUC ставьте 2–4)
"""
import time

import cv2
import rclpy
from cv_bridge import CvBridge
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import Image
from vision_msgs.msg import Detection2D, Detection2DArray, ObjectHypothesisWithPose

from xarm_grasping.detector import OpenVinoDetector, draw


class DetectorNode(Node):
    def __init__(self):
        super().__init__('openvino_detector')
        p = self.declare_parameter
        model = p('model_path', '~/.local/share/quasi/models/yolox/yolox_s.onnx').value
        self.max_rate = float(p('max_rate_hz', 5.0).value)
        self.debug = bool(p('publish_debug_image', False).value)
        device = p('device', 'AUTO').value
        self.det = OpenVinoDetector(
            model, device, conf=p('conf', 0.4).value, nms=p('nms', 0.45).value,
            labels=p('labels_path', '').value, model_type=p('model_type', 'yolox').value,
            num_threads=p('num_threads', 0).value)
        self.bridge = CvBridge()
        self.last = 0.0
        self.frames = 0
        self.pub = self.create_publisher(Detection2DArray, 'detections', 10)
        self.pub_img = self.create_publisher(Image, 'detections/image', 1) if self.debug else None
        self.create_subscription(Image, p('image_topic', '/gripper_camera/color/image_raw').value,
                                 self.on_image, qos_profile_sensor_data)
        self.get_logger().info(
            f'детектор готов: {model}, вход {self.det.size}, устройство {device}, '
            f'не чаще {self.max_rate:g} Гц')

    def on_image(self, msg: Image):
        now = time.monotonic()
        if self.max_rate > 0 and now - self.last < 1.0 / self.max_rate:
            return
        self.last = now
        bgr = self.bridge.imgmsg_to_cv2(msg, 'bgr8')
        dets = self.det.detect(bgr)
        out = Detection2DArray(header=msg.header)
        for i, d in enumerate(dets):
            det = Detection2D(header=msg.header, id=f'{d.label}_{i}')
            hyp = ObjectHypothesisWithPose()
            hyp.hypothesis.class_id = d.label
            hyp.hypothesis.score = d.score
            det.results.append(hyp)
            det.bbox.center.position.x = d.cx
            det.bbox.center.position.y = d.cy
            det.bbox.size_x = d.x2 - d.x1
            det.bbox.size_y = d.y2 - d.y1
            out.detections.append(det)
        self.pub.publish(out)
        if self.pub_img is not None:
            self.pub_img.publish(self.bridge.cv2_to_imgmsg(draw(bgr, dets), 'bgr8', msg.header))
        self.frames += 1
        if self.frames % 50 == 1:
            self.get_logger().info(
                f'кадр {self.frames}: {len(dets)} объектов, {self.det.timing["total_ms"]:.0f} мс '
                f'(сеть {self.det.timing["infer_ms"]:.0f} мс)')


def main():
    rclpy.init()
    node = DetectorNode()
    try:
        rclpy.spin(node)           # одиночный исполнитель: у многопоточного лишний расход CPU
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
