#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
merge_datasets — склеивает несколько датасетов в формате COCO (из capture_dataset.py) в один.

Зачем: сбор может оборваться (сбои Gazebo под нагрузкой), тогда его добирают второй порцией с другим
--seed, а потом объединяют. Картинки копируются с новыми номерами, номера классов должны совпадать.

  python3 merge_datasets.py --out ~/.local/share/quasi/training/datasets/sim_shapes_v4 \
      --inputs ~/.local/share/quasi/training/datasets/sim_shapes_v4a ~/.local/share/quasi/training/datasets/sim_shapes_v4b
"""
import argparse
import json
import os
import shutil


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--out', required=True)
    ap.add_argument('--inputs', nargs='+', required=True)
    a = ap.parse_args()
    out = os.path.expanduser(a.out)
    if os.path.exists(os.path.join(out, 'annotations')):
        raise SystemExit(f'{out} уже существует: удалите или выберите другое имя')
    cats = None
    res = {}
    n_img = {'train': 0, 'val': 0}
    for sp in ('train', 'val'):
        os.makedirs(os.path.join(out, 'images', sp), exist_ok=True)
    os.makedirs(os.path.join(out, 'annotations'), exist_ok=True)
    res = {sp: dict(images=[], annotations=[], categories=None) for sp in ('train', 'val')}
    ann_id, iid, meta = 1, 1, []
    for src in a.inputs:
        src = os.path.expanduser(src)
        for sp in ('train', 'val'):
            j = json.load(open(os.path.join(src, 'annotations', f'{sp}.json')))
            if cats is None:
                cats = j['categories']
            elif j['categories'] != cats:
                raise SystemExit(f'классы в {src} не совпадают с первым датасетом')
            res[sp]['categories'] = cats
            by = {}
            for an in j['annotations']:
                by.setdefault(an['image_id'], []).append(an)
            for im in j['images']:
                fn = f'{iid:06d}.jpg'
                shutil.copy(os.path.join(src, 'images', sp, im['file_name']), os.path.join(out, 'images', sp, fn))
                new = dict(im, id=iid, file_name=fn)
                res[sp]['images'].append(new)
                for an in by.get(im['id'], []):
                    res[sp]['annotations'].append(dict(an, id=ann_id, image_id=iid))
                    ann_id += 1
                iid += 1
        if os.path.exists(os.path.join(src, 'meta.json')):
            meta.append(json.load(open(os.path.join(src, 'meta.json'))))
    for sp in ('train', 'val'):
        json.dump(res[sp], open(os.path.join(out, 'annotations', f'{sp}.json'), 'w'))
    names = [c['name'] for c in sorted(cats, key=lambda c: c['id'])]
    open(os.path.join(out, 'class_names.txt'), 'w').write('\n'.join(names) + '\n')
    json.dump(dict(merged_from=[os.path.abspath(os.path.expanduser(s)) for s in a.inputs], sources=meta,
                   train=len(res['train']['images']), val=len(res['val']['images'])),
              open(os.path.join(out, 'meta.json'), 'w'), indent=1)
    shutil.copytree(os.path.join(os.path.expanduser(a.inputs[0]), 'preview'), os.path.join(out, 'preview'),
                    dirs_exist_ok=True) if os.path.exists(os.path.join(os.path.expanduser(a.inputs[0]), 'preview')) else None
    print(f'готово: train {len(res["train"]["images"])}, val {len(res["val"]["images"])}, '
          f'рамок {len(res["train"]["annotations"])} + {len(res["val"]["annotations"])} -> {out}')


if __name__ == '__main__':
    main()
