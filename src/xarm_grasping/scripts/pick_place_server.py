#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
pick_place_server — взять объект присоской и поставить в зону (MoveIt + вакуум).

action /pick_place (xarm_grasping_msgs/action/PickPlace): object_id из /objects,
zone из place_zones.yaml (пусто — по правилам для класса объекта).

Камера гриппера стоит выше кончика присоски (~7 см) и сбоку (~6 см) и видит
только дальше 25 см: у объекта она стол не видит. Поэтому распознавание — из
ПОЗЫ НАБЛЮДЕНИЯ (observe_pose: присоска вниз, высоко над столом), туда же рука
возвращается после установки и при ошибке.

Шаги (feedback.stage):
  detect        — свежее распознавание (/detect_objects, update_scene=true):
                  всегда из позы наблюдения; актуальная поза объекта и сцена
                  для MoveIt;
  plan_pregrasp — MoveIt (/move_action, OMPL, с объездом препятствий) в позу
                  над объектом: присоска вниз, approach_height над верхом;
  approach      — прямо вниз (/compute_cartesian_path + /execute_trajectory) до
                  grasp_gap над верхней гранью;
  grip          — нет касания (/vacuum_gripper/ready) — опускаемся шагами
                  probe_step до probe_depth (как присоска с датчиком давления:
                  верх объекта известен с точностью ~1 см); затем вакуум;
                  scene_manager прикрепляет объект к присоске в Planning Scene;
  lift          — прямо вверх на approach_height;
  transfer      — место в зоне (/find_place_pose), MoveIt над место;
  lower         — прямо вниз до place_drop над столом (низ объекта);
  release       — вакуум выкл, объект встаёт на стол;
  retreat       — прямо вверх, в позу наблюдения, распознавание (обновить сцену).

Интерфейсы move_group стандартные (moveit_py в Kilted нет). Группа xarm6,
кончик — link_tcp (кончик присоски). Камера гриппера в Planning Scene —
от scene_manager.

Параметры
  group xarm6  tcp_frame link_tcp  base_frame link_base
  approach_height 0.10  grasp_gap 0.005  place_drop 0.006 (м)
  velocity_scaling 0.3  accel_scaling 0.3  planning_time 5.0
  observe_pose [0.30, 0.0, 0.40] (x y z присоски в base_frame; камера ~0.47 над столом,
               видны обе зоны: из 0.30 зоны были на краю кадра и распознавались частично)
  probe_step 0.003  probe_depth 0.025
