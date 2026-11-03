# -*- coding: utf-8 -*-
"""
detector — детектор объектов на OpenVINO (без ROS): кадр BGR -> рамки с классом.

Нужен, чтобы по картинке узнавать, ЧТО лежит на столе; где объект и какой он формы, говорит облако
точек (perception.py). Связка «рамка <-> кластер облака» — в object_perception (см.
docs/R2_OBJECT_RECOGNITION.md, docs/RECOGNITION_MODELS.md).

Движок — OpenVINO Runtime: на ПК разработки процессор, на Intel NUC — CPU | GPU (Iris Xe/Arc) | NPU,
выбирается параметром `device` (AUTO — решает сам OpenVINO). Модель — ONNX или OpenVINO IR (.xml).
Сейчас разбирается выход семейства YOLOX (Apache-2.0, без декодирования в сети); другие семейства —
добавлением функции в DECODERS.

Проверка без ROS:
  python -m xarm_grasping.detector --model ~/.local/share/quasi/models/yolox/yolox_s.onnx \
      --image ~/.local/share/quasi/test_images/000000039769.jpg --device CPU --out /tmp/out.jpg
"""
import argparse
import json
import os
import time
from dataclasses import dataclass

import cv2
import numpy as np

COCO80 = [
    'person', 'bicycle', 'car', 'motorcycle', 'airplane', 'bus', 'train', 'truck', 'boat',
    'traffic light', 'fire hydrant', 'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog',
    'horse', 'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe', 'backpack', 'umbrella',
    'handbag', 'tie', 'suitcase', 'frisbee', 'skis', 'snowboard', 'sports ball', 'kite',
    'baseball bat', 'baseball glove', 'skateboard', 'surfboard', 'tennis racket', 'bottle',
    'wine glass', 'cup', 'fork', 'knife', 'spoon', 'bowl', 'banana', 'apple', 'sandwich', 'orange',
    'broccoli', 'carrot', 'hot dog', 'pizza', 'donut', 'cake', 'chair', 'couch', 'potted plant',
    'bed', 'dining table', 'toilet', 'tv', 'laptop', 'mouse', 'remote', 'keyboard', 'cell phone',
    'microwave', 'oven', 'toaster', 'sink', 'refrigerator', 'book', 'clock', 'vase', 'scissors',
    'teddy bear', 'hair drier', 'toothbrush']


@dataclass
class Detection:
    label_id: int
    label: str
    score: float
    x1: float   # рамка в пикселях ИСХОДНОГО кадра
    y1: float
    x2: float
    y2: float

    @property
    def cx(self):
        return 0.5 * (self.x1 + self.x2)

    @property
    def cy(self):
        return 0.5 * (self.y1 + self.y2)


def load_labels(spec):
    """Имена классов: None -> COCO80; путь к файлу (по строке на класс) или список."""
    if spec is None or spec == '':
        return list(COCO80)
    if isinstance(spec, (list, tuple)):
        return list(spec)
    with open(os.path.expanduser(spec), encoding='utf-8') as f:
        return [ln.strip() for ln in f if ln.strip()]


def letterbox(img, size):
    """Вписывает кадр в size=(H, W) без искажения пропорций, поле — серое 114 (как в YOLOX)."""
    h, w = img.shape[:2]
    r = min(size[0] / h, size[1] / w)
    nh, nw = int(round(h * r)), int(round(w * r))
    out = np.full((size[0], size[1], 3), 114, dtype=np.uint8)
    out[:nh, :nw] = cv2.resize(img, (nw, nh), interpolation=cv2.INTER_LINEAR)
    return out, r


def pre_yolox(bgr, size):
    """Кадр -> вход сети YOLOX: поля 114 вместо искажения, BGR 0..255. ctx — масштаб."""
    img, ratio = letterbox(bgr, size)
    return np.ascontiguousarray(img.transpose(2, 0, 1)[None].astype(np.float32)), ratio


def pre_ssd(bgr, size):
    """Кадр -> вход SSD: растяжение до размера сети (как в torchvision), BGR 0..255. Нормировка (в RGB,
    среднее и разброс 0.5) уже внутри модели. ctx — масштабы по осям (исходный/сеть)."""
    h, w = bgr.shape[:2]
    img = cv2.resize(bgr, (size[1], size[0]), interpolation=cv2.INTER_LINEAR)
    return (np.ascontiguousarray(img.transpose(2, 0, 1)[None].astype(np.float32)),
            (w / size[1], h / size[0]))


