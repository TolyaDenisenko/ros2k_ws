# Камера Orbbec Gemini 335Le и вакуум в симуляции — устройство и проверка

Дополняемый документ. Статус и журнал — в [STATUS.md](STATUS.md). Здесь: как это
устроено, какие топики/фреймы/числа, и **команды для самостоятельной проверки**.

## 1. Что добавлено (карта файлов)

```
src/r2_robot/
  quasi_camera_description/            пакет: описание камеры (глава 3)
    urdf/cameras/gemini335le.xacro       макрос тела камеры (меш, фреймы, коллизия-бокс)
    urdf/gripper_camera.xacro            кронштейн + камера на link_eef (eye-in-hand)
    urdf/gz/gz_rgbd_camera.xacro         сенсор rgbd_camera для Gazebo
    config/gemini335le.yaml              параметры камеры в симуляции (FOV, разрешение, шум)
    meshes/gemini335Le/base_Link.STL     меш из OrbbecSDK_ROS2 (Apache-2.0)
    meshes/brackets/camera_stand_1300.stl  кронштейн из CAD
  quasi_cart_description/urdf/cart-arm.xacro   выбор камеры, плагины DetachableJoint (по одному на объект)
  quasi_cart_description/urdf/arm_on_table.urdf.xacro   та же рука на столе, без тележки
  cart_xarm_bringup/
    launch/cart_xarm6_gz.launch.py       рука на тележке: camera_model, gripper_camera_preview, headless; мосты; облако; нода вакуума
    launch/arm_table_gz.launch.py        рука на столе, 7 предметов (удобнее всего для проверки вакуума)
    config/gz_bridge.yaml                общий мост: clock, колёса, езда, /vacuum/pick_box/*, позы
    config/gz_bridge_arm_table.yaml      мост «руки на столе» (генерируется из arm_table_objects.yaml)
    config/gz_bridge_gripper_camera.yaml мост камеры Orbbec
    config/gz_bridge_d435i.yaml          мост старой D435i
    config/arm_table_objects.yaml        список предметов «руки на столе» (единственный источник)
    worlds/cart_table_offline.world      стол, коробка pick_box, цилиндр (тележка)
    worlds/arm_table.world               стол с 7 предметами (генерируется)
    tools/fake_xarm_vacuum.py            подмена драйвера xArm для проверки вакуума реальной руки
    rviz/cart_xarm.rviz, cart_xarm_moveit.rviz   + Image, PointCloud2, Depth, VacuumContact
src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro   «крючок» add_gripper_camera
src/xarm_grasping/scripts/
  vacuum_gripper_sim.py                «вакуум» в симуляции (сервис /vacuum_gripper/set)
  vacuum_gripper_xarm.py               вакуум РЕАЛЬНОЙ руки: тот же интерфейс поверх драйвера xArm
  vacuum_rviz_button.py                кнопка вакуума в RViz
  arm_goto_tcp.py                      утилита: подвести присоску к точке без MoveIt
```

## 2. Камера

### Фреймы (одинаковые в симуляции и у драйвера Orbbec при `camera_name:=gripper_camera`)

```
link_eef ─ gripper_camera_bracket_link              (кронштейн Camera_Stand_1300, меш из CAD)
link_eef ─ gripper_camera_mount_link                (нижний винт камеры; поза mount_xyz/mount_rpy)
            └ gripper_camera_link                   (начало глубины = левый ИК)
               ├ gripper_camera_depth_frame → …_depth_optical_frame
               ├ gripper_camera_color_frame → …_color_optical_frame   ← frame_id картинки и облака
               └ gripper_camera_left_ir_frame / …_right_ir_frame      (база 95 мм)
```

Расположение: `mount_xyz = 0.075 0 0.015`, `mount_rpy = 0 −π/2 0` (в `gripper_camera.xacro`):
камера прикручена задней панелью к лапке кронштейна, ось взгляда камеры (X) = ось
инструмента (Z `link_eef`), камера в ~6 см от оси присоски. Край присоски частично
виден в кадре (так и на железе). Камера сидит ~7 см выше кончика присоски и видит
только дальше 0.25 м — поэтому предметы ищут из позы обзора, а не вплотную.
Точную позу ещё надо уточнить калибровкой «рука — глаз» (hand-eye; не сделана).

### Топики (ROS)

