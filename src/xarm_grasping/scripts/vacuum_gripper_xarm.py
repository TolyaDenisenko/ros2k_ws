#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
vacuum_gripper_xarm — вакуумный гриппер РЕАЛЬНОЙ руки xArm с тем же ROS-интерфейсом,
что у симуляции (vacuum_gripper_sim.py). pick_place_server работает с обоими одинаково.

Зачем: драйвер xarm_api даёт свои сервисы (/xarm/set_vacuum_gripper, /xarm/get_vacuum_gripper),
а наш код захвата говорит на /vacuum_gripper/*. Эта нода — переходник между ними плюс защита:
вакуум не остаётся включённым без толку.

Датчик
  У гриппера UFACTORY только ЦИФРОВОЙ сигнал давления: /xarm/get_vacuum_gripper (GetInt16),
  data = -1 вакуум выключен, 0 включён, предмета нет (давление «низкое»), 1 предмет взят.
  Величины давления нет, порог задан железом. Касание до включения вакуума проверить нечем,
  поэтому топик /vacuum_gripper/ready здесь не публикуется (у симуляции он есть).

Интерфейс (как у симуляции)
  сервис  /vacuum_gripper/set (std_srvs/SetBool)
          data=true  — включить вакуум и ждать подтверждения «предмет взят» до grip_timeout
                       секунд; нет подтверждения — вакуум выключается, success=false;
          data=false — выключить.
  топик   /vacuum_gripper/cmd (std_msgs/Bool) — включить/выключить БЕЗ ожидания (для спуска
          с включённым вакуумом); защита по idle_timeout ниже работает и здесь.
  топик   /vacuum_gripper/state (std_msgs/Bool, transient_local) — true: предмет удерживается
          (датчик давления = 1). Источник правды «взятие было или нет».
  топик   /vacuum_gripper/held (std_msgs/String, transient_local) — 'object' или ''.
  топик   /vacuum_gripper/status (std_msgs/String) — состояние текстом, ~2 Гц.

Защита (вакуум включён, но предмета нет)
  Драйвер xArm сам вакуум НЕ отключает: ни по таймауту, ни при пропаже предмета.
  Поэтому здесь: если вакуум включён дольше idle_timeout секунд, а предмет так и не взят —
  выключить (насос не работает вхолостую). Выключение при выходе из ноды и при ошибке связи
  с драйвером дольше comm_timeout секунд.
  Потеря предмета при переносе (state: true -> false) сама вакуум не выключает — это решает
  pick_place_server, который видит падение state и останавливает работу.

Параметры
  ns /xarm — пространство имён драйвера          hardware_version 1 — 1 Plug-in, 2 Contact
  grip_timeout 3.0   — сколько ждать подтверждения в /vacuum_gripper/set
  idle_timeout 8.0   — вакуум включён без предмета дольше этого — выключить
  comm_timeout 2.0   — нет ответа датчика дольше этого при включённом вакууме — выключить
  poll_hz 10.0       — частота опроса датчика

Проверка без руки: tools/fake_xarm_vacuum.py (подменяет сервисы драйвера).
"""
import signal
import time

import rclpy
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import ExternalShutdownException, MultiThreadedExecutor
from rclpy.node import Node
from rclpy.signals import SignalHandlerOptions
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from std_msgs.msg import Bool, String
from std_srvs.srv import SetBool
from xarm_msgs.srv import GetInt16, VacuumGripperCtrl


class VacuumGripperXarm(Node):
    def __init__(self):
        super().__init__('vacuum_gripper_xarm')
        dp = self.declare_parameter
        dp('ns', '/xarm')
        dp('hardware_version', 1)
        dp('grip_timeout', 3.0)
        dp('idle_timeout', 8.0)
        dp('comm_timeout', 2.0)
        dp('poll_hz', 10.0)
        ns = str(self.get_parameter('ns').value).rstrip('/')
        cbg = ReentrantCallbackGroup()
        self.cli_set = self.create_client(VacuumGripperCtrl, f'{ns}/set_vacuum_gripper',
                                          callback_group=cbg)
        self.cli_get = self.create_client(GetInt16, f'{ns}/get_vacuum_gripper',
                                          callback_group=cbg)
        latched = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL,
                             reliability=ReliabilityPolicy.RELIABLE)
        self.pub_state = self.create_publisher(Bool, '/vacuum_gripper/state', latched)
        self.pub_held = self.create_publisher(String, '/vacuum_gripper/held', latched)
        self.pub_status = self.create_publisher(String, '/vacuum_gripper/status', 10)
        self.create_service(SetBool, '/vacuum_gripper/set', self.on_set, callback_group=cbg)
        self.create_subscription(Bool, '/vacuum_gripper/cmd', self.on_cmd, 10,
                                 callback_group=cbg)
        self.level = None          # последнее data датчика: -1 / 0 / 1 (None — ответа не было)
        self.last_ok = time.monotonic()
        self.on_since = None       # когда вакуум включили (None — выключен по нашей команде)
        self.held = None
        self._publish_state(False)
        self.create_timer(1.0 / float(self.get_parameter('poll_hz').value), self.poll,
                          callback_group=cbg)
        self.create_timer(0.5, self.status, callback_group=cbg)
        self.get_logger().info(
            f'готов: /vacuum_gripper/{{set,cmd,state}} <-> {ns}/{{set,get}}_vacuum_gripper, '
            f'hardware_version={self.get_parameter("hardware_version").value}')

    # ------------------------------------------------------------ вызовы драйвера
    def _call(self, client, req, timeout=2.0):
        """Синхронный вызов (спим: ответ обрабатывает другой поток MultiThreadedExecutor)."""
        if not client.wait_for_service(timeout_sec=1.0):
            return None
        fut = client.call_async(req)
        t_end = time.monotonic() + timeout
        while not fut.done() and time.monotonic() < t_end:
            time.sleep(0.01)
        return fut.result() if fut.done() else None

    def drv_set(self, on):
        req = VacuumGripperCtrl.Request()
        req.on = bool(on)
        req.wait = False
        req.hardware_version = int(self.get_parameter('hardware_version').value)
        r = self._call(self.cli_set, req)
        return r is not None and r.ret == 0, (r.message if r else 'нет ответа драйвера')

    def drv_get(self):
        req = GetInt16.Request()
        r = self._call(self.cli_get, req)
        if r is None or r.ret != 0:
            return None
        return int(r.data)

    # ------------------------------------------------------------ состояние
    def _publish_state(self, held):
        if held == self.held:
            return
        self.held = held
        self.pub_state.publish(Bool(data=bool(held)))
        self.pub_held.publish(String(data='object' if held else ''))
        self.get_logger().info('предмет ВЗЯТ (давление в норме)' if held
                               else 'предмета нет в присоске')

    def poll(self):
        level = self.drv_get()
        now = time.monotonic()
        if level is None:
            # связи с драйвером нет: предмет считаем не удерживаемым, вакуум выключаем
            if self.on_since is not None and now - self.last_ok > float(
                    self.get_parameter('comm_timeout').value):
                self.get_logger().error('нет ответа датчика при включённом вакууме — выключаю')
                self.off('нет связи с драйвером')
            self._publish_state(False)
            self.level = None
            return
        self.last_ok = now
        self.level = level
        self._publish_state(level == 1)
        idle = float(self.get_parameter('idle_timeout').value)
        if self.on_since is not None and level != 1 and now - self.on_since > idle:
            self.get_logger().warning(f'вакуум включён {idle:.0f} с, предмета нет — выключаю')
            self.off('предмета нет дольше idle_timeout')

    def status(self):
        names = {None: 'нет данных', -1: 'выключен', 0: 'включён, предмета нет',
                 1: 'включён, предмет взят'}
        self.pub_status.publish(String(data=f'вакуум: {names.get(self.level, self.level)}'))

    # ------------------------------------------------------------ команды
    def on(self, wait_held):
        ok, msg = self.drv_set(True)
        if not ok:
            return False, f'драйвер не включил вакуум: {msg}'
        self.on_since = time.monotonic()
        if not wait_held:
            return True, 'вакуум включён'
        t_end = time.monotonic() + float(self.get_parameter('grip_timeout').value)
        while time.monotonic() < t_end:
            if self.level == 1:
                return True, 'предмет взят (датчик давления)'
            time.sleep(0.05)
        self.off('нет подтверждения захвата')
        return False, 'предмет не взят: давление не набралось за grip_timeout, вакуум выключен'

    def off(self, why=''):
        self.on_since = None
        ok, msg = self.drv_set(False)
        if not ok:
            self.get_logger().error(f'не удалось выключить вакуум ({why}): {msg}')
        return ok, 'вакуум выключен' if ok else f'выключить не удалось: {msg}'

    def _log(self, ok, text):
        # rclpy не разрешает менять уровень лога в одной и той же строке кода
        if ok:
            self.get_logger().info(text)
        else:
            self.get_logger().warning(text)

    def on_set(self, req, res):
        res.success, res.message = self.on(True) if req.data else self.off('команда')
        self._log(res.success, res.message)
        return res

    def on_cmd(self, msg):
        ok, text = self.on(False) if msg.data else self.off('команда cmd')
        self._log(ok, text)


def main():
    # свой обработчик сигналов: иначе rclpy закрывает контекст раньше, чем мы успеем
    # выключить вакуум при Ctrl+C / kill
    rclpy.init(signal_handler_options=SignalHandlerOptions.NO)
    signal.signal(signal.SIGTERM, lambda *_: (_ for _ in ()).throw(KeyboardInterrupt()))
    node = VacuumGripperXarm()
    ex = MultiThreadedExecutor(num_threads=4)
    ex.add_node(node)
    try:
        ex.spin()
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        try:
            # насос не должен остаться включённым. Исполнитель уже остановлен, поэтому ответ
            # драйвера принимаем сами (spin_until_future_complete), а не ждём другой поток
            req = VacuumGripperCtrl.Request()
            req.on = False
            req.hardware_version = int(node.get_parameter('hardware_version').value)
            if node.cli_set.wait_for_service(timeout_sec=1.0):
                fut = node.cli_set.call_async(req)
                ex.spin_until_future_complete(fut, timeout_sec=2.0)
                node.get_logger().info('выход: вакуум выключен' if fut.done()
                                       else 'выход: драйвер не ответил на выключение')
        except Exception:  # noqa: BLE001
            pass
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