def decode_ssd(outs, size, conf, nms_thr, ctx, extra):
    """Выход SSD (torchvision SSDlite): смещения рамок и логиты классов относительно якорей.
    outs = [bbox_regression (1, N, 4), cls_logits (1, N, C+1)], extra['anchors'] — (N, 4) xyxy в пикселях
    сети. Возвращает [x1, y1, x2, y2, score, класс] в пикселях исходного кадра, класс с нуля (без фона)."""
    rel = np.asarray(outs[0], dtype=np.float32).reshape(-1, 4)
    logits = np.asarray(outs[1], dtype=np.float32).reshape(rel.shape[0], -1)
    anchors = extra['anchors']
    e = np.exp(logits - logits.max(1, keepdims=True))
    prob = (e / e.sum(1, keepdims=True))[:, 1:]                       # без класса «фон»
    cid = prob.argmax(1)
    score = prob[np.arange(len(prob)), cid]
    keep = score >= conf
    if not keep.any():
        return np.zeros((0, 6), np.float32)
    rel, anc, cid, score = rel[keep], anchors[keep], cid[keep], score[keep]
    wa, ha = anc[:, 2] - anc[:, 0], anc[:, 3] - anc[:, 1]
    xa, ya = anc[:, 0] + 0.5 * wa, anc[:, 1] + 0.5 * ha
    dx, dy = rel[:, 0] / 10.0, rel[:, 1] / 10.0                       # веса BoxCoder SSD: 10, 10, 5, 5
    dw, dh = np.minimum(rel[:, 2] / 5.0, 4.135), np.minimum(rel[:, 3] / 5.0, 4.135)
    xc, yc, bw, bh = dx * wa + xa, dy * ha + ya, np.exp(dw) * wa, np.exp(dh) * ha
    sx, sy = ctx
    boxes = np.stack(((xc - 0.5 * bw) * sx, (yc - 0.5 * bh) * sy, (xc + 0.5 * bw) * sx, (yc + 0.5 * bh) * sy), 1)
    off = cid[:, None] * 100000.0
    xywh = np.concatenate((boxes[:, :2] + off, boxes[:, 2:] - boxes[:, :2]), 1)
    idx = np.array(cv2.dnn.NMSBoxes(xywh.tolist(), score.tolist(), conf, nms_thr)).reshape(-1)
    return np.concatenate((boxes[idx], score[idx, None], cid[idx, None]), 1).astype(np.float32)


def decode_yolox(raw, size, conf, nms_thr, ratio):
    """Выход YOLOX без декодирования: (1, N, 5+C) -> массив [x1, y1, x2, y2, score, класс]."""
    out = np.asarray(raw, dtype=np.float32)
    out = out.reshape(-1, out.shape[-1])
    grids, strides = [], []
    for s in (8, 16, 32):
        gh, gw = size[0] // s, size[1] // s
        xv, yv = np.meshgrid(np.arange(gw), np.arange(gh))
        grids.append(np.stack((xv, yv), 2).reshape(-1, 2))
        strides.append(np.full((gh * gw, 1), s))
    grids, strides = np.concatenate(grids), np.concatenate(strides)
    if len(grids) != len(out):
        raise ValueError(f'число ячеек {len(grids)} не совпадает с выходом сети {len(out)}: '
                         f'проверьте размер входа {size} и тип модели')
    xy = (out[:, :2] + grids) * strides
    wh = np.exp(out[:, 2:4]) * strides
    cls = out[:, 5:]
    cid = cls.argmax(1)
    score = out[:, 4] * cls[np.arange(len(cls)), cid]
    keep = score >= conf
    if not keep.any():
        return np.zeros((0, 6), np.float32)
    xy, wh, cid, score = xy[keep], wh[keep], cid[keep], score[keep]
    boxes = np.concatenate((xy - wh / 2, xy + wh / 2), 1) / ratio
    # NMS по классам: смещаем рамки разных классов, чтобы они не подавляли друг друга
    off = cid[:, None] * 10000.0
    xywh = np.concatenate((boxes[:, :2] + off, boxes[:, 2:] - boxes[:, :2]), 1)
    idx = cv2.dnn.NMSBoxes(xywh.tolist(), score.tolist(), conf, nms_thr)
    idx = np.array(idx).reshape(-1)
    return np.concatenate((boxes[idx], score[idx, None], cid[idx, None]), 1).astype(np.float32)


DECODERS = {'yolox': lambda outs, size, conf, nms, ctx, extra: decode_yolox(outs[0], size, conf, nms, ctx),
            'ssd': decode_ssd}
PREPROCESS = {'yolox': pre_yolox, 'ssd': pre_ssd}


def load_meta(model_path):
    """Файл-спутник модели <имя>.meta.json: тип, классы, якоря. Нет файла — считаем, что это YOLOX/COCO."""
    stem = os.path.splitext(os.path.expanduser(model_path))[0]
    p = stem + '.meta.json'
    if not os.path.exists(p):
        return None
    with open(p, encoding='utf-8') as f:
        meta = json.load(f)
    meta['_dir'] = os.path.dirname(p)
    return meta


