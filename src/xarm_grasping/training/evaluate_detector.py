#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
evaluate_detector — честная проверка готовых моделей на проверочных картинках (val), которые они
никогда не видели. Проверяется ровно то, что поедет на тележку: файл модели + наш детектор
(xarm_grasping/detector.py), а не PyTorch.

Для каждой модели печатает: mAP (насколько точны рамки и классы, 0..1), mAP@0.5 (мягче: рамка
совпала хотя бы наполовину), время кадра на выбранном устройстве и размер файла.

  ~/.venvs/yolox/bin/python evaluate_detector.py \
      --data ~/.local/share/quasi/training/datasets/sim_tabletop_v1 \
      --models ~/.local/share/quasi/models/sim_tabletop_v1/model_{fp32,fp16,int8}.xml --device CPU --threads 4
"""
import argparse
import importlib.util
import json
import os
import sys
import time

import cv2
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('detector', os.path.join(HERE, '..', 'xarm_grasping', 'detector.py'))
detector = importlib.util.module_from_spec(spec)
sys.modules['detector'] = detector
spec.loader.exec_module(detector)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--data', required=True)
    ap.add_argument('--models', required=True, nargs='+')
    ap.add_argument('--device', default='CPU')
    ap.add_argument('--threads', type=int, default=0)
    ap.add_argument('--conf', type=float, default=0.05, help='низкий порог нужен для честного mAP')
    ap.add_argument('--limit', type=int, default=0, help='проверить только первые N картинок')
    a = ap.parse_args()
    from pycocotools.coco import COCO
    from pycocotools.cocoeval import COCOeval
    data = os.path.expanduser(a.data)
    ann = os.path.join(data, 'annotations', 'val.json')
    gt = COCO(ann)
    with open(ann) as f:
        j = json.load(f)
    cats = sorted(c['id'] for c in j['categories'])
    imgs = j['images'][:a.limit] if a.limit else j['images']
    print(f'проверочных картинок: {len(imgs)}, рамок: {sum(1 for x in j["annotations"] if x["image_id"] in {i["id"] for i in imgs})}')
    print(f'{"модель":24s} {"mAP":>6s} {"mAP@.5":>7s} {"кадр, мс":>9s} {"размер":>8s}')
    for mp in a.models:
        mp = os.path.expanduser(mp)
        det = detector.OpenVinoDetector(mp, a.device, a.conf, nms=0.5, num_threads=a.threads)
        res, ts = [], []
        for im in imgs:
            bgr = cv2.imread(os.path.join(data, 'images', 'val', im['file_name']))
            dets = det.detect(bgr)
            ts.append(det.timing['total_ms'])
            for d in dets:
                res.append({'image_id': im['id'], 'category_id': cats[det.labels.index(d.label)], 'score': d.score,
                            'bbox': [d.x1, d.y1, d.x2 - d.x1, d.y2 - d.y1]})
        if res:
            e = COCOeval(gt, gt.loadRes(res), 'bbox')
            e.params.imgIds = [i['id'] for i in imgs]
            import contextlib
            with open(os.devnull, 'w') as dn, contextlib.redirect_stdout(dn):
                e.evaluate(); e.accumulate(); e.summarize()
            m, m50 = e.stats[0], e.stats[1]
        else:
            m = m50 = 0.0
        size = os.path.getsize(os.path.splitext(mp)[0] + '.bin') / 1e6
        print(f'{os.path.basename(mp):24s} {m:6.3f} {m50:7.3f} {np.mean(ts[2:] or ts):9.1f} {size:6.1f} МБ')


if __name__ == '__main__':
    main()
