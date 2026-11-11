#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
pick_by_name — «возьми куб 2»: принимает команду текстом, находит предмет на столе и берёт его
через /pick_place. Если предметов этого вида несколько, а номер не назван — спрашивает
«этот?», показывая стрелку на предмете.

Команды (топик /pick_command, std_msgs/String; из приложения, rosbridge или `ros2 topic pub`)
  <название> <номер>   — взять конкретный: «куб 2», «cube 2», «пластина 1»;
  <название>           — один такой предмет на столе: берёт; несколько — диалог «этот?»;
  да | yes | ок        — в диалоге: взять показанный предмет;
  нет | no | дальше    — в диалоге: показать следующий;
  стоп | отмена | stop — отменить диалог и текущее взятие;
  список | list        — ответить, что есть на столе.
Названия и синонимы — config/object_catalog.yaml.

Номер предмета — порядковый номер среди предметов ЭТОГО класса на столе, считая по положению:
сначала меньшее y, при равных — меньшее x (в системе руки link_base). Пока предметы не двигают,
номера не меняются; после перестановки порядок считается заново. Номер показан на картинке.

Ответы (топик /pick_status, std_msgs/String, transient_local): понятный текст для приложения —
«ищу…», «беру куб 2», «этот куб? (да/нет)», «готово», «нет такого предмета» и т.д.
Для программы то же в /pick_status_json (JSON: state, text, class, number, object_id).

Подсказка в диалоге (когда рука в позе наблюдения — после каждого взятия она там):
  /pick_by_name/image   — кадр камеры со стрелкой на предмете, подпись «куб 2 — этот?»; пока диалог
                          открыт, обновляется раз в секунду (окно можно открыть в любой момент);
  /pick_by_name/marker  — стрелка и подпись в RViz (MarkerArray, link_base).

Параметры
  catalog (путь к yaml, по умолчанию config/object_catalog.yaml пакета)
  min_detector_score 0.3 — уверенность нейросети, с которой верим её названию (ниже — класс по размерам)
  image_topic /gripper_camera/color/image_raw   camera_info_topic /gripper_camera/color/camera_info
  base_frame link_base   objects_topic /objects (для проверок можно подменить)
