#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
check_survey_accuracy — насколько точно система знает, где предметы: вид сверху (/objects) против
осмотра с разных сторон (/objects_refined, survey_objects.py). Только симуляция «рука на столе».

Правда — позы моделей в Gazebo (`gz model -m <имя> -p`) и размеры из config/arm_table_objects.yaml.
link_base стоит на столе: x, y те же, что в мире, z = мир − 0.75.

  # симуляция arm_table_gz (use_moveit:=true detector:=true), затем:
  ros2 service call /survey_objects std_srvs/srv/Trigger
  GZ_PARTITION=<как у симуляции> python3 check_survey_accuracy.py

Печатает по каждому предмету: ошибку центра (мм), ошибку высоты верха (мм — точка хвата присоски),
ошибку размеров (мм) — для вида сверху и после осмотра, и итог (средние).
"""
import argparse
import math
import os
import re
import subprocess
import time

import numpy as np
import rclpy
import yaml
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy

from xarm_grasping_msgs.msg import DetectedObject, DetectedObjectArray

HERE = os.path.dirname(os.path.abspath(__file__))
TABLE_TOP_WORLD_Z = 0.75


def gz_pose(model):
    out = subprocess.run(['gz', 'model', '-m', model, '-p'], capture_output=True, text=True, timeout=20).stdout
    nums = re.findall(r'\[([-\d.e ]+)\]', out.split('Pose')[-1])
    if len(nums) < 2:
        return None
    xyz = [float(v) for v in nums[0].split()]
    rpy = [float(v) for v in nums[1].split()]
    return xyz[0], xyz[1], xyz[2] - TABLE_TOP_WORLD_Z, rpy[2]


def dims_sorted(o):
    """Размеры для сравнения: коробка — (длинная, короткая, высота), цилиндр — (высота, радиус)."""
    d = list(o.dimensions)
    if o.shape == DetectedObject.SHAPE_CYLINDER:
        return ('cyl', d[0], d[1])
    return ('box', max(d[0], d[1]), min(d[0], d[1]), d[2])


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--objects', default=os.path.join(HERE, '..', 'config', 'arm_table_objects.yaml'))
    ap.add_argument('--match', type=float, default=0.06, help='предмет — модель не дальше, м')
    a = ap.parse_args()
    truth = []
    for m in yaml.safe_load(open(a.objects))['objects']:
        p = gz_pose(m['name'])
        if p is None:
            continue
        if m['shape'] == 'cylinder':
            r, h = m['size']
            td = ('cyl', h, r)
            top = h
        else:
            sx, sy, sz = m['size']
            td = ('box', max(sx, sy), min(sx, sy), sz)
            top = sz
        truth.append(dict(name=m['name'], x=p[0], y=p[1], top=top, d=td))     # стоит на столе: верх = высота
    rclpy.init()
    n = Node('check_survey_accuracy')
    lat = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL, reliability=ReliabilityPolicy.RELIABLE)
    got = {}
    n.create_subscription(DetectedObjectArray, '/objects', lambda m: got.__setitem__('top', m), lat)
    n.create_subscription(DetectedObjectArray, '/objects_refined', lambda m: got.__setitem__('ref', m), lat)
    end = time.time() + 15
    while time.time() < end and len(got) < 2:
        rclpy.spin_once(n, timeout_sec=0.2)
    if 'top' not in got:
        raise SystemExit('нет /objects')
    if 'ref' not in got:
        print('нет /objects_refined — осмотр не запускался; показываю только вид сверху')

    def errors(o):
        best = min(truth, key=lambda t: math.hypot(t['x'] - o.pose.position.x, t['y'] - o.pose.position.y))
        dc = math.hypot(best['x'] - o.pose.position.x, best['y'] - o.pose.position.y)
        if dc > a.match:
            return None
        dt = abs(o.grasp_pose.position.z - best['top'])
        od = dims_sorted(o)
        dd = max(abs(x - y) for x, y in zip(od[1:], best['d'][1:])) if od[0] == best['d'][0] else float('nan')
        return best['name'], dc * 1000, dt * 1000, dd * 1000, od[0] == best['d'][0]

    rows = {}
    for key in ('top', 'ref'):
        if key in got:
            for o in got[key].objects:
                e = errors(o)
                if e:
                    rows.setdefault(e[0], {})[key] = e[1:]
    print(f'{"предмет":12s} | {"центр, мм":>17s} | {"верх, мм":>15s} | {"размеры, мм":>15s} | форма')
    print(f'{"":12s} | {"сверху":>8s} {"осмотр":>8s} | {"сверху":>7s} {"осмотр":>7s} | {"сверху":>7s} {"осмотр":>7s} |')
    tot = {k: [] for k in ('top', 'ref')}
    for name in sorted(rows):
        r = rows[name]

        def f(k, i):
            return f'{r[k][i]:7.1f}' if k in r and not math.isnan(r[k][i]) else '      —'
        shape = ' '.join(('верно' if r[k][3] else 'НЕВЕРНО') for k in ('top', 'ref') if k in r)
        print(f'{name:12s} | {f("top", 0)} {f("ref", 0):>8s} | {f("top", 1)} {f("ref", 1)} | '
              f'{f("top", 2)} {f("ref", 2)} | {shape}')
        for k in tot:
            if k in r:
                tot[k].append(r[k])
    for k, title in (('top', 'сверху'), ('ref', 'после осмотра')):
        if tot[k]:
            A = np.array([x[:3] for x in tot[k]], float)
            print(f'среднее ({title}): центр {np.nanmean(A[:, 0]):.1f} мм, верх {np.nanmean(A[:, 1]):.1f} мм, '
                  f'размеры {np.nanmean(A[:, 2]):.1f} мм; макс. центр {np.nanmax(A[:, 0]):.1f} мм')
    rclpy.shutdown()


if __name__ == '__main__':
    main()
