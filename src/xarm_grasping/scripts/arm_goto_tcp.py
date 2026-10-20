#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
arm_goto_tcp — отладочная утилита: подвести TCP (кончик присоски) к точке.

Нужна для ПРОВЕРКИ симуляции без MoveIt (камера, вакуум): позволяет одной
командой поставить присоску на объект и потом включить вакуум сервисом
/vacuum_gripper/set. Это не планировщик: траектория — прямая интерполяция в
пространстве суставов контроллера xarm6_traj_controller, без проверки
столкновений. Для рабочих захватов используйте MoveIt (docs/09).

Как работает
  1. Берёт URDF из топика /robot_description, собирает цепочку
     link_base -> link_tcp (суставы joint1..joint6 + фиксированные звенья).
  2. Численно решает обратную кинематику (scipy least_squares): позиция TCP в
     системе link_base + ось инструмента (+Z TCP) направлена вниз (-Z link_base);
     поворот вокруг оси (yaw) свободен.
  3. Отправляет траекторию в /xarm6_traj_controller/follow_joint_trajectory:
     сначала точка на --lift метров выше цели, затем сама цель.

Координаты --x --y --z — в системе link_base (база руки), метры. Для сцены
cart_table_offline.world при spawn 0,0:
    link_base в мире = (-0.0075, 0, 0.919); верх стола z_base = +0.006;
    pick_box (0.10x0.05x0.05) по центру (0.6075, 0, 0.031), верх z_base = +0.056.
Пример: поставить присоску на верх коробки:
    ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.058
