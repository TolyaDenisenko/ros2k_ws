#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
vacuum_rviz_button — кнопка вакуума в RViz (интерактивный маркер у присоски).

Клик левой кнопкой мыши по кубику (инструмент Interact в RViz) переключает
вакуум через сервис /vacuum_gripper/set (vacuum_gripper_sim). Правая кнопка —
меню «Включить / Выключить». Цвет и надпись — готовность к захвату
(по /vacuum_gripper/contact от vacuum_gripper_sim):
  серый         — присоска далеко от объектов (надпись: ближайший, мм);
  жёлтый        — подводим, ближе near_distance (мм и угол до грани);
  светло-зелёный, крупнее — МОЖНО БРАТЬ: клик включит вакуум;
  зелёный       — объект удерживается (имя объекта);
  красный       — последняя попытка включить отклонена (причина, ~5 с).

RViz: дисплей InteractiveMarkers, Interactive Markers Namespace = /vacuum_button
(уже добавлен в cart_xarm_moveit.rviz, cart_xarm.rviz, arm_table.rviz).

Параметры
  frame 'link_tcp'         — к какому кадру прикреплена кнопка (едет с рукой)
  offset [0.0, 0.12, 0.0]  — смещение кнопки от кадра, м (чтобы не закрывать присоску)
"""
import json

import rclpy
from interactive_markers import InteractiveMarkerServer, MenuHandler
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from std_msgs.msg import String
from std_srvs.srv import SetBool
from visualization_msgs.msg import (InteractiveMarker, InteractiveMarkerControl,
                                    InteractiveMarkerFeedback, Marker)

NAME = 'vacuum_button'


class VacuumButton(Node):
    def __init__(self):
        super().__init__('vacuum_rviz_button')
        self.declare_parameter('frame', 'link_tcp')
        self.declare_parameter('offset', [0.0, 0.12, 0.0])
        self.frame = self.get_parameter('frame').value
        self.offset = [float(v) for v in self.get_parameter('offset').value]

        self.held = ''
        self.contact = {}          # последнее /vacuum_gripper/contact
        self.last_view = None      # (цвет, текст) — перерисовка только при изменении
        self.last_draw = 0.0
        self.error = ''            # причина последнего отказа (показывается ~5 с)
        self.error_timer = None
        self.busy = False

        self.cli = self.create_client(SetBool, '/vacuum_gripper/set')
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.create_subscription(String, '/vacuum_gripper/held', self.on_held, latched)
        self.create_subscription(String, '/vacuum_gripper/contact', self.on_contact, 10)

        self.server = InteractiveMarkerServer(self, 'vacuum_button')
        self.menu = MenuHandler()
        self.menu.insert('Включить вакуум', callback=lambda fb: self.request(True))
        self.menu.insert('Выключить вакуум', callback=lambda fb: self.request(False))
        self.redraw()
        self.get_logger().info(
            'Кнопка вакуума в RViz: дисплей InteractiveMarkers, namespace '
            '/vacuum_button, инструмент Interact, клик по кубику у присоски')

    # ---------------------------------------------------------------- маркер
    def view(self):
        """(цвет, текст, размер кубика) по текущему состоянию."""
        c = self.contact
        if self.held:
            return (0.0, 0.8, 0.0), f'ВАКУУМ: держит {self.held}', 0.04
        if self.error:
            return (0.9, 0.1, 0.1), self.error, 0.04
        lvl, obj = c.get('level'), c.get('object', '')
        mm = f"{c['dist'] * 1000:.0f} мм" if c.get('dist') is not None else ''
        if lvl == 'ready':
            return (0.3, 1.0, 0.3), f'МОЖНО БРАТЬ: {obj} {mm} — клик', 0.055
        if lvl == 'near':
            return (1.0, 0.8, 0.0), f"{obj}: {mm}, {c['angle']:.0f}°", 0.04
        if lvl == 'far':
            return (0.6, 0.6, 0.6), f'ВАКУУМ выкл · {obj}: {mm}', 0.04
        return (0.6, 0.6, 0.6), 'ВАКУУМ: выкл (клик — вкл)', 0.04

    def on_contact(self, msg: String):
        try:
            self.contact = json.loads(msg.data)
        except ValueError:
            return
        now = self.get_clock().now().nanoseconds * 1e-9
        if self.view() != self.last_view and now - self.last_draw >= 0.2:
            self.redraw()

    def redraw(self):
        color, text, size = self.view()
        self.last_view = (color, text, size)
        self.last_draw = self.get_clock().now().nanoseconds * 1e-9
        if self.busy:
            text = '... ' + text

        im = InteractiveMarker()
        im.header.frame_id = self.frame
        im.name = NAME
        im.description = ''
        im.scale = 0.05
        im.pose.position.x, im.pose.position.y, im.pose.position.z = self.offset
        im.pose.orientation.w = 1.0

        box = Marker()
        box.type = Marker.CUBE
        box.scale.x = box.scale.y = box.scale.z = size
        box.color.r, box.color.g, box.color.b = color
        box.color.a = 0.9
        label = Marker()
        label.type = Marker.TEXT_VIEW_FACING
        label.scale.z = 0.025
        label.pose.position.z = 0.045
        label.color.r = label.color.g = label.color.b = label.color.a = 1.0
        label.text = text

        ctrl = InteractiveMarkerControl()
        ctrl.interaction_mode = InteractiveMarkerControl.BUTTON
        ctrl.always_visible = True
        ctrl.markers = [box, label]
        im.controls.append(ctrl)

        self.server.insert(im, feedback_callback=self.on_feedback)
        self.menu.apply(self.server, NAME)
        self.server.applyChanges()

    def on_feedback(self, fb: InteractiveMarkerFeedback):
        if fb.event_type == InteractiveMarkerFeedback.BUTTON_CLICK:
            self.request(not self.held)

    # ---------------------------------------------------------------- вакуум
    def request(self, on: bool):
        if self.busy:
            return
        if not self.cli.service_is_ready():
            self.show_error('нет сервиса /vacuum_gripper/set')
            return
        self.busy = True
        self.redraw()
        fut = self.cli.call_async(SetBool.Request(data=on))
        fut.add_done_callback(self.on_result)

    def on_result(self, fut):
        self.busy = False
        res = fut.result()
        if res is not None and not res.success:
            self.show_error(res.message.replace('отказ: ', 'отказ:\n'))
        else:
            self.redraw()

    def show_error(self, text):
        self.error = text
        if self.error_timer is not None:
            self.error_timer.cancel()
        self.error_timer = self.create_timer(5.0, self.clear_error)
        self.redraw()

    def clear_error(self):
        self.error_timer.cancel()
        self.error_timer = None
        self.error = ''
        self.redraw()

    def on_held(self, msg: String):
        self.held = msg.data
        if self.held:
            self.error = ''
        self.redraw()


def main():
    rclpy.init()
    node = VacuumButton()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
