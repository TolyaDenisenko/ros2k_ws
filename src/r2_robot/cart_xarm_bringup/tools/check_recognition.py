#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
check_recognition — проверка распознавания БЕЗ ROS и без симуляции (3 шага).

  1. OpenVINO: версия и доступные устройства.
  2. Сопоставление рамок детектора с кластерами облака на синтетических случаях.
  3. (если заданы --model и --image) детекция на фотографии и время кадра.

Запускать интерпретатором окружения OpenVINO:
  ~/.venvs/openvino/bin/python check_recognition.py
  ~/.venvs/openvino/bin/python check_recognition.py \
      --model ~/.local/share/quasi/models/yolox/yolox_tiny.onnx \
      --image ~/.local/share/quasi/test_images/000000039769.jpg --device CPU --threads 2
Код возврата 0 — всё прошло.
"""
import argparse
import importlib.util
import os
import sys
import time

import numpy as np

SRC = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'xarm_grasping',
                                   'xarm_grasping'))


def load(name):
    spec = importlib.util.spec_from_file_location(name, os.path.join(SRC, name + '.py'))
    mod = importlib.util.module_from_spec(spec)
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


def step1():
    print('== 1. OpenVINO')
    try:
        import openvino as ov
    except ImportError:
        print('   НЕТ openvino. Установка: python3 -m venv --system-site-packages ~/.venvs/openvino && '
              '~/.venvs/openvino/bin/pip install openvino  (и запускать ЕГО python)')
        return False
    core = ov.Core()
    print(f'   версия {ov.__version__}')
    for d in core.available_devices:
        print(f'   устройство {d}: {core.get_property(d, "FULL_DEVICE_NAME")}')
    return True


def step2():
    print('== 2. Сопоставление рамок и кластеров (синтетика)')
    P = load('perception')
    rng = np.random.default_rng(0)

    def blob(cx, cy, w, h, n=200):
        return np.stack((rng.uniform(cx - w / 2, cx + w / 2, n), rng.uniform(cy - h / 2, cy + h / 2, n)), 1)

    A, B = blob(200, 200, 60, 60), blob(300, 200, 60, 60)          # две соседние коробки
    box_a, box_b = (165, 165, 235, 235, 0.9, 'bottle'), (265, 165, 335, 235, 0.8, 'cup')
    table = (0, 0, 640, 480, 0.95, 'dining table')                  # рамка стола накрывает всё
    ok = True

    def check(title, got, expect):
        nonlocal ok
        good = [None if g is None else g[0] for g in got] == expect
        ok &= good
        print(f'   {"ок " if good else "СБОЙ"} {title}: {[None if g is None else g[0] for g in got]}')

    check('две коробки + рамка стола', P.associate_detections([A, B], [table, box_a, box_b]), ['bottle', 'cup'])
    check('рамка только у первой', P.associate_detections([A, B], [box_a]), ['bottle', None])
    check('рамка в стороне', P.associate_detections([A], [(400, 400, 450, 450, 0.9, 'cup')]), [None])
    one = P.associate_detections([A, B], [(150, 150, 350, 250, 0.9, 'tray')])
    good = sum(g is not None for g in one) == 1
    ok &= good
    print(f'   {"ок " if good else "СБОЙ"} одна рамка на две коробки достаётся одной: {[None if g is None else g[0] for g in one]}')
    uv = P.project_points(np.array([[0, 0, 1.0], [0.1, 0, 1.0], [0, 0, -1.0]]), 600, 600, 320, 200)
    good = np.allclose(uv[:2], [[320, 200], [380, 200]]) and np.isnan(uv[2]).all()
    ok &= good
    print(f'   {"ок " if good else "СБОЙ"} проекция точек в кадр')
    return ok


def step3(a):
    print('== 3. Детекция на фотографии')
    import cv2
    D = load('detector')
    t = time.perf_counter()
    det = D.OpenVinoDetector(a.model, a.device, a.conf, num_threads=a.threads)
    print(f'   модель загружена за {time.perf_counter() - t:.2f} с, вход {det.size}, устройство {a.device}')
    img = cv2.imread(os.path.expanduser(a.image))
    if img is None:
        print('   не удалось прочитать картинку')
        return False
    dets = det.detect(img)
    for d in sorted(dets, key=lambda d: -d.score)[:12]:
        print(f'   {d.label:14s} {d.score:.2f}  [{d.x1:.0f},{d.y1:.0f} .. {d.x2:.0f},{d.y2:.0f}]')
    ts = []
    for _ in range(20):
        det.detect(img)
        ts.append(det.timing['total_ms'])
    print(f'   найдено {len(dets)}; время кадра {np.mean(ts):.1f} мс ({1000 / np.mean(ts):.0f} кадр/с)')
    if a.out:
        cv2.imwrite(a.out, D.draw(img, dets))
        print(f'   кадр с рамками: {a.out}')
    return len(dets) > 0


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--model')
    ap.add_argument('--image')
    ap.add_argument('--device', default='CPU')
    ap.add_argument('--threads', type=int, default=2)
    ap.add_argument('--conf', type=float, default=0.4)
    ap.add_argument('--out', default='')
    a = ap.parse_args()
    ok = step1()
    ok = step2() and ok
    if a.model and a.image and ok:
        ok = step3(a) and ok
    print('\nИТОГ:', 'всё прошло' if ok else 'ЕСТЬ ПРОБЛЕМЫ')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
