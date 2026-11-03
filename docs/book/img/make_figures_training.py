#!/usr/bin/env python3
"""Рисунки к документам MODEL_TRAINING.md и RECOGNITION_OPENVINO.md (docs/book/img/fig2*.png).

Откуда данные:
  * кривые обучения — настоящий журнал ~/.local/share/quasi/training/runs/shapes_v3/log.csv;
  * кадры датасета — ~/.local/share/quasi/training/datasets/sim_shapes_v3/preview/*.jpg
    (кадры камеры из симуляции с рамками, посчитанными сборщиком);
  * цифры качества (mAP, пороги, уверенность по классам) — замеры 2026-11-02 скриптами
    evaluate_detector.py и подсчётом по проверочной выборке; внесены ниже константами, чтобы
    рисунки собирались без запуска нейросети;
  * fig26/fig27 — снимки (копируются отдельно, см. раздел «снимки» в конце файла).

    python3 docs/book/img/make_figures_training.py        # из корня workspace
Тяжёлых файлов в репозиторий не кладётся: на выходе только маленькие PNG.
"""
import csv
import glob
import os

import cv2
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
Q = os.path.expanduser('~/.local/share/quasi/training')

plt.rcParams.update({'font.size': 11, 'figure.dpi': 110, 'axes.grid': True, 'grid.alpha': 0.3,
                     'axes.spines.top': False, 'axes.spines.right': False})
BLUE, GREEN, ORANGE, RED, GREY = '#2b6cb0', '#2f9e44', '#e8890c', '#d1342f', '#8a8f98'


def save(fig, name):
    fig.tight_layout()
    fig.savefig(os.path.join(HERE, name), dpi=110)
    plt.close(fig)
    print('->', name)


# ---------------------------------------------------------------- fig20: кадры датасета
def fig20_dataset():
    files = sorted(glob.glob(os.path.join(Q, 'datasets/sim_shapes_v3/preview/*.jpg')))[:6]
    if not files:
        print('нет превью датасета — fig20 пропущен')
        return
    fig, axs = plt.subplots(2, 3, figsize=(12, 5.6))
    for ax, f in zip(axs.ravel(), files):
        ax.imshow(cv2.cvtColor(cv2.imread(f), cv2.COLOR_BGR2RGB))
        ax.axis('off')
    fig.suptitle('Кадры датасета: красные рамки и подписи посчитаны сборщиком автоматически (вручную не рисовались)')
    save(fig, 'fig20_dataset_samples.png')


# ---------------------------------------------------------------- fig21: кривые обучения
def fig21_training():
    path = os.path.join(Q, 'runs/shapes_v3/log.csv')
    if not os.path.exists(path):
        print('нет log.csv — fig21 пропущен')
        return
    rows = list(csv.DictReader(open(path)))
    ep = [int(r['epoch']) for r in rows]
    loss = [float(r['loss']) for r in rows]
    em = [int(r['epoch']) for r in rows if r['map'] != 'nan']
    mp = [float(r['map']) for r in rows if r['map'] != 'nan']
    fig, ax = plt.subplots(figsize=(9, 4.6))
    ax.plot(ep, loss, color=RED, lw=2, label='потери на обучающих картинках (ниже — лучше)')
    ax.set_xlabel('эпоха (один проход по всем картинкам)')
    ax.set_ylabel('потери', color=RED)
    ax2 = ax.twinx()
    ax2.plot(em, mp, color=GREEN, lw=2, marker='o', ms=4, label='mAP на проверочных картинках (выше — лучше)')
    ax2.set_ylabel('mAP', color=GREEN)
    ax2.set_ylim(0, 1)
    ax2.grid(False)
    best = int(np.argmax(mp))
    ax2.annotate(f'лучший: {mp[best]:.3f}\n(эпоха {em[best]})', (em[best], mp[best]),
                 (em[best] - 22, mp[best] - 0.33), arrowprops=dict(arrowstyle='->'), color=GREEN)
    h1, l1 = ax.get_legend_handles_labels()
    h2, l2 = ax2.get_legend_handles_labels()
    ax.legend(h1 + h2, l1 + l2, loc='center right', fontsize=9)
    ax.set_title('Как идёт обучение: потери падают, mAP растёт и выходит на «полку»')
    save(fig, 'fig21_training_curve.png')


# ---------------------------------------------------------------- fig22: точности fp32/fp16/int8
def fig22_precision():
    names = ['fp32', 'fp16', 'int8']
    mAP = [0.945, 0.945, 0.897]       # evaluate_detector.py, sim_shapes_v3, 75 проверочных кадров
    ms = [7.3, 6.4, 5.9]
    mb = [8.9, 4.5, 2.4]
    fig, axs = plt.subplots(1, 3, figsize=(11, 3.8))
    for ax, vals, title, fmt, col in zip(
            axs, (mAP, ms, mb), ('качество (mAP, ось от 0.8), выше — лучше', 'время кадра, мс, ниже — лучше',
                                 'размер файла, МБ, ниже — лучше'), ('{:.3f}', '{:.1f}', '{:.1f}'),
            (GREEN, BLUE, ORANGE)):
        b = ax.bar(names, vals, color=col, width=0.6)
        ax.set_title(title, fontsize=10)
        for r, v in zip(b, vals):
            ax.text(r.get_x() + r.get_width() / 2, v, fmt.format(v), ha='center', va='bottom')
        ax.set_ylim(0, max(vals) * 1.18)
    axs[0].set_ylim(0.8, 0.97)
    fig.suptitle('Три «точности» одной и той же модели (процессор, 4 потока): fp16 — золотая середина')
    save(fig, 'fig22_precision.png')


