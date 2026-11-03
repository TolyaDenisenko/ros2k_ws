#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
export_openvino — превращает обученные веса (best.pth) в модели для OpenVINO.

Шаги (простыми словами, подробнее — docs/book/MODEL_TRAINING.md):
  1. Веса PyTorch -> ONNX (универсальный формат файла модели). В модель встроена подготовка картинки:
     детектор подаёт «сырой» кадр BGR 0..255, перекраска и нормировка делаются внутри.
  2. Сохраняются «якоря» (anchors.npy): шаблонные рамки SSD, относительно которых сеть выдаёт ответ.
     Они нужны детектору, чтобы превратить выход сети в настоящие рамки на картинке.
  3. ONNX -> OpenVINO IR (.xml схема + .bin веса) в трёх точностях:
       fp32 — исходная точность;  fp16 — вдвое легче, качество почти то же;
       int8 — квантование (NNCF): ещё легче и быстрее, нужна калибровка по картинкам без разметки.
  4. Для каждой модели пишется файл-спутник <имя>.meta.json: тип, классы, якоря, размер входа.

Запуск (окружение обучения):
  ~/.venvs/yolox/bin/python export_openvino.py \
      --weights ~/.local/share/quasi/training/runs/ssdlite_v1/best.pth \
      --data ~/.local/share/quasi/training/datasets/sim_tabletop_v1 \
      --out ~/.local/share/quasi/models/sim_tabletop_v1
"""
import argparse
import glob
import json
import os
import sys

import cv2
import numpy as np
import torch

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import train_ssdlite  # noqa: E402
from train_ssdlite import build_model  # noqa: E402


class Deployed(torch.nn.Module):
    """Что реально считается на тележке: подготовка кадра + сеть. Выход — «сырые» ответы по якорям."""

    def __init__(self, model):
        super().__init__()
        self.backbone, self.head = model.backbone, model.head

    def forward(self, x):                       # x: [1, 3, 320, 320], BGR, 0..255
        x = x[:, [2, 1, 0]]                     # BGR -> RGB
        x = (x / 255.0 - 0.5) / 0.5             # как внутри torchvision SSDlite (среднее и разброс 0.5)
        out = self.head(list(self.backbone(x).values()))
        return out['bbox_regression'], out['cls_logits']


def anchors_of(model, SIZE):
    from torchvision.models.detection.image_list import ImageList
    x = torch.zeros(1, 3, SIZE, SIZE)
    feats = list(model.backbone(x).values())
    return model.anchor_generator(ImageList(x, [(SIZE, SIZE)]), feats)[0].detach().numpy().astype(np.float32)


def calib_images(data, n):
    files = sorted(glob.glob(os.path.join(data, 'images', 'train', '*.jpg')))
    step = max(1, len(files) // n)
    return files[::step][:n]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--weights', required=True)
    ap.add_argument('--data', required=True, help='датасет: калибровочные картинки для INT8 берутся из train')
    ap.add_argument('--out', required=True)
    ap.add_argument('--calib', type=int, default=300, help='сколько картинок для калибровки INT8')
    a = ap.parse_args()
    out = os.path.expanduser(a.out)
    os.makedirs(out, exist_ok=True)
    cfg_path = os.path.join(os.path.dirname(os.path.expanduser(a.weights)), 'config.json')
    if os.path.exists(cfg_path):                  # размер входа, с которым обучали
        train_ssdlite.SIZE = json.load(open(cfg_path))['size']
    SIZE = train_ssdlite.SIZE
    classes = [ln.strip() for ln in open(os.path.join(os.path.dirname(os.path.expanduser(a.weights)), 'classes.txt')) if ln.strip()]
    model = build_model(len(classes), pretrained=False)
    model.load_state_dict(torch.load(os.path.expanduser(a.weights), map_location='cpu', weights_only=True))
    model.eval()
    # 1. ONNX
    onnx_path = os.path.join(out, 'model.onnx')
    torch.onnx.export(Deployed(model).eval(), torch.zeros(1, 3, SIZE, SIZE), onnx_path, opset_version=17,
                      input_names=['image'], output_names=['bbox_regression', 'cls_logits'], dynamo=False)
    print('ONNX:', onnx_path, f'{os.path.getsize(onnx_path) / 1e6:.1f} МБ')
    # 2. якоря
    anchors = anchors_of(model, SIZE)
    np.save(os.path.join(out, 'anchors.npy'), anchors)
    print('якорей:', len(anchors))
    # 3. OpenVINO IR
    import openvino as ov
    ovm = ov.convert_model(onnx_path)
    ov.save_model(ovm, os.path.join(out, 'model_fp32.xml'), compress_to_fp16=False)
    ov.save_model(ovm, os.path.join(out, 'model_fp16.xml'), compress_to_fp16=True)
    print('IR fp32 и fp16 сохранены')
    import nncf
    files = calib_images(os.path.expanduser(a.data), a.calib)

    def prep(path):
        img = cv2.resize(cv2.imread(path), (SIZE, SIZE), interpolation=cv2.INTER_LINEAR)
        return np.ascontiguousarray(img.transpose(2, 0, 1)[None].astype(np.float32))
    q = nncf.quantize(ovm, nncf.Dataset(files, prep), subset_size=len(files))
    ov.save_model(q, os.path.join(out, 'model_int8.xml'))
    print(f'IR int8 сохранён (калибровка по {len(files)} картинкам)')
    # 4. спутники
    for name in ('fp32', 'fp16', 'int8'):
        meta = {'type': 'ssd', 'classes': classes, 'anchors': 'anchors.npy', 'input_size': [SIZE, SIZE],
                'precision': name, 'weights': os.path.abspath(os.path.expanduser(a.weights)),
                'note': 'вход: BGR 0..255, растяжение до input_size; нормировка внутри модели'}
        with open(os.path.join(out, f'model_{name}.meta.json'), 'w', encoding='utf-8') as f:
            json.dump(meta, f, ensure_ascii=False, indent=1)
    with open(os.path.join(out, 'classes.txt'), 'w') as f:
        f.write('\n'.join(classes) + '\n')
    print('готово ->', out)


if __name__ == '__main__':
    main()