| Топик | Тип | Примечание |
|---|---|---|
| `/gripper_camera/color/image_raw` | `sensor_msgs/Image` rgb8 | 1280×800 @15 Гц (`preview`: 640×400) |
| `/gripper_camera/aligned_depth/image_raw` | `sensor_msgs/Image` 32FC1 | метры, совмещена с цветом; вне [0.25; 6] м — `inf` |
| `/gripper_camera/color/camera_info` | `sensor_msgs/CameraInfo` | fx = fy ≈ 640 при 1280×800 / HFOV 90° |
| `/gripper_camera/points` | `sensor_msgs/PointCloud2` | цветное облако, строит `depth_image_proc` |

Параметры симуляции — `quasi_camera_description/config/gemini335le.yaml` (блоки `sim` и
`sim_preview`): FOV 90°, мёртвая зона `near` = 0.25 м, шум глубины σ = 2 мм.

### Аргументы launch

| Аргумент | По умолчанию | Смысл |
|---|---|---|
| `camera_model` | `gemini335le` | `d435i` — вернуть старую камеру (топики `/camera/...`) |
| `gripper_camera_preview` | `false` | `true` — режим 640×400 (лёгкий) |
| `headless` | `false` | `true` — Gazebo без GUI (для проверок по SSH) |
| `arm` | `true` | `false` — тележка без руки: нет камеры гриппера и вакуума |

Подробно обо всех аргументах — [RUN_SCENARIOS.md](RUN_SCENARIOS.md).

## 3. Сцена (мир `cart_table_offline.world`)

| Объект | Размеры | Положение (мир) | Положение в `link_base` руки* |
|---|---|---|---|
| Стол, столешница | 1.2 × 0.8 × 0.05 м, 4 ножки 6×6 см | центр x = 1.05, верх **z = 0.925** (плоский, без зазоров) | верх: z = +0.006 |
| **`pick_box`** (динамический, 0.1 кг) | **0.10 (X) × 0.05 (Y) × 0.05 (Z, высота)** м | центр (0.60, 0, 0.9505) | центр (0.6075, 0, 0.031), **верх z = +0.056** |
| `cylinder_block` (static) | r = 0.04, h = 0.09 | (0.80, −0.28, 0.9705) | — |

\* Столбец «в `link_base`» посчитан для старой тележки **c2** со спавном в (0, 0):
`link_base` руки в мире = (−0.0075, 0, 0.919). У тележки по умолчанию **c2l-v3**
рука крепится в другом месте (`arm_mount_xyz = -0.25 -0.1295 0.849`): по замеру из
комментария в `cart_xarm6_gz.launch.py` при спавне (−0.3, 0) `link_base` ≈ (−0.54, −0.135, 0.910),
до `pick_box` ≈ 1.14 м — **рука (вылет ~0.7 м) коробку не достаёт**. Сейчас в launch
`spawn_x` по умолчанию −0.5, тогда до коробки ещё ≈ 0.2 м дальше (не проверено).
Поэтому вакуум удобнее проверять на сцене «рука на столе» (ниже и 5.6).

Плагин `DetachableJoint` завязан на имя модели **`pick_box`** и link **`link`**, а
`PosePublisher` объявлен у самой модели в мире — при переименовании менять
`vacuum_targets` (`cart-arm.xacro` / launch), `object_names` ноды `vacuum_gripper_sim`
в launch и мост `gz_bridge.yaml`.

## 4. Вакуум

### Как устроен

Подробный разбор с кодом — [глава 9](09-вакуум-кнопка-готовность.md).

```
vacuum_gripper_sim (ROS)                          Gazebo
  сервис /vacuum_gripper/set (SetBool)  ┐
  топик  /vacuum_gripper/cmd (Bool)     ┤  то же самое, топик — без ответа
  кнопка в RViz (vacuum_rviz_button)    ┘  (вызывает сервис)
    true  ─ касание? ближайший объект ─ /vacuum/<объект>/attach ─► DetachableJoint: link6 ⇄ <объект>
    false ──────────────────────────── /vacuum/<объект>/detach ─►   соединение удаляется, объект падает
  /vacuum/<объект>/state (String) ◄──────────────────────────────── "attached" / "detached"
  /gz/robot_pose, /gz/object_pose ◄─ PosePublisher (10 Гц) — мировые позы для проверки касания
```