# ---------------------------------------------------------------- fig23: порог уверенности
def fig23_threshold():
    thr = [0.40, 0.30, 0.20, 0.15, 0.10, 0.05]
    found = [92, 94, 97, 98, 99, 99]      # % найденных предметов (259 рамок), sim_shapes_v3 int8
    false = [1, 1, 1, 1, 1, 1]            # ложные рамки на 75 кадров
    fig, ax = plt.subplots(figsize=(8.6, 4.4))
    ax.plot(thr, found, color=GREEN, lw=2.5, marker='o')
    ax.set_xlabel('порог уверенности (рамки ниже порога отбрасываются)')
    ax.set_ylabel('найдено предметов, %', color=GREEN)
    ax.set_ylim(85, 101)
    ax.invert_xaxis()
    ax.axvspan(0.10, 0.20, color=ORANGE, alpha=0.18)
    ax.text(0.15, 90, 'рекомендуемая зона\n0.1 – 0.2', ha='center', color=ORANGE)
    ax2 = ax.twinx()
    ax2.bar(thr, false, width=0.02, color=RED, alpha=0.55)
    ax2.set_ylabel('ложных рамок на 75 кадров', color=RED)
    ax2.set_ylim(0, 10)
    ax2.grid(False)
    ax.set_title('Чем ниже порог, тем больше найдено; ложных рамок почти нет', fontsize=11)
    save(fig, 'fig23_threshold.png')


# ---------------------------------------------------------------- fig24: уверенность по классам
def fig24_per_class():
    cls = ['rect_plate', 'parallelepiped', 'cylinder', 'cube']
    conf = [1.00, 0.95, 0.95, 0.65]     # средняя уверенность, sim_shapes_v3 int8
    found04 = [100, 98, 95, 73]         # найдено при пороге 0.4, %
    y = np.arange(len(cls))
    fig, axs = plt.subplots(1, 2, figsize=(10, 3.8), sharey=True)
    axs[0].barh(y, conf, color=BLUE)
    axs[0].set_yticks(y, cls)
    axs[0].set_xlim(0, 1.1)
    axs[0].set_title('средняя уверенность модели')
    axs[1].barh(y, found04, color=GREEN)
    axs[1].set_xlim(0, 110)
    axs[1].set_title('найдено при пороге 0.4, %')
    for ax, v, f in ((axs[0], conf, '{:.2f}'), (axs[1], found04, '{:.0f}')):
        for i, x in enumerate(v):
            ax.text(x, i, ' ' + f.format(x), va='center')
    axs[0].invert_yaxis()
    fig.suptitle('Куб самый «неуверенный»: сверху он выглядит как обычный квадрат')
    save(fig, 'fig24_per_class.png')


# ---------------------------------------------------------------- fig25: мелкие против крупных
def fig25_small_vs_big():
    old = {'box_small': 0.25, 'cyl_small': 0.14, 'cylinder_block': 0.78, 'pick_box': 0.88,
           'box_tall': 0.93, 'cyl_tall': 0.91, 'box_flat': 1.00, 'cyl_wide': 1.00}
    new = {'cube': 0.65, 'parallelepiped': 0.95, 'cylinder': 0.95, 'rect_plate': 1.00}
    fig, axs = plt.subplots(1, 2, figsize=(11, 4.2), gridspec_kw={'width_ratios': [2, 1.3]})
    for ax, data, title in ((axs[0], old, 'Первый набор: 8 предметов, два мелких'),
                            (axs[1], new, 'Нынешний набор: 4 крупных')):
        names = list(data)
        vals = [data[n] for n in names]
        cols = [RED if v < 0.5 else (ORANGE if v < 0.8 else GREEN) for v in vals]
        ax.bar(names, vals, color=cols)
        ax.axhline(0.4, color=GREY, ls='--')
        ax.text(-0.4, 0.43, 'порог 0.4', ha='left', color=GREY, fontsize=9)
        ax.set_ylim(0, 1.1)
        ax.set_title(title, fontsize=11)
        ax.tick_params(axis='x', rotation=35)
    axs[0].set_ylabel('средняя уверенность модели')
    fig.suptitle('Мелкие предметы (5 см) модель видит, но уверенность низкая, и ниже порога они пропадают')
    save(fig, 'fig25_small_vs_big.png')


# ---------------------------------------------------------------- fig26: история экспериментов
def fig26_history():
    runs = [('1. мелкие\nпредметы, 320', 0.859), ('2. мелкие,\nвход 480', 0.888),
            ('3. пять форм,\n320', 0.941), ('4. пять форм,\nвход 480', 0.949),
            ('5. четыре формы,\n320 (итог)', 0.940)]
    fig, ax = plt.subplots(figsize=(10, 4.2))
    cols = [GREY, GREY, GREY, GREY, GREEN]
    b = ax.bar([r[0] for r in runs], [r[1] for r in runs], color=cols, width=0.6)
    for r, (_, v) in zip(b, runs):
        ax.text(r.get_x() + r.get_width() / 2, v, f'{v:.3f}', ha='center', va='bottom')
    ax.set_ylim(0.7, 1.0)
    ax.set_ylabel('лучший mAP при обучении')
    ax.set_title('Пять прогонов обучения (ось от 0.7). mAP — не всё: см. уверенность', fontsize=11)
    save(fig, 'fig26_history.png')


if __name__ == '__main__':
    fig20_dataset()
    fig21_training()
    fig22_precision()
    fig23_threshold()
    fig24_per_class()
    fig25_small_vs_big()
    fig26_history()

# ------------------------------------------------------------------ снимки
# fig27_sim_detection.png — кадр /detections/image из симуляции (модель sim_shapes_v3/model_fp16.xml,
#   detector_conf:=0.2); fig28_bad_dataset.png — превью испорченного датасета (предметы не двигались, а
#   рамки случайные). Это одноразовые снимки: перерисовывать их не надо.
