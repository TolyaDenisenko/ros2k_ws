#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
train_ssdlite — дообучение детектора SSDlite320 (MobileNetV3) на своём датасете в формате COCO.

Что делает (простыми словами, подробнее — docs/book/MODEL_TRAINING.md):
  1. Берёт модель, уже обученную на 80 бытовых классах COCO («предобученные веса»).
  2. Заменяет у неё «последний слой» под наши классы и доучивает на картинках датасета.
  3. Каждую эпоху проверяет качество на проверочных картинках (mAP) и сохраняет лучшие веса.

Запуск (интерпретатором окружения обучения):
  ~/.venvs/yolox/bin/python train_ssdlite.py \
      --data ~/.local/share/quasi/training/datasets/sim_tabletop_v1 \
      --out ~/.local/share/quasi/training/runs/ssdlite_v1 --epochs 40

Результат в --out: best.pth (лучшие веса), last.pth, log.csv (по эпохам), classes.txt.
Модель — torchvision (BSD-3), веса COCO скачиваются с download.pytorch.org.
"""
import argparse
import csv
import json
import os
import random
import time

import numpy as np
import torch
import torchvision
from PIL import Image
from torchvision import tv_tensors
from torchvision.models.detection import ssdlite320_mobilenet_v3_large
from torchvision.models.detection.ssdlite import SSDLite320_MobileNet_V3_Large_Weights
from torchvision.transforms import v2 as T

ZOOM_P = 0.4
SIZE = 320     # размер входа сети, пикселей (меняется параметром --size; нужен кратный 32)


class CocoDet(torch.utils.data.Dataset):
    """Картинки + рамки из annotations/<split>.json. Метки классов сдвинуты на 1 (0 — «фон»)."""

    def __init__(self, root, split, train):
        self.dir = os.path.join(root, 'images', split)
        with open(os.path.join(root, 'annotations', f'{split}.json')) as f:
            j = json.load(f)
        self.images = j['images']
        self.cats = sorted(c['id'] for c in j['categories'])
        self.names = [c['name'] for c in sorted(j['categories'], key=lambda c: c['id'])]
        self.cat2label = {c: i + 1 for i, c in enumerate(self.cats)}
        self.by_img = {}
        for a in j['annotations']:
            self.by_img.setdefault(a['image_id'], []).append(a)
        if train:   # аугментации: цвет, масштаб/сдвиг, отражение. Рамки преобразуются вместе с картинкой
            self.tf = T.Compose([
                T.RandomPhotometricDistort(p=0.8),
                T.RandomZoomOut(fill=(114, 114, 114), side_range=(1.0, 1.6), p=ZOOM_P),
                T.RandomIoUCrop(),
                T.Resize((SIZE, SIZE), antialias=True),
                T.RandomHorizontalFlip(0.5),
                T.RandomVerticalFlip(0.5),
                T.SanitizeBoundingBoxes(min_size=4),
            ])
        else:
            self.tf = T.Compose([T.Resize((SIZE, SIZE), antialias=True)])

    def __len__(self):
        return len(self.images)

    def __getitem__(self, i):
        im = self.images[i]
        img = Image.open(os.path.join(self.dir, im['file_name'])).convert('RGB')
        anns = self.by_img.get(im['id'], [])
        boxes = [[a['bbox'][0], a['bbox'][1], a['bbox'][0] + a['bbox'][2], a['bbox'][1] + a['bbox'][3]] for a in anns]
        labels = [self.cat2label[a['category_id']] for a in anns]
        tgt = {'boxes': tv_tensors.BoundingBoxes(torch.tensor(boxes, dtype=torch.float32).reshape(-1, 4),
                                                 format='XYXY', canvas_size=(img.height, img.width)),
               'labels': torch.tensor(labels, dtype=torch.int64)}
        img, tgt = self.tf(tv_tensors.Image(torch.from_numpy(np.array(img)).permute(2, 0, 1)), tgt)
        return img.float() / 255.0, {'boxes': tgt['boxes'].as_subclass(torch.Tensor), 'labels': tgt['labels'],
                                     'image_id': im['id'], 'orig': (im['width'], im['height'])}


def collate(b):
    return tuple(zip(*b))


def build_model(num_classes, pretrained=True):
    """SSDlite (MobileNetV3) под num_classes (+ фон), вход SIZE×SIZE. Предобученные веса COCO — всё, кроме
    слоёв классификации. Размер входа больше 320 помогает мелким предметам: их якоря и признаки
    считаются на более крупной картинке (свёртки от размера не зависят, поэтому веса COCO подходят)."""
    model = ssdlite320_mobilenet_v3_large(weights=None, weights_backbone=None, num_classes=num_classes + 1)
    model.transform.min_size, model.transform.max_size = (SIZE,), SIZE
    model.transform.fixed_size = (SIZE, SIZE)
    if pretrained:
        coco = SSDLite320_MobileNet_V3_Large_Weights.COCO_V1.get_state_dict(progress=True)
        own = model.state_dict()
        keep = {k: v for k, v in coco.items() if k in own and own[k].shape == v.shape}
        print(f'предобученные веса: перенесено {len(keep)} из {len(own)} тензоров '
              f'(классификационные слои учатся заново)')
        model.load_state_dict(keep, strict=False)
    return model


@torch.no_grad()
def evaluate(model, loader, ds, device):
    """mAP по COCO (pycocotools) на проверочных картинках; рамки возвращаются в пиксели исходного кадра."""
    from pycocotools.coco import COCO
    from pycocotools.cocoeval import COCOeval
    model.eval()
    res = []
    for imgs, tg in loader:
        out = model([i.to(device) for i in imgs])
        for o, t in zip(out, tg):
            w, h = t['orig']
            b = o['boxes'].cpu().numpy() * np.array([w / SIZE, h / SIZE, w / SIZE, h / SIZE])
            for bb, sc, lb in zip(b, o['scores'].cpu().numpy(), o['labels'].cpu().numpy()):
                res.append({'image_id': t['image_id'], 'category_id': ds.cats[int(lb) - 1], 'score': float(sc),
                            'bbox': [float(bb[0]), float(bb[1]), float(bb[2] - bb[0]), float(bb[3] - bb[1])]})
    if not res:
        return 0.0, 0.0
    gt = COCO(os.path.join(os.path.dirname(os.path.dirname(ds.dir)), 'annotations', 'val.json'))
    dt = gt.loadRes(res)
    e = COCOeval(gt, dt, 'bbox')
    e.evaluate(); e.accumulate(); e.summarize()
    return float(e.stats[0]), float(e.stats[1])     # mAP@[.5:.95], mAP@.5


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--data', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--epochs', type=int, default=40)
    ap.add_argument('--batch', type=int, default=32)
    ap.add_argument('--lr', type=float, default=0.01)
    ap.add_argument('--workers', type=int, default=4)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--no-pretrained', action='store_true', help='учить с нуля (обычно хуже)')
    ap.add_argument('--size', type=int, default=320, help='размер входа сети (кратно 32): 320, 416, 480…')
    ap.add_argument('--zoom-out', type=float, default=0.4, help='вероятность «отдалить» кадр (аугментация)')
    a = ap.parse_args()
    global SIZE, ZOOM_P
    SIZE, ZOOM_P = a.size, a.zoom_out
    random.seed(a.seed); np.random.seed(a.seed); torch.manual_seed(a.seed)
    data, out = os.path.expanduser(a.data), os.path.expanduser(a.out)
    os.makedirs(out, exist_ok=True)
    device = 'cuda' if torch.cuda.is_available() else 'cpu'
    tr, va = CocoDet(data, 'train', True), CocoDet(data, 'val', False)
    print(f'устройство {device}; картинок: обучение {len(tr)}, проверка {len(va)}; классы: {tr.names}')
    with open(os.path.join(out, 'classes.txt'), 'w') as f:
        f.write('\n'.join(tr.names) + '\n')
    with open(os.path.join(out, 'config.json'), 'w') as f:
        json.dump({'size': SIZE, 'epochs': a.epochs, 'batch': a.batch, 'lr': a.lr, 'seed': a.seed, 'zoom_out': ZOOM_P}, f)
    ltr = torch.utils.data.DataLoader(tr, a.batch, shuffle=True, num_workers=a.workers, collate_fn=collate, drop_last=True)
    lva = torch.utils.data.DataLoader(va, 16, shuffle=False, num_workers=2, collate_fn=collate)
    model = build_model(len(tr.names), not a.no_pretrained).to(device)
    params = [p for p in model.parameters() if p.requires_grad]
    opt = torch.optim.SGD(params, lr=a.lr, momentum=0.9, weight_decay=4e-5)
    steps = a.epochs * len(ltr)
    sched = torch.optim.lr_scheduler.OneCycleLR(opt, max_lr=a.lr, total_steps=steps, pct_start=0.1)
    best, log = -1.0, open(os.path.join(out, 'log.csv'), 'w', newline='')
    wr = csv.writer(log); wr.writerow(['epoch', 'loss', 'map', 'map50', 'sec'])
    for ep in range(1, a.epochs + 1):
        model.train(); t0 = time.time(); tot = 0.0
        for imgs, tg in ltr:
            imgs = [i.to(device) for i in imgs]
            tgs = [{'boxes': t['boxes'].to(device), 'labels': t['labels'].to(device)} for t in tg]
            loss = sum(model(imgs, tgs).values())
            opt.zero_grad(); loss.backward(); torch.nn.utils.clip_grad_norm_(params, 10.0); opt.step(); sched.step()
            tot += float(loss)
        m, m50 = evaluate(model, lva, va, device) if (ep % 2 == 0 or ep == a.epochs) else (float('nan'), float('nan'))
        print(f'эпоха {ep}/{a.epochs}: потери {tot / len(ltr):.3f}, mAP {m:.3f}, mAP@0.5 {m50:.3f}, {time.time() - t0:.0f} с', flush=True)
        wr.writerow([ep, round(tot / len(ltr), 4), m, m50, round(time.time() - t0)]); log.flush()
        torch.save(model.state_dict(), os.path.join(out, 'last.pth'))
        if m == m and m > best:
            best = m
            torch.save(model.state_dict(), os.path.join(out, 'best.pth'))
    print(f'готово. лучший mAP {best:.3f}; веса: {out}/best.pth')


if __name__ == '__main__':
    main()
