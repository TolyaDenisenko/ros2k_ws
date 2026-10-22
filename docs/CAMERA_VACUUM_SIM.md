# Камера Orbbec Gemini 335Le и вакуум в симуляции — устройство и проверка

Дополняемый документ. Статус и журнал — в [STATUS.md](STATUS.md). Здесь: как это
устроено, какие топики/фреймы/числа, и **команды для самостоятельной проверки**.

## 1. Что добавлено (карта файлов)

```
src/r2_robot/
  quasi_camera_description/            НОВЫЙ пакет: описание камеры
    urdf/cameras/gemini335le.xacro       макрос тела камеры (меш, фреймы, коллизия-бокс)
    urdf/gripper_camera.xacro            кронштейн + камера на link_eef (eye-in-hand)
    urdf/gz/gz_rgbd_camera.xacro         сенсор rgbd_camera для Gazebo
    config/gemini335le.yaml              параметры камеры в симуляции (FOV, разрешение, шум)
    meshes/gemini335Le/base_Link.STL     меш из OrbbecSDK_ROS2 (Apache-2.0)
  quasi_cart_description/urdf/cart-arm.xacro   выбор камеры, плагины DetachableJoint + PosePublisher
  cart_xarm_bringup/
    launch/cart_xarm6_gz.launch.py       аргументы camera_model, gripper_camera_preview, headless; мосты; облако; нода вакуума
    config/gz_bridge.yaml                общий мост: clock, колёса, езда, /vacuum/*, позы
    config/gz_bridge_gripper_camera.yaml мост камеры Orbbec
    config/gz_bridge_d435i.yaml          мост старой D435i
    worlds/cart_table_offline.world      выровненный стол, коробка pick_box, цилиндр
    rviz/cart_xarm.rviz, cart_xarm_moveit.rviz   + Image, PointCloud2, Depth
src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro   «крючок» add_gripper_camera
src/xarm_grasping/scripts/
  vacuum_gripper_sim.py                нода «вакуума» (сервис /vacuum_gripper/set)
  arm_goto_tcp.py                      утилита: подвести присоску к точке без MoveIt
```

## 2. Камера

### Фреймы (одинаковые в симуляции и у драйвера Orbbec при `camera_name:=gripper_camera`)

```
link_eef ─ gripper_camera_bracket_link              (кронштейн-заглушка 37×40×60 мм)
link_eef ─ gripper_camera_mount_link                (нижний винт камеры; поза mount_xyz/mount_rpy)
            └ gripper_camera_link                   (начало глубины = левый ИК)
               ├ gripper_camera_depth_frame → …_depth_optical_frame
               ├ gripper_camera_color_frame → …_color_optical_frame   ← frame_id картинки и облака
               └ gripper_camera_left_ir_frame / …_right_ir_frame      (база 95 мм)
```

Расположение: `mount_xyz = 0.075 0 0.030`, `mount_rpy = π −π/2 0` (в `gripper_camera.xacro`):
ось взгляда камеры (X) = ось инструмента (Z `link_eef`), камера смещена на ~9 см от оси
присоски. Присоска в кадр не попадает. Значения стартовые — уточнить по CAD кронштейна и
hand-eye калибровкой (док. 08).

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

## 3. Сцена (мир `cart_table_offline.world`)

| Объект | Размеры | Положение (мир) | Положение в `link_base` руки* |
|---|---|---|---|
| Стол, столешница | 1.2 × 0.8 × 0.05 м, 4 ножки 6×6 см | центр x = 1.05, верх **z = 0.925** (плоский, без зазоров) | верх: z = +0.006 |
| **`pick_box`** (динамический, 0.1 кг) | **0.10 (X) × 0.05 (Y) × 0.05 (Z, высота)** м | центр (0.60, 0, 0.9505) | центр (0.6075, 0, 0.031), **верх z = +0.056** |
| `cylinder_block` (static) | r = 0.04, h = 0.09 | (0.80, −0.28, 0.9705) | — |

\* `link_base` руки в мире = (−0.0075, 0, 0.919) при `spawn_x=y=0`, `spawn_z=0.07`.
Плагины `DetachableJoint` и `PosePublisher` завязаны на имя модели **`pick_box`** и
link **`link`** — при переименовании менять `vacuum_targets` в `cart-arm.xacro`
и `object_model_name` в launch.

## 4. Вакуум

### Как устроен

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
через пробел; на каждый свой плагин DetachableJoint) и параметры ноды
`object_names` / `object_sizes` (по 3 числа X Y Z на объект; цилиндр — описанный бокс
2r × 2r × h). Тележка: `pick_box`; рука на столе: `pick_box cylinder_block`.
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
верх стола z=0. Объекты (в link_base): `pick_box` (0.40, 0.10), верх z=0.05;
`cylinder_block` (0.35, −0.20), торец z=0.09. Быстрый захват без MoveIt:

```bash
ros2 run xarm_grasping arm_goto_tcp.py --x 0.40 --y 0.10 --z 0.25            # над коробкой
ros2 run xarm_grasping arm_goto_tcp.py --x 0.40 --y 0.10 --z 0.051 --lift 0  # присоска на верх
ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{data: true}"
ros2 run xarm_grasping arm_goto_tcp.py --x 0.40 --y 0.10 --z 0.25 --lift 0   # поднять
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
xacro ... use_sim:=false > /tmp/cart_hw.urdf; grep -c optical /tmp/cart_hw.urdf   # 0: на «железе» внутренних фреймов нет
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

Терминал 1 — запуск (5.3). Терминал 2:

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
| `/gz/object_pose` не приходит | нет моста: `ros2 node list | grep gz_bridge`; в gz: `gz topic -l | grep pick_box` |
| Сервис отвечает «нет TF base_footprint->link_tcp» | не запущен `robot_state_publisher` / нет `/merger/joint_states` |
| Сервис отвечает «плагин не подтвердил» | объект `pick_box` отсутствует в мире или другое имя (см. раздел 3) |
| Коробка «дрожит» после attach | контакт чаши с коробкой (STATUS K4); поднимайте TCP на 2–3 мм выше верха перед attach |
