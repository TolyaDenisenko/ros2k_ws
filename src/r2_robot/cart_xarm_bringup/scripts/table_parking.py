#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
table_parking — команда «паркуйся»: тележка встаёт у стола БОКОМ, правым бортом (сторона руки).

Запуск (входит в cart_nav_gz.launch.py, аргумент parking:=true):
  ros2 service call /park std_srvs/srv/Trigger                        # начать
  ros2 topic pub --once /park_command std_msgs/String "data: 'паркуйся'"   # то же текстом
  ros2 service call /park/cancel std_srvs/srv/Trigger                 # остановить
  ros2 topic echo /park/status                                        # ход и итог (latched)

Шаги (docs/NAV.md §8б):
  1. найти метку ArUco на торце стола (/aruco/marker_pose); не видно — поворот на месте;
  2. усреднить N замеров и перевести в map;
  3. поза стола: метка = центр торца, ось стола = от камеры внутрь стола;
  4. точки: стоянка (правый борт в gap от кромки, рука напротив `along`) и подход (approach до неё);
  5. Nav2 NavigateToPose до точки подхода;
  6. доводка сама: поворот на курс вдоль кромки, затем прямо, держа линию (/cmd_vel, TF map);
  7. проверка итоговой позы (допуски), публикация «стою».