Объекты, которые можно взять, — xacro-аргумент `vacuum_targets` (`cart-arm.xacro`,
через пробел; на каждый свой плагин DetachableJoint; `none` — ни одного) и параметры ноды
`object_names` / `object_sizes` (по 3 числа X Y Z на объект; цилиндр — описанный бокс
2r × 2r × h). Тележка: `pick_box` (при `manipulation_nodes:=false` launch ставит `none`,
иначе приваренную при загрузке коробку некому отпустить и тележка её тащит); рука на
столе: все 7 предметов из `config/arm_table_objects.yaml` — `cube parallelepiped
rect_plate cylinder cube_2 cube_3 cylinder_2` (запасной список в
`arm_on_table.urdf.xacro` — `pick_box cylinder_block`, launch его переопределяет).
Модели в мире должны быть НЕ static, с link `link` и своим PosePublisher.

Проверка касания (параметры ноды): расстояние от TCP (кончик присоски) до поверхности
объекта ≤ `contact_distance` = **15 мм** и угол между осью инструмента и нормалью
грани ≤ `max_angle_deg` = **20°**. Из подходящих берётся ближайший. Иначе отказ с
причиной. `require_contact:=false` отключает проверку (только для отладки).

Плагин приваривает все объекты при загрузке модели; нода после старта сама
рассылает detach всем объектам, которые держать не велели (сообщение плагина об
этом часто теряется до подъёма моста — без этого объекты ездили за рукой).

Топики для наблюдения: `/vacuum_gripper/status` (ближайший объект, расстояние, угол,
держит ли, 2 Гц), `/vacuum_gripper/state` (Bool), `/vacuum_gripper/held` (имя объекта).

### Сигнал «предмет взят» и реальная рука

`/vacuum_gripper/state` (`std_msgs/Bool`, latched) — **единый сигнал «предмет взят»**
и в симуляции, и на реальной руке. `pick_place_server` проверяет его трижды (после
включения вакуума, после подъёма, после переноса); если `false` — выключает вакуум и
завершается ошибкой. Его параметры `probe_with_vacuum` (по умолчанию `false`; `true` —
вакуум включён во время спуска, для реальной руки) и `hold_confirm_s` (1.0 с) — см.
[PICK_PLACE.md](PICK_PLACE.md).

Для реальной руки вместо `vacuum_gripper_sim` запускают
`vacuum_gripper_xarm.py` — переходник к драйверу xArm (`/xarm/set_vacuum_gripper`,
`/xarm/get_vacuum_gripper`). Он даёт `/vacuum_gripper/set`, `/cmd`, `/state`, `/held`,
`/status`, но **не** `/ready`, `/contact`, `/markers` (датчик у гриппера только
цифровой: держит или нет, касание заранее не узнать). Защита насоса: выключает вакуум,
если предмета нет дольше `idle_timeout` (8 с), если драйвер молчит дольше
`comm_timeout` (2 с) и при выходе. В launch пока не включён; на реальной руке не
проверен. Проверка без руки — подмена драйвера `tools/fake_xarm_vacuum.py`, команды в
[главе 9, раздел 9.3а](09-вакуум-кнопка-готовность.md).

### Кнопка в RViz и «можно ли брать»

Нода `vacuum_rviz_button.py` (запускается обоими launch): кубик рядом с присоской.
Дисплей **InteractiveMarkers**, namespace `/vacuum_button` (есть в конфигах RViz).
Инструмент **Interact** на панели RViz → клик по кубику переключает вакуум, правая
кнопка — меню. Кнопка — светофор готовности:

| Цвет | Когда | Надпись |
|---|---|---|
| серый | дальше `near_distance` (5 см) | `ВАКУУМ выкл · pick_box: 256 мм` |
| жёлтый | ближе 5 см | `pick_box: 23 мм, 4°` |
| светло-зелёный, крупнее | касание: ≤ 15 мм и ≤ 20° — клик сработает | `МОЖНО БРАТЬ: pick_box 3 мм — клик` |
| зелёный | держит | `ВАКУУМ: держит pick_box` |
| красный, ~5 с | отказ | причина |

Дисплей **VacuumContact** (`/vacuum_gripper/markers`): линия от кончика присоски до
ближайшей точки поверхности, точка касания и полупрозрачная оболочка вокруг объекта,
который возьмётся, — тем же цветом.

Для программ и обучения (10 Гц): `/vacuum_gripper/ready` (Bool — вакуум сейчас
сработает) и `/vacuum_gripper/contact` (JSON):

```json
{"object": "pick_box", "dist": 0.001, "angle": 0.0, "level": "ready", "ready": true,
 "held": "", "frame": "world", "tcp": [0.4, 0.1, 0.051], "point": [0.4, 0.1, 0.05]}
```

`level`: `none` (нет данных) | `far` | `near` | `ready` | `held`.

### Рука на столе (отладка без тележки)

```bash
ros2 launch cart_xarm_bringup arm_table_gz.launch.py
```

