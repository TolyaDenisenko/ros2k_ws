#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_aruco_marker — метка ArUco для парковки тележки у стола (сцена worlds/cart_nav.world).

Делает две вещи:
  1. картинку метки worlds/materials/aruco_<словарь>_<id>.png — сама метка и белая рамка
     («тихая зона»: без неё детектор метку не находит);
  2. модель `aruco_dock` в worlds/cart_nav.world (между маркерами <!-- ARUCO BEGIN/END -->):
     тонкая белая пластина с картинкой на торце стола, обращённая к тележке.

    python3 src/r2_robot/cart_xarm_bringup/tools/gen_aruco_marker.py              # по умолчанию
    python3 .../gen_aruco_marker.py --id 3 --marker 0.16 --x 0.444 --z 0.80

Размер МЕТКИ (--marker, чёрный квадрат с узором, без белой рамки) — это и есть marker_size
детектора aruco_detector.py: если поменяли здесь, поменяйте там.
После запуска: colcon build --packages-select cart_xarm_bringup (новый файл картинки).
"""
import argparse
import os

import cv2
import numpy as np

PKG = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BEGIN, END = '    <!-- ARUCO BEGIN', '    <!-- ARUCO END -->'


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1])
    ap.add_argument('--dict', default='DICT_4X4_50')
    ap.add_argument('--id', type=int, default=0)
    ap.add_argument('--marker', type=float, default=0.16, help='сторона метки без белой рамки, м')
    ap.add_argument('--border', type=float, default=0.02, help='белая рамка с каждой стороны, м')
    ap.add_argument('--x', type=float, default=0.444, help='x плоскости метки в мире (торец стола 0.45)')
    ap.add_argument('--y', type=float, default=0.0)
    ap.add_argument('--z', type=float, default=0.80, help='центр метки над полом (верх стола 0.925)')
    ap.add_argument('--px', type=int, default=400, help='пикселей на сторону метки в картинке')
    ap.add_argument('--world', default=os.path.join(PKG, 'worlds', 'cart_nav.world'))
    a = ap.parse_args()

    d = cv2.aruco.getPredefinedDictionary(getattr(cv2.aruco, a.dict))
    gen = getattr(cv2.aruco, 'generateImageMarker', None) or cv2.aruco.drawMarker  # OpenCV ≥4.7 / 4.6
    mark = gen(d, a.id, a.px)
    b = int(round(a.px * a.border / a.marker))
    img = np.full((a.px + 2 * b, a.px + 2 * b), 255, np.uint8)
    img[b:b + a.px, b:b + a.px] = mark
    name = f'aruco_{a.dict.replace("DICT_", "").lower()}_{a.id}.png'
    os.makedirs(os.path.join(PKG, 'worlds', 'materials'), exist_ok=True)
    cv2.imwrite(os.path.join(PKG, 'worlds', 'materials', name), img)
    side = a.marker + 2 * a.border

    # Пластина-коробка: тонкая по x, лицевая грань смотрит в -x (на тележку). Картинка — на
    # отдельной плоскости чуть впереди грани (у plane понятная развёртка текстуры).
    model = f'''    <!-- ARUCO BEGIN (генерирует tools/gen_aruco_marker.py, руками не править) -->
    <!--
      aruco_dock — метка ArUco {a.dict} id {a.id} для парковки тележки: на торце стола, обращена
      к тележке (в -x). Метка {a.marker:.3f} м (без белой рамки), пластина {side:.3f} м,
      центр ({a.x}, {a.y}, {a.z}) в мире.
    -->
    <model name="aruco_dock">
      <static>true</static>
      <pose>{a.x} {a.y} {a.z} 0 0 0</pose>
      <link name="link">
        <visual name="board">
          <pose>0.003 0 0 0 0 0</pose>
          <geometry><box><size>0.006 {side} {side}</size></box></geometry>
          <material><ambient>1 1 1 1</ambient><diffuse>1 1 1 1</diffuse></material>
        </visual>
        <visual name="marker">
          <pose>-0.0005 0 0 0 0 0</pose>
          <geometry><plane><normal>-1 0 0</normal><size>{side} {side}</size></plane></geometry>
          <material>
            <ambient>1 1 1 1</ambient><diffuse>1 1 1 1</diffuse><specular>0 0 0 1</specular>
            <pbr><metal><albedo_map>materials/{name}</albedo_map><roughness>1.0</roughness><metalness>0.0</metalness></metal></pbr>
          </material>
        </visual>
      </link>
    </model>
{END}'''
    w = open(a.world).read()
    if BEGIN in w:
        i, j = w.index(BEGIN), w.index(END) + len(END)
        w = w[:i] + model + w[j:]
    else:
        k = w.rindex('  </world>')
        w = w[:k] + model + '\n\n' + w[k:]
    open(a.world, 'w').write(w)
    print(f'метка {a.dict} id {a.id}: worlds/materials/{name}; модель aruco_dock -> {a.world}')


if __name__ == '__main__':
    main()