"""
import argparse
import sys
import xml.etree.ElementTree as ET

import numpy as np
import rclpy
from builtin_interfaces.msg import Duration
from control_msgs.action import FollowJointTrajectory
from rclpy.action import ActionClient
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile
from scipy.optimize import least_squares
from scipy.spatial.transform import Rotation
from std_msgs.msg import String
from trajectory_msgs.msg import JointTrajectoryPoint

ARM_JOINTS = [f'joint{i}' for i in range(1, 7)]


def rpy_xyz(el):
    """<origin xyz rpy> -> матрица 4x4 (RPY: R = Rz·Ry·Rx, как в URDF)."""
    o = el.find('origin')
    xyz = [float(v) for v in (o.get('xyz', '0 0 0') if o is not None else '0 0 0').split()]
    rpy = [float(v) for v in (o.get('rpy', '0 0 0') if o is not None else '0 0 0').split()]
    T = np.eye(4)
    T[:3, :3] = Rotation.from_euler('xyz', rpy).as_matrix()  # 'xyz' (строчные) = вращения вокруг ФИКСИРОВАННЫХ осей: R = Rz·Ry·Rx, как в URDF
    T[:3, 3] = xyz
    return T


class Chain:
    """Кинематическая цепочка root -> tip из URDF."""

    def __init__(self, urdf_xml, root, tip):
        robot = ET.fromstring(urdf_xml)
        by_child = {j.find('child').get('link'): j for j in robot.findall('joint')}
        seq, link = [], tip
        while link != root:
            j = by_child[link]
            seq.append(j)
            link = j.find('parent').get('link')
        self.joints = list(reversed(seq))
        self.limits = {}
        for j in self.joints:
            if j.get('type') in ('revolute', 'continuous'):
                lim = j.find('limit')
                lo = float(lim.get('lower', -np.pi)) if lim is not None else -np.pi
                hi = float(lim.get('upper', np.pi)) if lim is not None else np.pi
                self.limits[j.get('name')] = (lo, hi)
        self.active = [j for j in self.joints if j.get('name') in ARM_JOINTS]

    def fk(self, q):
        T = np.eye(4)
        qi = iter(q)
        for j in self.joints:
            T = T @ rpy_xyz(j)
            if j.get('name') in ARM_JOINTS:
                ax = np.array([float(v) for v in j.find('axis').get('xyz').split()])
                R = np.eye(4)
                R[:3, :3] = Rotation.from_rotvec(ax * next(qi)).as_matrix()
                T = T @ R
        return T


def solve_ik(chain, target, q0):
    lo = np.array([chain.limits[n][0] for n in ARM_JOINTS])
    hi = np.array([chain.limits[n][1] for n in ARM_JOINTS])
    down = np.array([0.0, 0.0, -1.0])

    def resid(q):
        T = chain.fk(q)
        return np.concatenate([(T[:3, 3] - target) * 10.0,   # позиция (вес 10)
                               (T[:3, 2] - down) * 3.0])      # ось инструмента вниз

    best = None
    # Сначала старт из текущей позы (быстро и даёт ближайшее решение); если не сошлось —
    # повороты основания (другие «ветви» решения).
    for dq1 in (0.0, 0.5, -0.5, 1.0, -1.0):
        s = np.array(q0, float)
        s[0] += dq1
        r = least_squares(resid, np.clip(s, lo + 1e-3, hi - 1e-3), bounds=(lo, hi))
        if best is None or r.cost < best.cost:
            best = r
        if best.cost < 1e-8:
            break
    # Суставы с диапазоном > 2π: берём эквивалентный угол, ближайший к текущему
    # (иначе рука раскручивается на лишний оборот).
    q = best.x.copy()
    for i in range(6):
        for k in (-2 * np.pi, 2 * np.pi):
            if abs(q[i] + k - q0[i]) < abs(q[i] - q0[i]) and lo[i] <= q[i] + k <= hi[i]:
                q[i] += k
    best.x = q
    T = chain.fk(best.x)
    err = np.linalg.norm(T[:3, 3] - target)
    tilt = np.degrees(np.arccos(np.clip(np.dot(T[:3, 2], down), -1, 1)))
    return best.x, err, tilt


class Goto(Node):
    def __init__(self):
        super().__init__('arm_goto_tcp')
        self.urdf = None
        qos = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
        self.create_subscription(String, '/robot_description', self.on_urdf, qos)
        self.client = ActionClient(
            self, FollowJointTrajectory, '/xarm6_traj_controller/follow_joint_trajectory')
        self.q = None
        from sensor_msgs.msg import JointState
        self.create_subscription(JointState, '/joint_states', self.on_js, 10)

    def on_urdf(self, msg):
        self.urdf = msg.data

    def on_js(self, msg):
        d = dict(zip(msg.name, msg.position))
        if all(j in d for j in ARM_JOINTS):
            self.q = [d[j] for j in ARM_JOINTS]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--x', type=float, required=True, help='TCP x в link_base, м')
    ap.add_argument('--y', type=float, required=True, help='TCP y в link_base, м')
    ap.add_argument('--z', type=float, required=True, help='TCP z в link_base, м')
    ap.add_argument('--lift', type=float, default=0.10,
                    help='подход сверху: сначала точка на столько выше, м (0 — сразу в цель)')
    ap.add_argument('--time', type=float, default=6.0, help='время на каждый участок, с')
    ap.add_argument('--dry-run', action='store_true', help='только посчитать IK')
    args, ros_args = ap.parse_known_args()

    rclpy.init(args=ros_args)
    node = Goto()
    node.get_logger().info('жду /robot_description и /joint_states ...')
    while rclpy.ok() and (node.urdf is None or node.q is None):
        rclpy.spin_once(node, timeout_sec=0.2)

    chain = Chain(node.urdf, 'link_base', 'link_tcp')
    waypoints = []
    if args.lift > 0:
        waypoints.append(np.array([args.x, args.y, args.z + args.lift]))
    waypoints.append(np.array([args.x, args.y, args.z]))

    q_prev, traj = node.q, []
    for w in waypoints:
        q, err, tilt = solve_ik(chain, w, q_prev)
        node.get_logger().info(
            f'IK цель {np.round(w, 4)}: ошибка позиции {err * 1000:.2f} мм, '
            f'наклон оси инструмента {tilt:.1f}°, q={np.round(q, 3).tolist()}')
        if err > 0.005 or tilt > 10:
            node.get_logger().error('IK не сошлась (точка вне досягаемости?) — отмена')
            return 1
        traj.append(q)
        q_prev = q
    if args.dry_run:
        return 0

    if not node.client.wait_for_server(timeout_sec=10.0):
        node.get_logger().error('нет action-сервера xarm6_traj_controller')
        return 1
    goal = FollowJointTrajectory.Goal()
    goal.trajectory.joint_names = ARM_JOINTS
    t = 0.0
    for q in traj:
        t += args.time
        p = JointTrajectoryPoint()
        p.positions = [float(v) for v in q]
        p.velocities = [0.0] * 6
        p.time_from_start = Duration(sec=int(t), nanosec=int((t % 1) * 1e9))
        goal.trajectory.points.append(p)
    fut = node.client.send_goal_async(goal)
    rclpy.spin_until_future_complete(node, fut)
    handle = fut.result()
    if not handle.accepted:
        node.get_logger().error('цель отклонена контроллером')
        return 1
    res_fut = handle.get_result_async()
    rclpy.spin_until_future_complete(node, res_fut)
    code = res_fut.result().result.error_code
    node.get_logger().info(f'готово, error_code={code} (0 = успех)')
    return 0 if code == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