Стол верх z=0.75 (в Gazebo), база руки на столешнице; в TF `world` = `link_base`,
верх стола z=0. Объекты (в link_base, `config/arm_table_objects.yaml`, 7 штук: 4 формы и
повторы):

| Имя | Форма, размер, м | Центр (x, y) | Верх z |
|---|---|---|---|
| `cube` | коробка 0.06³ | (0.50, 0.00) | 0.06 |
| `parallelepiped` | коробка 0.12 × 0.05 × 0.07 | (0.45, 0.18) | 0.07 |
| `rect_plate` | коробка 0.14 × 0.09 × 0.02 | (0.25, −0.05) | 0.02 |
| `cylinder` | цилиндр r 0.04, h 0.10 | (0.35, −0.20) | 0.10 |
| `cube_2` | коробка 0.06³ | (0.52, −0.13) | 0.06 |
| `cube_3` | коробка 0.06³ | (0.55, 0.10) | 0.06 |
| `cylinder_2` | цилиндр r 0.04, h 0.10 | (0.33, 0.08) | 0.10 |

Быстрый захват без MoveIt:

```bash
ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.00 --z 0.25            # над кубом
ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.00 --z 0.061 --lift 0  # присоска на верх куба (верх z=0.06)
ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{data: true}"
ros2 topic echo /vacuum_gripper/state --once                                 # data: true — взят
ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.00 --z 0.25 --lift 0   # поднять
ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{data: false}"  # уронить
```

## 5. Команды для проверки

Во всех терминалах: `cd ~/ros2k_ws && source /opt/ros/kilted/setup.bash && source install/setup.bash`.

### 5.1. Сборка

```bash
cd ~/ros2k_ws
colcon build --symlink-install --packages-select \
  quasi_camera_description quasi_cart_description xarm_description cart_xarm_bringup xarm_grasping
source install/setup.bash
```

### 5.2. URDF без симулятора (быстрая проверка модели)

```bash
xacro $(ros2 pkg prefix quasi_cart_description)/share/quasi_cart_description/urdf/cart_with_arm.urdf.xacro \
  use_sim:=true camera_model:=gemini335le > /tmp/cart.urdf && check_urdf /tmp/cart.urdf | head -3
# ожидаем: Successfully Parsed XML
grep -c "gripper_camera" /tmp/cart.urdf                 # > 0 — камера в модели
xacro ... use_sim:=true camera_model:=d435i > /tmp/cart_d435.urdf && check_urdf /tmp/cart_d435.urdf | head -2   # старая камера
xacro ... use_sim:=false > /tmp/cart_hw.urdf; grep -c gripper_camera_color_optical /tmp/cart_hw.urdf   # 0: на «железе» эти фреймы даёт драйвер
# (фреймы front_camera_* остаются всегда; убрать переднюю камеру — front_camera:=false)
```

### 5.3. Запуск (GUI Gazebo + RViz с MoveIt)

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py                          # полный режим 1280×800
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py gripper_camera_preview:=true   # лёгкий, если RTF низкий
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false        # обычный RViz
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py headless:=true use_moveit:=false use_rviz:=false  # без окон
```

RTF (должен быть ≥ 0.8 на целевом ПК):

```bash
gz topic -e -t /world/cart_xarm_world_offline/stats -n 1 | grep real_time_factor
```

### 5.4. Камера

```bash
ros2 topic hz /gripper_camera/color/image_raw                       # ≈15 Гц (RTF=1)
ros2 topic echo /gripper_camera/color/camera_info --once            # frame_id: gripper_camera_color_optical_frame; k[0]≈640
ros2 topic echo /gripper_camera/points --once --field header        # frame_id: gripper_camera_color_optical_frame
ros2 run tf2_ros tf2_echo link_base gripper_camera_color_optical_frame   # TF есть, без ошибок
ros2 run tf2_tools view_frames                                      # в дереве нет двойных родителей
```

Картинка без RViz: `ros2 run rqt_image_view rqt_image_view /gripper_camera/color/image_raw`.
В **RViz** (окно запускается тем же launch): дисплеи `GripperCamera` (картинка), `GripperPoints`
(цветное облако), `GripperDepth` (глубина, выключен — включите галочкой).

### 5.5. Сцена: стол ровный, коробка на месте

```bash
# коробка лежит на столе: z центра ≈ 0.95 (стол 0.925 + 0.025)
ros2 topic echo /gz/object_pose --once | grep -E "child_frame|x:|y:|z:"
# геометрия: коробка 0.10 x 0.05 x 0.05
gz sdf -p $(ros2 pkg prefix cart_xarm_bringup)/share/cart_xarm_bringup/worlds/cart_table_offline.world | grep -A3 "pick_box" | head
```

После старта коробка может сместиться на несколько мм (см. STATUS K2) — ориентируйтесь
на `/gz/object_pose`, а не на номинал.

### 5.6. Вакуум — полный сценарий проверки

> ⚠️ Числа ниже — для тележки **c2** со спавном в (0, 0):
> `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py robot_model:=c2 spawn_x:=0.0`
> (так сценарий проверялся раньше; с нынешними настройками не перепроверен). С тележкой
> по умолчанию c2l-v3 рука до `pick_box` не дотягивается (раздел 3). Самый простой
> вариант сегодня — сцена «рука на столе» и куб `cube` (раздел 4, команды те же,
> только координаты `--x 0.50 --y 0.00`, верх куба z = 0.06, `held` = `cube`).

Терминал 1 — запуск. Терминал 2:

```bash
# 0. Состояние: вакуум выкл., объект не приварен
ros2 topic echo /vacuum_gripper/status --once
ros2 topic echo /vacuum_gripper/state --once          # data: false

