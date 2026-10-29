#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
nav_bringup_watchdog — страж запуска SLAM и Nav2: если менеджер жизненного цикла не довёл узлы до
active (гонка при старте: «Action server follow_path not available after waiting for 30.00s»,
slam_toolbox остаётся inactive и т. п.), делает то же, что руками: сброс (RESET) и запуск (STARTUP).

Для каждого менеджера из `managers`:
  1. через `initial_delay` с старта проверяет /<менеджер>/is_active (std_srvs/Trigger);
  2. если не active и так же при повторной проверке через `confirm_delay` (не мешаем штатному
     запуску, который ещё идёт), вызывает /<менеджер>/manage_nodes: RESET, затем STARTUP;
  3. повторяет до `max_retries` раз с паузой `retry_period`.
Работает на системном времени (не зависит от /clock) и без таймеров/потоков: простой цикл.

Параметры: managers [lifecycle_manager_slam, lifecycle_manager_navigation], initial_delay 55.0,
confirm_delay 12.0, retry_period 25.0, max_retries 4.
"""
import time

import rclpy
from nav2_msgs.srv import ManageLifecycleNodes
from rclpy.node import Node
from std_srvs.srv import Trigger


class Watchdog(Node):
    def __init__(self):
        super().__init__('nav_bringup_watchdog')
        self.managers = list(self.declare_parameter(
            'managers', ['lifecycle_manager_slam', 'lifecycle_manager_navigation']).value)
        self.initial_delay = float(self.declare_parameter('initial_delay', 55.0).value)
        self.confirm_delay = float(self.declare_parameter('confirm_delay', 12.0).value)
        self.retry_period = float(self.declare_parameter('retry_period', 25.0).value)
        self.max_retries = int(self.declare_parameter('max_retries', 4).value)

    def wait(self, seconds):
        end = time.monotonic() + seconds
        while time.monotonic() < end and rclpy.ok():
            rclpy.spin_once(self, timeout_sec=0.2)

    def call(self, client, request, timeout):
        if not client.wait_for_service(timeout_sec=3.0):
            return None
        fut = client.call_async(request)
        rclpy.spin_until_future_complete(self, fut, timeout_sec=timeout)
        return fut.result() if fut.done() else None

    def is_active(self, manager):
        cli = self.create_client(Trigger, f'/{manager}/is_active')
        res = self.call(cli, Trigger.Request(), 5.0)
        self.destroy_client(cli)
        return bool(res and res.success)

    def restart(self, manager):
        cli = self.create_client(ManageLifecycleNodes, f'/{manager}/manage_nodes')
        for name, cmd in (('RESET', ManageLifecycleNodes.Request.RESET),
                          ('STARTUP', ManageLifecycleNodes.Request.STARTUP)):
            req = ManageLifecycleNodes.Request()
            req.command = cmd
            res = self.call(cli, req, 90.0)
            self.get_logger().warning(f'{manager}: {name} -> {"ok" if res and res.success else "не выполнен"}')
        self.destroy_client(cli)

    def run(self):
        self.get_logger().info(f'жду {self.initial_delay:.0f} с, затем проверяю: {", ".join(self.managers)}')
        self.wait(self.initial_delay)
        retries = {m: 0 for m in self.managers}
        pending = list(self.managers)
        while pending and rclpy.ok():
            for m in list(pending):
                if self.is_active(m):
                    self.get_logger().info(f'{m}: active')
                    pending.remove(m)
                    continue
                self.wait(self.confirm_delay)           # возможно, штатный запуск просто ещё идёт
                if self.is_active(m):
                    self.get_logger().info(f'{m}: active')
                    pending.remove(m)
                    continue
                if retries[m] >= self.max_retries:
                    self.get_logger().error(f'{m}: не поднялся после {self.max_retries} попыток — сдаюсь')
                    pending.remove(m)
                    continue
                retries[m] += 1
                self.get_logger().warning(f'{m}: не active — перезапуск ({retries[m]}/{self.max_retries})')
                self.restart(m)
            if pending:
                self.wait(self.retry_period)
        self.get_logger().info('страж закончил: всё поднято (или попытки исчерпаны)')


def main():
    rclpy.init()
    node = Watchdog()
    try:
        node.run()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