Узел без use_sim_time: время в нём — настенное, TF берётся «последний».
"""
import math
import threading
import time

import numpy as np
import rclpy
from geometry_msgs.msg import PoseStamped, Twist
from sensor_msgs.msg import LaserScan
from nav2_msgs.action import NavigateToPose
from rclpy.action import ActionClient
from rclpy.executors import ExternalShutdownException, SingleThreadedExecutor
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from std_msgs.msg import String
from std_srvs.srv import Trigger
from tf2_ros import Buffer, TransformListener

# Геометрия тележки c2l-v3 в base_footprint (URDF): основание руки и половина ширины корпуса.
ARM_X = 0.1125
HALF_WIDTH = 0.32


class Cancelled(Exception):
    pass


class ParkError(Exception):
    pass


def yaw_of(q):
    return math.atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))


def quat_rot(q, v):
    """Повернуть вектор v кватернионом (x, y, z, w)."""
    x, y, z, w = q
    u = np.array([x, y, z])
    v = np.asarray(v, float)
    return 2 * u.dot(v) * u + (w * w - u.dot(u)) * v + 2 * w * np.cross(u, v)


def wrap(a):
    return math.atan2(math.sin(a), math.cos(a))


class TableParking(Node):
    def __init__(self):
        super().__init__('table_parking')
        dp = self.declare_parameter
        dp('map_frame', 'map')
        dp('base_frame', 'base_footprint')
        dp('table_length', 1.2)        # м, вдоль оси от торца с меткой
        dp('table_width', 0.8)         # м
        dp('side', 1)                  # +1 — кромка слева от оси стола (смотря от метки внутрь), −1 — справа
        dp('along', 0.3)               # м от торца: где встанет основание руки
        dp('gap', 0.07)                # м от кромки стола до правого борта
        dp('approach', 0.9)            # м: точка подхода раньше стоянки (вне стола)
        dp('samples', 10)              # замеров метки для усреднения
        dp('max_marker_dist', 2.8)     # м: дальше метке не верим
        dp('search_speed', 0.3)        # рад/с поворот при поиске
        dp('creep_speed', 0.10)        # м/с доводка
        dp('turn_speed', 0.4)          # рад/с макс. при доводке
        dp('k_lat', 2.0)               # рад/с на м бокового отклонения
        dp('k_head', 1.5)              # рад/с на рад курса
        dp('tol_xy', 0.03)             # м
        dp('tol_yaw', 0.035)           # рад (~2°)
        dp('nav_timeout', 120.0)       # с
        dp('search_clearance', 0.75)   # м: ближе препятствие по /scan — поворот для поиска метки запрещён
        self.p = lambda n: self.get_parameter(n).value

        self.tf = Buffer()
        TransformListener(self.tf, self)
        lat = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.pub_status = self.create_publisher(String, '/park/status', lat)
        self.pub_cmd = self.create_publisher(Twist, '/cmd_vel', 10)
        self.pub_target = self.create_publisher(PoseStamped, '/park/target', lat)
        self.create_subscription(PoseStamped, '/aruco/marker_pose', self.on_marker, 10)
        self.create_subscription(String, '/park_command', self.on_text, 10)
        self.scan_min = None
        self.create_subscription(LaserScan, '/scan', self.on_scan, 5)
        self.create_service(Trigger, '/park', self.on_park)
        self.create_service(Trigger, '/park/cancel', self.on_cancel)
        self.nav = ActionClient(self, NavigateToPose, 'navigate_to_pose')
        self.marker = None          # (время приёма, PoseStamped)
        self.worker = None
        self.cancel = threading.Event()
        self.nav_handle = None
        self.status('жду команду /park')

    # ---------- интерфейс ----------
    def status(self, text):
        self.get_logger().info(text)
        self.pub_status.publish(String(data=text))

    def on_marker(self, m):
        self.marker = (time.monotonic(), m)

    def on_scan(self, m):
        r = [x for x in m.ranges if m.range_min < x < m.range_max]
        self.scan_min = min(r) if r else float('inf')

    def on_text(self, m):
        t = m.data.strip().lower()
        if t.startswith('парк') or t in ('park', 'dock'):
            self.start()
        elif t in ('стоп', 'stop', 'отмена', 'cancel'):
            self.stop()

    def on_park(self, req, res):
        res.success, res.message = self.start()
        return res

    def on_cancel(self, req, res):
        self.stop()
        res.success, res.message = True, 'отмена'
        return res

    def start(self):
        if self.worker and self.worker.is_alive():
            return False, 'парковка уже идёт'
        self.cancel.clear()
        self.worker = threading.Thread(target=self.run, daemon=True)
        self.worker.start()
        return True, 'начал парковку, ход — /park/status'

    def stop(self):
        self.cancel.set()
        if self.nav_handle is not None:
            self.nav_handle.cancel_goal_async()
        self.halt()

    # ---------- помощники ----------
    def halt(self):
        self.pub_cmd.publish(Twist())

    def check(self):
        if self.cancel.is_set():
            raise Cancelled()

    def sleep(self, s):
        end = time.monotonic() + s
        while time.monotonic() < end:
            self.check()
            time.sleep(0.02)

    def wait_future(self, fut, timeout):
        end = time.monotonic() + timeout
        while not fut.done():
            self.check()
            if time.monotonic() > end:
                raise ParkError('нет ответа (тайм-аут)')
            time.sleep(0.05)
        return fut.result()

    def robot_pose(self):
        t = self.tf.lookup_transform(self.p('map_frame'), self.p('base_frame'), rclpy.time.Time())
        tr = t.transform
        return tr.translation.x, tr.translation.y, yaw_of(tr.rotation)

    def fresh_marker(self, max_age=0.5):
        if self.marker and time.monotonic() - self.marker[0] < max_age:
            m = self.marker[1]
            p = m.pose.position
            if math.sqrt(p.x ** 2 + p.y ** 2 + p.z ** 2) <= self.p('max_marker_dist'):
                return m
        return None

    def cmd(self, v, w):
        t = Twist()
        t.linear.x = float(v)
        t.angular.z = float(w)
        self.pub_cmd.publish(t)

    # ---------- шаги ----------
    def find_marker(self):
        self.status('1/6 ищу метку')
        if self.fresh_marker(1.0):
            return
        # Корпус 1.06 м, нос в 0.84 м от центра поворота: поворот на месте у стола заденет его углом.
        if self.scan_min is None or self.scan_min < self.p('search_clearance'):
            d = 'нет /scan' if self.scan_min is None else f'препятствие в {self.scan_min:.2f} м'
            raise ParkError(f'метки не видно, а поворачиваться опасно ({d}). '
                            f'Отведите тележку и поставьте носом к торцу стола')
        _, _, y0 = self.robot_pose()
        turned, last = 0.0, y0
        while turned < 2 * math.pi + 0.3:
            self.cmd(0.0, self.p('search_speed'))
            self.sleep(0.1)
            if self.fresh_marker(0.3):
                self.halt()
                self.sleep(1.0)            # остановились — дать детектору свежие кадры
                if self.fresh_marker(1.0):
                    return
            _, _, y = self.robot_pose()
            turned += abs(wrap(y - last))
            last = y
        self.halt()
        raise ParkError('метка не найдена (повернулся на 360°)')

    def measure_table(self):
        """Метка → (центр торца xy, ось стола u) в map."""
        self.status('2/6 измеряю метку')
        n = int(self.p('samples'))
        pts, axes, seen = [], [], set()
        end = time.monotonic() + 15.0
        while len(pts) < n and time.monotonic() < end:
            self.sleep(0.05)
            m = self.fresh_marker(0.3)
            if m is None or id(m) in seen:
                continue
            seen.add(id(m))
            try:
                t = self.tf.lookup_transform(self.p('map_frame'), m.header.frame_id, rclpy.time.Time())
            except Exception as e:      # noqa: BLE001
                self.get_logger().warn(f'TF: {e}')
                continue
            qt = t.transform.rotation
            qt = (qt.x, qt.y, qt.z, qt.w)
            tt = t.transform.translation
            p = m.pose.position
            pm = quat_rot(qt, [p.x, p.y, p.z]) + [tt.x, tt.y, tt.z]
            qm = m.pose.orientation
            z_cam = quat_rot((qm.x, qm.y, qm.z, qm.w), [0, 0, 1])     # нормаль метки в кадре камеры
            z_map = quat_rot(qt, z_cam)
            cam = np.array([tt.x, tt.y, tt.z])
            u = -z_map[:2]
            to_marker = (pm - cam)[:2]
            if u.dot(to_marker) < 0:        # нормаль может смотреть в любую сторону — ось «от камеры»
                u = -u
            u /= np.linalg.norm(u)
            pts.append(pm[:2])
            axes.append(u)
        if len(pts) < max(3, n // 2):
            raise ParkError(f'мало замеров метки ({len(pts)})')
        c = np.mean(pts, axis=0)
        u = np.mean(axes, axis=0)
        u /= np.linalg.norm(u)
        spread = max(np.linalg.norm(p - c) for p in pts)
        self.status(f'2/6 стол: торец ({c[0]:.3f}, {c[1]:.3f}), ось {math.degrees(math.atan2(u[1], u[0])):.1f}°, '
                    f'разброс {spread * 1000:.0f} мм, замеров {len(pts)}')
        return c, u

    def targets(self, c, u):
        side = 1 if int(self.p('side')) >= 0 else -1
        v = np.array([-u[1], u[0]])             # влево от оси стола
        h = side * u                             # курс тележки: тогда её правый борт смотрит на кромку
        off = self.p('table_width') / 2 + self.p('gap') + HALF_WIDTH
        park = c + u * self.p('along') + v * side * off - h * ARM_X
        pre = park - h * self.p('approach')
        yaw = math.atan2(h[1], h[0])
        return park, pre, yaw

    def nav_to(self, xy, yaw):
        self.status(f'4/6 Nav2: еду к точке подхода ({xy[0]:.2f}, {xy[1]:.2f}, {math.degrees(yaw):.0f}°)')
        if not self.nav.wait_for_server(timeout_sec=10.0):
            raise ParkError('Nav2 /navigate_to_pose недоступен')
        g = NavigateToPose.Goal()
        g.pose.header.frame_id = self.p('map_frame')
        g.pose.pose.position.x, g.pose.pose.position.y = float(xy[0]), float(xy[1])
        g.pose.pose.orientation.z, g.pose.pose.orientation.w = math.sin(yaw / 2), math.cos(yaw / 2)
        h = self.wait_future(self.nav.send_goal_async(g), 10.0)
        if not h.accepted:
            raise ParkError('Nav2 отклонил цель')
        self.nav_handle = h
        try:
            r = self.wait_future(h.get_result_async(), self.p('nav_timeout'))
        finally:
            self.nav_handle = None
        if r.status != 4:           # STATUS_SUCCEEDED
            raise ParkError(f'Nav2 не доехал (статус {r.status}, код {r.result.error_code})')

    def rotate_to(self, yaw):
        end = time.monotonic() + 30
        while time.monotonic() < end:
            _, _, y = self.robot_pose()
            e = wrap(yaw - y)
            if abs(e) < 0.01:
                break
            w = max(-self.p('turn_speed'), min(self.p('turn_speed'), 1.5 * e))
            if abs(w) < 0.08:
                w = math.copysign(0.08, e)
            self.cmd(0.0, w)
            self.sleep(0.05)
        self.halt()

    def creep(self, park, yaw):
        """Прямо вдоль линии (park, yaw) до точки park, держа боковое отклонение и курс."""
        h = np.array([math.cos(yaw), math.sin(yaw)])
        n = np.array([-h[1], h[0]])
        self.status('5/6 доводка: выравниваюсь')
        self.rotate_to(yaw)
        self.status('5/6 доводка: еду вдоль кромки')
        end = time.monotonic() + 60
        while time.monotonic() < end:
            x, y, th = self.robot_pose()
            d = np.array([x, y]) - park
            rest = -d.dot(h)            # сколько осталось вдоль линии
            lat = d.dot(n)              # + — левее линии
            if rest < 0.005:
                break
            v = min(self.p('creep_speed'), max(0.02, rest))
            w = -self.p('k_lat') * lat + self.p('k_head') * wrap(yaw - th)
            w = max(-self.p('turn_speed'), min(self.p('turn_speed'), w))
            self.cmd(v, w)
            self.sleep(0.05)
        else:
            self.halt()
            raise ParkError('доводка не закончилась за 60 с')
        self.halt()
        self.sleep(0.5)
        self.rotate_to(yaw)

    def run(self):
        try:
            self.find_marker()
            c, u = self.measure_table()
            park, pre, yaw = self.targets(c, u)
            t = PoseStamped()
            t.header.frame_id = self.p('map_frame')
            t.pose.position.x, t.pose.position.y = float(park[0]), float(park[1])
            t.pose.orientation.z, t.pose.orientation.w = math.sin(yaw / 2), math.cos(yaw / 2)
            self.pub_target.publish(t)
            self.status(f'3/6 стоянка ({park[0]:.3f}, {park[1]:.3f}, {math.degrees(yaw):.1f}°), '
                        f'подход ({pre[0]:.2f}, {pre[1]:.2f})')
            self.nav_to(pre, yaw)
            self.creep(park, yaw)
            x, y, th = self.robot_pose()
            h = np.array([math.cos(yaw), math.sin(yaw)])
            d = np.array([x, y]) - park
            e_along, e_lat, e_yaw = d.dot(h), d.dot([-h[1], h[0]]), wrap(th - yaw)
            ok = abs(e_along) <= self.p('tol_xy') and abs(e_lat) <= self.p('tol_xy') and abs(e_yaw) <= self.p('tol_yaw')
            self.status(f'6/6 {"стою" if ok else "стою НЕТОЧНО"}: вдоль {e_along * 1000:+.0f} мм, '
                        f'вбок {e_lat * 1000:+.0f} мм, курс {math.degrees(e_yaw):+.1f}° (по карте)')
        except Cancelled:
            self.halt()
            self.status('отменено')
        except Exception as e:      # noqa: BLE001
            self.halt()
            self.status(f'ошибка: {e}')


def main():
    rclpy.init()
    node = TableParking()
    ex = SingleThreadedExecutor()
    ex.add_node(node)
    try:
        ex.spin()
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.cancel.set()
        try:
            node.halt()
        except Exception:       # noqa: BLE001
            pass


if __name__ == '__main__':
    main()