# 1. Отказ вдали от объекта (проверка защиты): рука в стартовой позе далеко
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
# ожидаем: success=False, message "отказ: нет касания — расстояние ... мм"

# 2. Подвести присоску на верх коробки. ВНИМАНИЕ: утилита грубая (интерполяция в суставах, без проверки
#    коллизий) — следите за рукой; при низком RTF добавьте --time 20. См. STATUS K9.
#    Подвести присоску на верх коробки (x,y,z — в link_base, z=0.058 = 2 мм над верхом коробки 0.056)
ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.058
#    (скорректируйте x,y по /gz/object_pose: x_base = x_world + 0.0075, y_base = y_world)
ros2 topic echo /vacuum_gripper/status --once          # расстояние < 15 мм, угол < 20°

# 3. Включить вакуум
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
# ожидаем: success=True, "объект захвачен (DetachableJoint: attached)"
ros2 topic echo /vacuum_gripper/held --once            # data: pick_box

# 4. Поднять: цель на 15 см выше. Коробка должна ехать вместе с присоской
ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.21 --lift 0
ros2 topic echo /gz/object_pose --once | grep "z:"    # z вырос (~1.1), а не 0.95

# 5. Отпустить — коробка падает на стол
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: false}"
ros2 topic echo /vacuum_gripper/held --once            # data: ''
```

Отладка в обход ноды (принудительно, без проверки касания):

```bash
gz topic -t /vacuum/pick_box/attach -m gz.msgs.Empty -p ""   # приварить
gz topic -t /vacuum/pick_box/detach -m gz.msgs.Empty -p ""   # отпустить
```

### 5.7. Старая камера (регрессия)

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py camera_model:=d435i use_moveit:=false
ros2 topic hz /camera/color/image_raw
```

## 6. Типичные проблемы

| Симптом | Причина / действие |
|---|---|
| RTF 0.1–0.2, камера «тормозит» | слабая GPU. `gripper_camera_preview:=true`, понизить `rate` в `gemini335le.yaml`, `headless:=true`, запускать на ПК с дискретной GPU |
| В Gazebo нет меша камеры (`Unable to find file ... STL`) | меш подключается через `file://$(find quasi_camera_description)`: пересобрать пакет (`colcon build --packages-select quasi_camera_description`) |
| `Scene already exists` + segfault | Sensors-система объявлена дважды: только в мире, не в модели |
| `/gz/object_pose` не приходит | нет моста: `ros2 node list \| grep gz_bridge`; в gz: `gz topic -l \| grep pick_box` |
| Сервис ответил отказом один раз, дальше молчит | старая версия `vacuum_gripper_sim` (менялся уровень лога в одной строке, исправлено 2026-11-09) — обновить и пересобрать |
| Тележка тащит `pick_box` за собой | плагин приварил коробку при загрузке, а `vacuum_gripper_sim` не запущен — `vacuum_targets:=none` / `manipulation_nodes:=false` |
| Сервис отвечает «нет TF base_footprint->link_tcp» | не запущен `robot_state_publisher` / нет `/merger/joint_states` |
| Сервис отвечает «плагин не подтвердил» | объект `pick_box` отсутствует в мире или другое имя (см. раздел 3) |
| Коробка «дрожит» после attach | контакт чаши с коробкой (STATUS K4); поднимайте TCP на 2–3 мм выше верха перед attach |