class OpenVinoDetector:
    def __init__(self, model_path, device='AUTO', conf=0.4, nms=0.45, labels=None,
                 model_type='yolox', input_size=None, performance_hint='LATENCY', num_threads=0):
        import openvino as ov
        self.conf, self.nms = float(conf), float(nms)
        meta = load_meta(model_path)
        self.extra = {}
        if meta:                                   # модель со спутником: тип, классы и якоря берём из него
            model_type = meta['type']
            if not labels:
                labels = meta.get('classes')
            if meta.get('anchors'):
                self.extra['anchors'] = np.load(os.path.join(meta['_dir'], meta['anchors'])).astype(np.float32)
        self.labels = load_labels(labels)
        if model_type not in DECODERS:
            raise ValueError(f'тип модели {model_type!r} не поддерживается: {sorted(DECODERS)}')
        self.decode, self.preprocess, self.model_type = DECODERS[model_type], PREPROCESS[model_type], model_type
        core = ov.Core()
        model = core.read_model(os.path.expanduser(model_path))
        shape = model.input(0).partial_shape
        if input_size:                       # размер входа можно переопределить (динамические модели)
            model.reshape([1, 3, int(input_size[0]), int(input_size[1])])
            shape = model.input(0).partial_shape
        self.size = (int(shape[2].get_length()), int(shape[3].get_length()))
        self.device = device
        cfg = {'PERFORMANCE_HINT': performance_hint}
        if int(num_threads) > 0:      # ограничить потоки: по умолчанию OpenVINO берёт все ядра процессора
            cfg['INFERENCE_NUM_THREADS'] = int(num_threads)
        self.compiled = core.compile_model(model, device, cfg)
        self.req = self.compiled.create_infer_request()
        self.timing = {}

    def detect(self, bgr):
        """Кадр BGR (H, W, 3, uint8) -> список Detection (рамки в пикселях кадра)."""
        t0 = time.perf_counter()
        blob, ctx = self.preprocess(bgr, self.size)
        t1 = time.perf_counter()
        res = self.req.infer({0: blob})
        outs = [res[o] for o in self.compiled.outputs]
        t2 = time.perf_counter()
        dets = self.decode(outs, self.size, self.conf, self.nms, ctx, self.extra)
        h, w = bgr.shape[:2]
        out = []
        for x1, y1, x2, y2, sc, c in dets:
            c = int(c)
            out.append(Detection(c, self.labels[c] if c < len(self.labels) else str(c), float(sc),
                                 float(np.clip(x1, 0, w)), float(np.clip(y1, 0, h)),
                                 float(np.clip(x2, 0, w)), float(np.clip(y2, 0, h))))
        t3 = time.perf_counter()
        self.timing = {'pre_ms': (t1 - t0) * 1e3, 'infer_ms': (t2 - t1) * 1e3,
                       'post_ms': (t3 - t2) * 1e3, 'total_ms': (t3 - t0) * 1e3}
        return out


def draw(bgr, dets):
    img = bgr.copy()
    for d in dets:
        p1, p2 = (int(d.x1), int(d.y1)), (int(d.x2), int(d.y2))
        cv2.rectangle(img, p1, p2, (0, 200, 0), 2)
        txt = f'{d.label} {d.score:.2f}'
        cv2.putText(img, txt, (p1[0], max(12, p1[1] - 4)), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                    (0, 200, 0), 1, cv2.LINE_AA)
    return img


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--model', required=True)
    ap.add_argument('--image', required=True, nargs='+')
    ap.add_argument('--device', default='CPU')
    ap.add_argument('--conf', type=float, default=0.4)
    ap.add_argument('--out', default='', help='куда сохранить кадр с рамками (для одной картинки)')
    ap.add_argument('--bench', type=int, default=30, help='число повторов для замера времени')
    ap.add_argument('--threads', type=int, default=0, help='потоков вывода на CPU (0 — все ядра)')
    a = ap.parse_args()
    t = time.perf_counter()
    det = OpenVinoDetector(a.model, a.device, a.conf, num_threads=a.threads)
    print(f'модель загружена за {time.perf_counter() - t:.2f} с, вход {det.size}, устройство {a.device}')
    for path in a.image:
        img = cv2.imread(os.path.expanduser(path))
        dets = det.detect(img)
        print(f'{os.path.basename(path)} ({img.shape[1]}x{img.shape[0]}): {len(dets)} объектов')
        for d in sorted(dets, key=lambda d: -d.score):
            print(f'   {d.label:14s} {d.score:.2f}  [{d.x1:.0f},{d.y1:.0f} .. {d.x2:.0f},{d.y2:.0f}]')
        if a.out and len(a.image) == 1:
            cv2.imwrite(a.out, draw(img, dets))
    ts = []
    for _ in range(a.bench):
        det.detect(img)
        ts.append(det.timing['total_ms'])
    print(f'время кадра: среднее {np.mean(ts):.1f} мс, p95 {np.percentile(ts, 95):.1f} мс '
          f'({1000 / np.mean(ts):.0f} кадр/с); сеть {det.timing["infer_ms"]:.1f} мс')


if __name__ == '__main__':
    main()