"""
import json
import os
import threading
import time

import numpy as np
import rclpy
import yaml
from ament_index_python.packages import get_package_share_directory
from geometry_msgs.msg import Point
from rclpy.action import ActionClient
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from sensor_msgs.msg import CameraInfo, Image
from std_msgs.msg import String
from tf2_ros import Buffer, TransformListener
from visualization_msgs.msg import Marker, MarkerArray

from xarm_grasping_msgs.action import PickPlace
from xarm_grasping_msgs.msg import DetectedObjectArray

YES = {'да', 'yes', 'y', 'ок', 'ok', 'этот', 'верно'}
NO = {'нет', 'no', 'n', 'дальше', 'next', 'другой'}
STOP = {'стоп', 'stop', 'отмена', 'cancel'}
LIST = {'список', 'list', 'что на столе'}


def ordered(objs):
    """Порядок предметов одного класса: меньшее y, при равных меньшее x (номера 1, 2, …)."""
    return sorted(objs, key=lambda o: (round(o.pose.position.y, 3), o.pose.position.x))


class PickByName(Node):
    def __init__(self):
        super().__init__('pick_by_name')
        dp = self.declare_parameter
        dp('catalog', '')
        dp('min_detector_score', 0.3)
        dp('image_topic', '/gripper_camera/color/image_raw')
        dp('camera_info_topic', '/gripper_camera/color/camera_info')
        dp('base_frame', 'link_base')
        dp('objects_topic', '/objects')
        path = self.get_parameter('catalog').value or os.path.join(
            get_package_share_directory('xarm_grasping'), 'config', 'object_catalog.yaml')
        self.catalog = yaml.safe_load(open(path))['catalog']
        self.alias = {}
        for cls, e in self.catalog.items():
            for a in e['aliases'] + [cls]:
                self.alias[str(a).lower()] = cls
        self.title = {cls: e['aliases'][0] for cls, e in self.catalog.items()}

        cbg = ReentrantCallbackGroup()
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL,
                             reliability=ReliabilityPolicy.RELIABLE)
        self.pub_status = self.create_publisher(String, '/pick_status', latched)
        self.pub_json = self.create_publisher(String, '/pick_status_json', latched)
        self.pub_marker = self.create_publisher(MarkerArray, '/pick_by_name/marker', 10)
        self.pub_image = self.create_publisher(Image, '/pick_by_name/image', 1)
        self.create_subscription(String, '/pick_command', self.on_command, 10,
                                 callback_group=cbg)
        self.create_subscription(DetectedObjectArray, self.get_parameter('objects_topic').value,
                                 self.on_objects, latched,
                                 callback_group=cbg)
        self.create_subscription(Image, self.get_parameter('image_topic').value, self.on_image, 1,
                                 callback_group=cbg)
        self.create_subscription(CameraInfo, self.get_parameter('camera_info_topic').value,
                                 lambda m: setattr(self, 'cam', m), 1, callback_group=cbg)
        self.client = ActionClient(self, PickPlace, '/pick_place', callback_group=cbg)
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

        self.objects = []          # последний /objects
        self.image = None
        self.cam = None
        self.dialog = None         # {'cls', 'cands': [...], 'i': int}
        self.shown = None          # (предмет, подпись, подпись на картинке) пока открыт диалог
        self.busy = False
        self.goal_handle = None
        self.lock = threading.Lock()
        # пока диалог открыт, подсказку на картинке обновляем раз в секунду: окно, подключившееся
        # позже, или пропущенный кадр не оставят пустой экран, а кадр камеры не «замерзает»
        self.create_timer(1.0, self.refresh_hint, callback_group=cbg)
        self.say('state_idle', 'готов: напишите, что взять, например «куб 1»')
        self.get_logger().info(f'готов: /pick_command, классы: {", ".join(self.catalog)}')

    # ------------------------------------------------------------ вывод
    def say(self, state, text, cls='', number=0, object_id=''):
        self.get_logger().info(f'[{state}] {text}')
        self.pub_status.publish(String(data=text))
        self.pub_json.publish(String(data=json.dumps(
            {'state': state, 'text': text, 'class': cls, 'number': number,
             'object_id': object_id}, ensure_ascii=False)))

    def on_objects(self, msg):
        self.objects = list(msg.objects)

    def on_image(self, msg):
        self.image = msg

    # ------------------------------------------------------------ разбор
    def cls_of(self, o):
        """Класс предмета: название нейросети, если ей верим, иначе по размерам."""
        if o.detector_label and o.detector_score >= float(self.get_parameter('min_detector_score').value):
            return o.detector_label
        return o.label

    def by_class(self, cls):
        return ordered([o for o in self.objects if self.cls_of(o) == cls])

    def parse(self, text):
        """-> (класс, номер или 0) либо (None, 0)."""
        words = text.lower().replace(',', ' ').split()
        number = 0
        if words and words[-1].isdigit():
            number = int(words[-1])
            words = words[:-1]
        cls = self.alias.get(' '.join(words)) or (self.alias.get(words[0]) if words else None)
        return cls, number

    # ------------------------------------------------------------ команды
    def on_command(self, msg):
        text = msg.data.strip().lower()
        if not text:
            return
        if text in STOP:
            return self.cancel()
        if text in LIST:
            return self.list_objects()
        if self.dialog and text in YES:
            return self.confirm()
        if self.dialog and text in NO:
            return self.next_candidate()
        if self.busy:
            return self.say('busy', 'занят: уже выполняю взятие. Скажите «стоп», чтобы отменить')
        cls, number = self.parse(text)
        if cls is None:
            known = ', '.join(self.title.values())
            return self.say('unknown', f'не понял «{msg.data.strip()}». Я знаю: {known}')
        self.dialog = None
        cands = self.by_class(cls)
        if not cands:
            return self.say('not_found', f'нет такого предмета на столе: {self.title[cls]}', cls)
        if number:
            if number > len(cands):
                return self.say('not_found', f'{self.title[cls]} {number}: на столе только '
                                f'{len(cands)} шт.', cls, number)
            return self.start_pick(cls, number, cands[number - 1])
        if len(cands) == 1:
            return self.start_pick(cls, 1, cands[0])
        self.dialog = {'cls': cls, 'cands': cands, 'i': 0}
        self.ask()

    def list_objects(self):
        parts = []
        for cls in self.catalog:
            n = len(self.by_class(cls))
            if n:
                parts.append(f'{self.title[cls]} — {n}')
        self.say('list', 'на столе: ' + (', '.join(parts) if parts else 'ничего не видно'))

    # ------------------------------------------------------------ диалог «этот?»
    def ask(self):
        d = self.dialog
        o = d['cands'][d['i']]
        n = d['i'] + 1
        self.say('ask', f'{self.title[d["cls"]]} {n} из {len(d["cands"])} — этот? (да / нет / стоп)',
                 d['cls'], n, o.id)
        self.shown = (o, f'{self.title[d["cls"]]} {n} — этот?', f'{d["cls"]} {n} - this one?')
        self.show(*self.shown)

    def refresh_hint(self):
        if self.dialog and self.shown:
            o, _, image_label = self.shown
            top = Point(x=o.grasp_pose.position.x, y=o.grasp_pose.position.y,
                        z=o.grasp_pose.position.z)
            self.draw_on_image(top, image_label)

    def next_candidate(self):
        d = self.dialog
        d['i'] += 1
        if d['i'] >= len(d['cands']):
            self.dialog = None
            self.clear_marker()
            return self.say('not_found', 'больше таких предметов нет')
        self.ask()

    def confirm(self):
        d, self.dialog = self.dialog, None
        self.clear_marker()
        self.start_pick(d['cls'], d['i'] + 1, d['cands'][d['i']])

    def cancel(self):
        self.dialog = None
        self.clear_marker()
        if self.goal_handle is not None:
            self.goal_handle.cancel_goal_async()
            return self.say('cancel', 'отменяю взятие')
        self.say('cancel', 'отменено')

    # ------------------------------------------------------------ взятие
    def start_pick(self, cls, number, obj):
        if not self.client.wait_for_server(timeout_sec=2.0):
            return self.say('error', 'нет сервера /pick_place (запущен ли MoveIt?)', cls, number)
        with self.lock:
            self.busy = True
        title = f'{self.title[cls]} {number}'
        self.say('picking', f'беру {title}', cls, number, obj.id)
        goal = PickPlace.Goal(object_id=obj.id, zone=self.catalog[cls].get('zone', ''))

        def fb(m):
            self.say('progress', f'{title}: {m.feedback.stage} — {m.feedback.detail}', cls, number,
                     obj.id)

        fut = self.client.send_goal_async(goal, feedback_callback=fb)
        fut.add_done_callback(lambda f: self.on_accepted(f, cls, number, obj.id, title))

    def on_accepted(self, fut, cls, number, oid, title):
        gh = fut.result()
        if not gh.accepted:
            self.busy = False
            return self.say('error', f'{title}: задание отклонено', cls, number, oid)
        self.goal_handle = gh
        gh.get_result_async().add_done_callback(
            lambda f: self.on_result(f, cls, number, oid, title))

    def on_result(self, fut, cls, number, oid, title):
        r = fut.result().result
        self.busy, self.goal_handle = False, None
        if r.success:
            self.say('done', f'готово: {title} поставлен', cls, number, oid)
        else:
            self.say('failed', f'не удалось взять {title}: {r.message}', cls, number, oid)

    # ------------------------------------------------------------ подсказка на картинке и в RViz
    def clear_marker(self):
        m = Marker()
        m.action = Marker.DELETEALL
        self.pub_marker.publish(MarkerArray(markers=[m]))

    def show(self, o, label, image_label):
        base = self.get_parameter('base_frame').value
        top = Point(x=o.grasp_pose.position.x, y=o.grasp_pose.position.y,
                    z=o.grasp_pose.position.z)
        arrow = Marker()
        arrow.header.frame_id = base
        arrow.ns, arrow.id, arrow.type, arrow.action = 'pick', 1, Marker.ARROW, Marker.ADD
        arrow.points = [Point(x=top.x, y=top.y, z=top.z + 0.12), top]
        arrow.scale.x, arrow.scale.y, arrow.scale.z = 0.025, 0.05, 0.06      # толстая
        arrow.color.r, arrow.color.g, arrow.color.b, arrow.color.a = 1.0, 0.0, 0.0, 1.0  # красная
        text = Marker()
        text.header.frame_id = base
        text.ns, text.id, text.type, text.action = 'pick', 2, Marker.TEXT_VIEW_FACING, Marker.ADD
        text.pose.position = Point(x=top.x, y=top.y, z=top.z + 0.16)
        text.scale.z = 0.03
        text.color.r = text.color.g = text.color.b = text.color.a = 1.0
        text.text = label
        self.pub_marker.publish(MarkerArray(markers=[arrow, text]))
        self.draw_on_image(top, image_label)       # cv2.putText не знает кириллицу

    def draw_on_image(self, top, label):
        """Кадр камеры со стрелкой над предметом (проекция точки через TF и camera_info)."""
        import cv2
        img, cam = self.image, self.cam
        if img is None or cam is None or img.encoding not in ('rgb8', 'bgr8'):
            return
        try:
            tf = self.tf_buffer.lookup_transform(img.header.frame_id,
                                                 self.get_parameter('base_frame').value,
                                                 rclpy.time.Time())
        except Exception as e:  # noqa: BLE001
            self.get_logger().warning(f'нет TF для подсказки на картинке: {e}')
            return
        t, q = tf.transform.translation, tf.transform.rotation
        x, y, z, w = q.x, q.y, q.z, q.w
        R = np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                      [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                      [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])

        def proj(px, py, pz):
            c = R @ np.array([px, py, pz]) + np.array([t.x, t.y, t.z])
            if c[2] <= 0.01:
                return None
            return int(cam.k[0] * c[0] / c[2] + cam.k[2]), int(cam.k[4] * c[1] / c[2] + cam.k[5])

        tip, tail = proj(top.x, top.y, top.z), proj(top.x, top.y, top.z + 0.12)
        arr = np.frombuffer(img.data, np.uint8).reshape(img.height, img.width, 3).copy()
        if tip and tail:
            # зеркально по диагонали: хвост симметрично относительно острия (стрелка идёт сверху
            # справа вниз-влево, а не снизу слева вверх-вправо)
            tail = (2 * tip[0] - tail[0], 2 * tip[1] - tail[1])
            color = (0, 0, 255) if img.encoding == 'bgr8' else (255, 0, 0)      # красный
            cv2.arrowedLine(arr, tail, tip, (255, 255, 255), 14, tipLength=0.4)  # белая обводка
            cv2.arrowedLine(arr, tail, tip, color, 9, tipLength=0.4)              # толстая стрелка
            pos = (min(max(tail[0] - 140, 5), img.width - 330), min(max(tail[1] - 18, 30), img.height - 10))
            cv2.putText(arr, label, pos, cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 255, 255), 6)
            cv2.putText(arr, label, pos, cv2.FONT_HERSHEY_SIMPLEX, 0.9, color, 3)
        out = Image()
        out.header, out.height, out.width = img.header, img.height, img.width
        out.encoding, out.step, out.data = img.encoding, img.width * 3, arr.tobytes()
        self.pub_image.publish(out)


def main():
    rclpy.init()
    node = PickByName()
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
