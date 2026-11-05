#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
analyze_detector — разбор ошибок детектора на проверочных картинках: что находит, что пропускает,
кого с кем путает и сколько лишних рамок. Дополняет evaluate_detector.py (там один общий mAP).

Печатает:
  1. по порогам уверенности: сколько предметов найдено верно, сколько названо неверно, сколько
     пропущено и сколько лишних рамок;
  2. по классам при выбранном пороге: найдено, средняя уверенность, пропуски;
  3. матрицу ошибок: строка — настоящий предмет, столбец — что сказала модель (или «пропуск»).

Рамка считается найденной, если пересекается с настоящей по IoU не меньше 0.5. Среди рамок над
порогом для предмета берётся самая уверенная.

  ~/.venvs/yolox/bin/python analyze_detector.py \
      --data ~/.local/share/quasi/training/datasets/sim_shapes_v4 \
      --model ~/.local/share/quasi/models/sim_shapes_v4/model_fp16.xml \
      --threshold 0.2 --filter layout=tight

Ключи --filter те же, что у evaluate_detector.py.
"""
import argparse
import importlib.util
import json
import os
import sys

import cv2
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('detector', os.path.join(HERE, '..', 'xarm_grasping', 'detector.py'))
detector = importlib.util.module_from_spec(spec)
sys.modules['detector'] = detector
spec.loader.exec_module(detector)

MISS = 'пропуск'


def iou(a, b):
    w = max(0.0, min(a[2], b[2]) - max(a[0], b[0]))
    h = max(0.0, min(a[3], b[3]) - max(a[1], b[1]))
    i = w * h
    return i / ((a[2] - a[0]) * (a[3] - a[1]) + (b[2] - b[0]) * (b[3] - b[1]) - i + 1e-9)


def analyze(frames, thr):
    """frames: [(gt [(класс, x1,y1,x2,y2)], dets [Detection])]. Возвращает счётчики для порога thr."""
    conf = {}                       # (настоящий, сказанный или MISS) -> число
    scores = {}                     # настоящий -> [уверенность найденной верно рамки]
    extra = 0                       # рамки, не привязанные ни к одному предмету
    for gt, dets in frames:
        dets = [d for d in dets if d.score >= thr]
        used = set()
        for cls, *box in gt:
            best, bi = None, None
            for k, d in enumerate(dets):
                if k in used or iou(box, (d.x1, d.y1, d.x2, d.y2)) < 0.5:
                    continue
                if best is None or d.score > best.score:
                    best, bi = d, k
            if best is None:
                conf[(cls, MISS)] = conf.get((cls, MISS), 0) + 1
            else:
                used.add(bi)
                conf[(cls, best.label)] = conf.get((cls, best.label), 0) + 1
                if best.label == cls:
                    scores.setdefault(cls, []).append(best.score)
        extra += len(dets) - len(used)
    return conf, scores, extra


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--data', required=True)
    ap.add_argument('--model', required=True)
    ap.add_argument('--device', default='CPU')
    ap.add_argument('--threads', type=int, default=4)
    ap.add_argument('--threshold', type=float, default=0.2, help='порог для таблицы по классам и матрицы ошибок')
    ap.add_argument('--filter', action='append', default=[], metavar='КЛЮЧ=ЗНАЧЕНИЕ')
    ap.add_argument('--split', default='val')
    a = ap.parse_args()
    data = os.path.expanduser(a.data)
    j = json.load(open(os.path.join(data, 'annotations', f'{a.split}.json')))
    names = {c['id']: c['name'] for c in j['categories']}
    imgs = j['images']
    for f in a.filter:
        k, v = f.split('=', 1)
        imgs = [i for i in imgs if str(i.get(k)) == v]
    if not imgs:
        raise SystemExit('после фильтра не осталось картинок')
    gts = {}
    for an in j['annotations']:
        x, y, w, h = an['bbox']
        gts.setdefault(an['image_id'], []).append((names[an['category_id']], x, y, x + w, y + h))
    det = detector.OpenVinoDetector(os.path.expanduser(a.model), a.device, 0.05, nms=0.5, num_threads=a.threads)
    frames = [(gts.get(im['id'], []), det.detect(cv2.imread(os.path.join(data, 'images', a.split, im['file_name']))))
              for im in imgs]
    total = sum(len(g) for g, _ in frames)
    print(f'картинок: {len(imgs)}, предметов: {total}, фильтр: {a.filter or "нет"}\n')
    print(f'{"порог":>6s} {"верно":>6s} {"неверное имя":>13s} {"пропуск":>8s} {"лишних рамок":>13s}')
    for t in (0.5, 0.4, 0.3, 0.2, 0.15, 0.1, 0.05):
        conf, _, extra = analyze(frames, t)
        ok = sum(n for (c, p), n in conf.items() if c == p)
        miss = sum(n for (c, p), n in conf.items() if p == MISS)
        wrong = total - ok - miss
        print(f'{t:6.2f} {ok:4d} ({100 * ok / total:3.0f}%) {wrong:8d} {miss:11d} {extra:13d}')
    conf, scores, extra = analyze(frames, a.threshold)
    classes = [names[i] for i in sorted(names)]
    print(f'\nпо классам при пороге {a.threshold}:')
    print(f'{"класс":16s} {"всего":>6s} {"верно":>6s} {"%":>5s} {"ср. уверенность":>16s}')
    for c in classes:
        n = sum(v for (cc, _), v in conf.items() if cc == c)
        ok = conf.get((c, c), 0)
        sc = np.mean(scores[c]) if scores.get(c) else 0.0
        print(f'{c:16s} {n:6d} {ok:6d} {100 * ok / max(n, 1):4.0f}% {sc:16.2f}')
    cols = classes + [MISS]
    print(f'\nматрица ошибок при пороге {a.threshold} (строка — настоящий предмет, столбец — ответ модели):')
    print(f'{"":16s}' + ''.join(f'{c[:11]:>12s}' for c in cols))
    for c in classes:
        print(f'{c:16s}' + ''.join(f'{conf.get((c, p), 0):12d}' for p in cols))
    print(f'\nлишних рамок (без предмета под ними): {extra}')


if __name__ == '__main__':
    main()