"""
import math
import time
import traceback

import rclpy
from geometry_msgs.msg import Pose, PoseStamped
from moveit_msgs.action import ExecuteTrajectory, MoveGroup
from moveit_msgs.msg import (Constraints, MoveItErrorCodes, OrientationConstraint,
                             PositionConstraint)
from moveit_msgs.srv import GetCartesianPath
from rclpy.action import ActionClient, ActionServer, CancelResponse
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from shape_msgs.msg import SolidPrimitive
from std_msgs.msg import Bool
from std_srvs.srv import SetBool
from tf2_ros import Buffer, TransformListener

from xarm_grasping import perception as P
from xarm_grasping_msgs.action import DetectObjects, FindPlacePose, PickPlace
from xarm_grasping_msgs.msg import DetectedObject


class StepFailed(Exception):
    pass


def wait(fut, timeout):
    """Ждать future из другого потока MultiThreadedExecutor."""
    t0 = time.monotonic()
    while not fut.done():
        if time.monotonic() - t0 > timeout:
            raise StepFailed(f'таймаут {timeout:.0f} с')
        time.sleep(0.01)
    return fut.result()


class PickPlaceServer(Node):
    def __init__(self):
        super().__init__('pick_place_server')
        dp = self.declare_parameter
        dp('group', 'xarm6')
        dp('tcp_frame', 'link_tcp')
        dp('base_frame', 'link_base')
        dp('approach_height', 0.10)
        dp('grasp_gap', 0.005)
        dp('place_drop', 0.006)
        dp('velocity_scaling', 0.3)
        dp('accel_scaling', 0.3)
        dp('planning_time', 5.0)
        dp('observe_pose', [0.30, 0.0, 0.40])
        dp('probe_step', 0.003)
        dp('probe_depth', 0.025)

        cbg = ReentrantCallbackGroup()
        self.move = ActionClient(self, MoveGroup, '/move_action', callback_group=cbg)
        self.execute = ActionClient(self, ExecuteTrajectory, '/execute_trajectory',
                                    callback_group=cbg)
        self.cartesian = self.create_client(GetCartesianPath, '/compute_cartesian_path',
                                            callback_group=cbg)
        self.detect = ActionClient(self, DetectObjects, '/detect_objects', callback_group=cbg)
        self.find_place = ActionClient(self, FindPlacePose, '/find_place_pose',
                                       callback_group=cbg)
        self.vacuum = self.create_client(SetBool, '/vacuum_gripper/set', callback_group=cbg)
        self.ready = False
        self.create_subscription(Bool, '/vacuum_gripper/ready',
                                 lambda m: setattr(self, 'ready', m.data), 10,
                                 callback_group=cbg)
        self.goal = None
        self.moved = False
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)
        ActionServer(self, PickPlace, '/pick_place', self.on_goal, callback_group=cbg,
                     cancel_callback=lambda _: CancelResponse.ACCEPT)
        self.get_logger().info('pick_place_server готов: action /pick_place')

    def p(self, name):
        return self.get_parameter(name).value

    # ------------------------------------------------------------ шаги
    def stage(self, stage, detail=''):
        self.get_logger().info(f'[{stage}] {detail}')
        if self.goal is not None:
            self.goal.publish_feedback(PickPlace.Feedback(stage=stage, detail=detail))
            if self.goal.is_cancel_requested:
                raise StepFailed('отменено')

    def call_action(self, client, goal, timeout, what):
        if not client.wait_for_server(timeout_sec=5.0):
            raise StepFailed(f'{what}: нет сервера action')
        gh = wait(client.send_goal_async(goal), 10.0)
        if not gh.accepted:
            raise StepFailed(f'{what}: цель отклонена')
        return wait(gh.get_result_async(), timeout).result

    def detect_objects(self):
        r = self.call_action(self.detect, DetectObjects.Goal(update_scene=True), 15.0,
                             'распознавание')
        if not r.success:
            raise StepFailed(f'распознавание: {r.message}')
        time.sleep(0.5)            # scene_manager применяет сцену по /objects
        return {o.id: o for o in r.objects.objects}

    def tcp_goal(self, pose: Pose):
        """Constraints: link_tcp в точку (сфера 2 мм) с ориентацией (±0.05 рад)."""
        hdr_frame = self.p('base_frame')
        pc = PositionConstraint()
        pc.header.frame_id = hdr_frame
        pc.link_name = self.p('tcp_frame')
        pc.constraint_region.primitives = [SolidPrimitive(type=SolidPrimitive.SPHERE,
                                                          dimensions=[0.002])]
        center = Pose()
        center.position = pose.position
        center.orientation.w = 1.0
        pc.constraint_region.primitive_poses = [center]
        pc.weight = 1.0
        oc = OrientationConstraint()
        oc.header.frame_id = hdr_frame
        oc.link_name = self.p('tcp_frame')
        oc.orientation = pose.orientation
        oc.absolute_x_axis_tolerance = oc.absolute_y_axis_tolerance = 0.05
        oc.absolute_z_axis_tolerance = 0.05
        oc.weight = 1.0
        return Constraints(position_constraints=[pc], orientation_constraints=[oc])

    def plan_and_move(self, pose: Pose, what):
        g = MoveGroup.Goal()
        r = g.request
        r.group_name = self.p('group')
        r.goal_constraints = [self.tcp_goal(pose)]
        r.num_planning_attempts = 5
        r.allowed_planning_time = float(self.p('planning_time'))
        r.max_velocity_scaling_factor = float(self.p('velocity_scaling'))
        r.max_acceleration_scaling_factor = float(self.p('accel_scaling'))
        r.start_state.is_diff = True
        g.planning_options.plan_only = False
        g.planning_options.replan = True
        g.planning_options.replan_attempts = 2
        g.planning_options.planning_scene_diff.is_diff = True
        g.planning_options.planning_scene_diff.robot_state.is_diff = True
        res = self.call_action(self.move, g, 60.0, what)
        if res.error_code.val != MoveItErrorCodes.SUCCESS:
            raise StepFailed(f'{what}: MoveIt код {res.error_code.val}')

    def straight(self, pose: Pose, what, avoid_collisions=True):
        """Прямолинейное движение link_tcp в pose (декартов путь)."""
        if not self.cartesian.wait_for_service(timeout_sec=5.0):
            raise StepFailed(f'{what}: нет /compute_cartesian_path')
        req = GetCartesianPath.Request()
        req.header.frame_id = self.p('base_frame')
        req.start_state.is_diff = True
        req.group_name = self.p('group')
        req.link_name = self.p('tcp_frame')
        req.waypoints = [pose]
        req.max_step = 0.005
        req.avoid_collisions = avoid_collisions
        req.max_velocity_scaling_factor = float(self.p('velocity_scaling'))
        req.max_acceleration_scaling_factor = float(self.p('accel_scaling'))
        res = wait(self.cartesian.call_async(req), 15.0)
        if res.fraction < 0.98:
            raise StepFailed(f'{what}: прямой путь пройден на {res.fraction:.0%}')
        r = self.call_action(self.execute, ExecuteTrajectory.Goal(trajectory=res.solution),
                             60.0, what)
        if r.error_code.val != MoveItErrorCodes.SUCCESS:
            raise StepFailed(f'{what}: исполнение, код {r.error_code.val}')

    def set_vacuum(self, on):
        if not self.vacuum.wait_for_service(timeout_sec=5.0):
            raise StepFailed('нет сервиса /vacuum_gripper/set')
        r = wait(self.vacuum.call_async(SetBool.Request(data=on)), 10.0)
        if not r.success:
            raise StepFailed(f'вакуум: {r.message}')
        return r.message

    def go_observe(self):
        x, y, z = self.p('observe_pose')
        self.stage('observe', 'поза наблюдения (камера высоко над столом)')
        self.plan_and_move(self.pose_at(x, y, z, P.grasp_quat(0.0)), 'поза наблюдения')

    def detect_from_view(self):
        # Только из позы наблюдения: из других поз камера (над присоской, видит
        # дальше 25 см) видит стол под углом и частично — объекты пропадают или
        # получают не тот класс (проверено: из стартовой позы 3 из 8 неверно).
        self.go_observe()
        return self.detect_objects()

    def recover(self):
        """После ошибки: вверх и в позу наблюдения (объект, если держим, остаётся
        в присоске). Всё по возможности, ошибки только в лог."""
        for what, fn in (('вверх', self.lift_up), ('поза наблюдения', self.go_observe)):
            try:
                fn()
            except StepFailed as e:
                self.get_logger().warning(f'восстановление ({what}): {e}')

    def lift_up(self):
        t = self.tf_buffer.lookup_transform(self.p('base_frame'), self.p('tcp_frame'),
                                            rclpy.time.Time()).transform
        p = Pose()
        p.position.x, p.position.y = t.translation.x, t.translation.y
        p.position.z = t.translation.z + float(self.p('approach_height'))
        p.orientation = t.rotation
        self.straight(p, 'вверх', avoid_collisions=False)

    @staticmethod
    def pose_at(x, y, z, quat):
        p = Pose()
        p.position.x, p.position.y, p.position.z = float(x), float(y), float(z)
        (p.orientation.x, p.orientation.y, p.orientation.z, p.orientation.w) = quat
        return p

    # ------------------------------------------------------------ цикл
    def on_goal(self, goal_handle):
        self.goal = goal_handle
        req = goal_handle.request
        result = PickPlace.Result()
        try:
            result.place_pose = self.pick_place(req.object_id, req.zone)
            result.success, result.message = True, f'{req.object_id} поставлен'
            goal_handle.succeed()
        except StepFailed as e:
            result.success, result.message = False, str(e)
            self.get_logger().error(f'pick&place {req.object_id}: {e}')
            if not goal_handle.is_cancel_requested and self.moved:
                self.goal = None          # восстановление без feedback/отмены
                self.recover()
            if goal_handle.is_cancel_requested:
                goal_handle.canceled()
            else:
                goal_handle.abort()
        except Exception:  # noqa: BLE001
            result.success, result.message = False, traceback.format_exc()
            self.get_logger().error(result.message)
            goal_handle.abort()
        finally:
            self.goal = None
        return result

    def pick_place(self, object_id, zone):
        up = float(self.p('approach_height'))
        gap = float(self.p('grasp_gap'))

        self.moved = False
        self.stage('detect', 'свежее распознавание')
        objs = self.detect_from_view()
        for _ in range(2):                    # разовая ошибка распознавания — ещё кадр
            if object_id in objs:
                break
            self.stage('detect', f'{object_id} не виден — ещё раз')
            objs = self.detect_objects()
        if object_id not in objs:
            raise StepFailed(f'объект {object_id} не найден (есть: {", ".join(objs) or "ничего"})')
        o: DetectedObject = objs[object_id]
        gp = o.grasp_pose
        q = (gp.orientation.x, gp.orientation.y, gp.orientation.z, gp.orientation.w)
        top = gp.position.z
        h = o.dimensions[2] if o.shape == DetectedObject.SHAPE_BOX else o.dimensions[0]

        # место ищем ДО захвата — если его нет, не берём объект зря
        self.stage('transfer', 'ищу место в зоне')
        fp = self.call_action(self.find_place,
                              FindPlacePose.Goal(object_id=object_id, zone=zone), 10.0,
                              'поиск места')
        if not fp.success:
            raise StepFailed(f'место: {fp.message}')
        place = fp.place_pose.pose.position
        self.get_logger().info(fp.message)

        self.moved = True
        self.stage('plan_pregrasp', f'над {object_id}')
        self.plan_and_move(self.pose_at(gp.position.x, gp.position.y, top + up, q),
                           'подход над объектом')
        self.stage('approach', f'вниз до {gap * 1000:.0f} мм над верхом')
        self.straight(self.pose_at(gp.position.x, gp.position.y, top + gap, q), 'спуск')

        self.stage('grip', 'жду касания и включаю вакуум')
        time.sleep(0.3)           # /vacuum_gripper/ready обновляется 10 Гц
        z = top + gap
        while not self.ready and z > top + gap - float(self.p('probe_depth')):
            z -= float(self.p('probe_step'))
            self.straight(self.pose_at(gp.position.x, gp.position.y, z, q),
                          'спуск до касания', avoid_collisions=False)
            time.sleep(0.3)
        msg = self.set_vacuum(True)
        self.stage('grip', msg)
        time.sleep(0.5)           # scene_manager прикрепляет объект в Planning Scene

        self.stage('lift', 'вверх')
        self.straight(self.pose_at(gp.position.x, gp.position.y, z + up, q), 'подъём')

        # присоска над местом: верх объекта = низ (стол + place_drop) + h + зазор захвата
        table_z = place.z - h / 2
        # z - top: фактический зазор присоска—верх при захвате (после спуска до касания)
        tcp_place_z = table_z + float(self.p('place_drop')) + h + (z - top)
        po = fp.place_pose.pose.orientation  # поворот места (коробка вдоль/поперёк зоны);
        place_yaw = 2 * math.atan2(po.z, po.w)  # объект в присоске повёрнут как TCP
        q_place = P.grasp_quat(place_yaw)
        self.stage('transfer', f'к зоне {fp.zone}')
        self.plan_and_move(self.pose_at(place.x, place.y, tcp_place_z + up, q_place),
                           'перенос')
        self.stage('lower', 'вниз к столу')
        self.straight(self.pose_at(place.x, place.y, tcp_place_z, q_place), 'опускание')

        self.stage('release', self.set_vacuum(False))
        time.sleep(0.5)
        self.stage('retreat', 'вверх')
        self.straight(self.pose_at(place.x, place.y, tcp_place_z + up, q_place), 'отход')
        try:
            self.go_observe()
            self.detect_objects()
        except StepFailed as e:
            self.get_logger().warning(f'после установки не распознал сцену: {e}')
        out = PoseStamped()
        out.header.frame_id = self.p('base_frame')
        out.pose = fp.place_pose.pose
        return out


def main():
    rclpy.init()
    node = PickPlaceServer()
    ex = MultiThreadedExecutor(num_threads=6)
    ex.add_node(node)
    try:
        while rclpy.ok():
            try:
                ex.spin()
            except (KeyboardInterrupt, ExternalShutdownException):
                break
            except Exception:  # noqa: BLE001
                node.get_logger().error('исключение, продолжаю:\n' + traceback.format_exc())
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
