# Запуск сценариев: рука на столе и рука на тележке

Полное руководство: что запускать, с какими аргументами, что при этом поднимается, как проверить,
что всё работает, и как повторить проверки, которые я делал при разработке. Все команды и
значения по умолчанию сняты с настоящих launch-файлов (`ros2 launch ... --show-args`), а
перечни узлов, топиков и действий — с живых запусков 2026-11-02. Где что-то не проверялось,
так и написано.

Содержание: [0. Какой сценарий выбрать](#0-какой-сценарий-выбрать) ·
[1. Подготовка](#1-подготовка) · [2. Рука на столе](#2-сценарий-а-рука-на-столе-arm_table_gz) ·
[3. Рука на тележке](#3-сценарий-б-рука-на-тележке) ·
[4. Распознавание объектов (OpenVINO)](#4-распознавание-объектов-openvino) ·
[5. Как повторить мои проверки](#5-как-повторить-мои-проверки) ·
[6. Нагрузка на ПК](#6-нагрузка-на-пк) · [7. Готовые рецепты](#7-готовые-рецепты) ·
[8. Неполадки](#8-неполадки) · [9. Что не проверено](#9-что-не-проверено)

---

## 0. Какой сценарий выбрать

| Сценарий | Launch-файл | Для чего | Что умеет |
|---|---|---|---|
| **А. Рука на столе** | `arm_table_gz.launch.py` | отладка захвата вакуумной присоской, восприятия, pick&place | полный конвейер: объекты по камере → сцена MoveIt → взять и поставить |
| **Б1. Рука на тележке, ручное управление** | `cart_xarm6_gz.launch.py` | проверка тележки с рукой целиком: датчики, камеры, MoveIt, вакуум | датчики тележки (лидар, ToF, передняя камера), рука с MoveIt и вакуумом, ручной захват |
| **Б2. Рука на тележке, навигация** | `cart_nav_gz.launch.py` | езда по карте: Nav2 + SLAM + OctoMap | автономная езда по целям, рука на тормозе в «арке» |

- В **А** рука закреплена на столе, тележки нет: всё, что связано с объектами на столе, работает
  только здесь (`/objects`, `/pick_place`, зоны установки, детектор классов).
- В **Б1** и **Б2** рука стоит на тележке `c2l-v3`. Конвейера pick&place на тележке пока **нет**:
  база руки в ~1.14 м от `pick_box` и до него не достаёт (CLAUDE.md, «Gotchas»).
- **Б2** — это тот же стенд, что **Б1**, но с навигацией и без нод захвата (аргументы
  `manipulation_nodes:=false`, `arm_parked:=true`); отдельная поза руки «перевёрнутая U».

---

## 1. Подготовка

### 1.1. Один раз на машину

Базовая установка ROS 2 Kilted, Gazebo, сборка — в [README.md](../../README.md). Кроме неё для
перечисленных сценариев нужны:

```bash
sudo apt install ros-kilted-navigation2 ros-kilted-nav2-bringup ros-kilted-slam-toolbox \
                 ros-kilted-octomap-server ros-kilted-octomap-rviz-plugins \
                 ros-kilted-topic-tools ros-kilted-depth-image-proc
```

- `topic_tools` — прореживание часов `/clock` (без него симуляция не получает времени).
- `depth_image_proc` — облака точек камер из глубины.
- Навигация, SLAM, OctoMap — только для Б2.

Сборка (всегда из корня workspace):

```bash
cd ~/ros2k_ws
source /opt/ros/kilted/setup.bash
colcon build --symlink-install
source install/setup.bash
```

С `--symlink-install` правки launch, xacro, Python и yaml подхватываются сразу; **новые** файлы
требуют пересборки пакета (`colcon build --symlink-install --packages-select <пакет>`).

Распознавание объектов (OpenVINO) — отдельная установка, см. §4.1. Для сценариев без детектора
она не нужна.

### 1.2. В каждом терминале

```bash
source /opt/ros/kilted/setup.bash
source ~/ros2k_ws/install/setup.bash
```

### 1.3. Перед каждым запуском

```bash
pgrep -a gz            # должно быть пусто; если висит gz sim server — закрыть: kill <pid>
fastdds shm clean      # убирает мёртвые сегменты Fast DDS (см. §8: из-за них сервисы «не находятся»)
```

### 1.4. Как остановить

`Ctrl+C` в терминале с launch и подождать 10–15 секунд, пока завершатся все процессы.
Если после этого осталось лишнее (`pgrep -a gz`), закрыть вручную. **Не** убивать процессы
`kill -9`: от этого копится мусор в общей памяти (§8).

### 1.5. Несколько симуляций одновременно

Две симуляции в одном домене ROS мешают друг другу (двойные часы, двойные контроллеры). Для
второй задайте отдельные домен и раздел Gazebo:

```bash
export ROS_DOMAIN_ID=77 GZ_PARTITION=my_test
```

### 1.6. Экран

Gazebo (если без `headless:=true`) и RViz открываются на экране. На машине без экрана или по SSH
используйте `headless:=true use_rviz:=false` и смотрите данные через `ros2 topic ...` (§5).

---

## 2. Сценарий А: рука на столе (`arm_table_gz`)

### 2.1. Что это

Рука xArm6 с вакуумной присоской и камерой Orbbec Gemini 335Le на запястье стоит на столе,
тележки нет. Сборка руки та же, что на тележке (`arm_on_table.urdf.xacro` → `cart-arm.xacro`),
поэтому всё, что отлажено здесь, переносится на тележку. Система координат: кадр `world` =
`link_base` (база руки), верх стола на z = 0.

### 2.2. Что стоит на столе

Мир `worlds/arm_table.world` и мост `config/gz_bridge_arm_table.yaml` **генерируются** из
`config/arm_table_objects.yaml` инструментом `tools/gen_arm_table_scene.py` (запустить и
пересобрать пакет, если меняете список). 8 объектов, координаты `xy` — в кадре `link_base`:

| Имя (класс) | Форма | Размер, м | xy |
|---|---|---|---|
| `pick_box` | коробка | 0.10 × 0.05 × 0.05 | (0.40, 0.10) |
| `box_small` | коробка | 0.06 × 0.06 × 0.04 | (0.50, 0.00), поворот 0.3 рад |
| `box_tall` | коробка | 0.07 × 0.05 × 0.10 | (0.52, 0.15) |
| `box_flat` | коробка | 0.12 × 0.08 × 0.03 | (0.27, 0.17) |
| `cylinder_block` | цилиндр | радиус 0.04, высота 0.09 | (0.35, −0.20) |
| `cyl_small` | цилиндр | 0.025 × 0.06 | (0.50, −0.15) |
| `cyl_wide` | цилиндр | 0.05 × 0.05 | (0.27, −0.08) |
| `cyl_tall` | цилиндр | 0.035 × 0.13 | (0.40, −0.05) |

Зоны, куда ставить объекты, — `xarm_grasping/config/place_zones.yaml` (`zone_boxes`,
`zone_cylinders`); классы по размерам — `xarm_grasping/config/objects.yaml`.

### 2.3. Запуск

```bash
ros2 launch cart_xarm_bringup arm_table_gz.launch.py                 # всё: Gazebo, RViz с MoveIt
ros2 launch cart_xarm_bringup arm_table_gz.launch.py headless:=true use_moveit:=false use_rviz:=false
ros2 launch cart_xarm_bringup arm_table_gz.launch.py detector:=true  # + классы по картинке (§4)
```

Старт занимает около минуты: Gazebo, спавн руки (3 с), контроллеры (6 с), вакуум и маркеры (8 с),
восприятие и сцена (10 с), MoveIt позже.

### 2.4. Аргументы

| Аргумент | По умолчанию | Что делает и когда менять |
|---|---|---|
| `use_moveit` | `true` | `true` — поднимается `move_group` и `pick_place_server`, окно RViz MoveIt (панель MotionPlanning). `false` — без MoveIt: `/pick_place` нет, но руку можно двигать утилитой `arm_goto_tcp.py`, а вакуум и восприятие работают. Для отладки камеры и вакуума, а также если нужна лёгкая нагрузка |
| `use_rviz` | `true` | открыть RViz. С MoveIt — окно MoveIt (`cart_xarm_moveit.rviz`), без него — обычное (`cart_xarm.rviz`). `false` — без окна RViz (данные доступны в топиках) |
| `headless` | `false` | `true` — Gazebo без окна (физика и рендер камер работают). Рекомендуется для проверок, а также когда окно мешает нагрузкой (§6) |
| `gripper_camera_preview` | `false` | `true` — камера запястья 640×400 вместо 1280×800: облако в 4 раза легче. Для отладки движений и при низкой скорости симуляции. Распознавание на лёгком кадре хуже |
| `arm_initial_positions` | `0 -0.596 -1.013 0 1.609 0` | начальные углы `joint1..joint6` (рад) через пробел. Это **поза наблюдения**: присоска вниз в (0.30, 0, 0.40); `pick_place_server` распознаёт объекты только из неё. Менять только для опытов |
| `detector` | `false` | `false` — без классов по картинке; `true` — запустить `openvino_detector` (§4); `stub` — детектор не запускать, но `object_perception` слушает `/detections` (для заглушки `tools/stub_detections.py`) |
| `gz_args`, `gz_version`, `ign_*`, `debugger`, `debug_env`, `on_exit_shutdown` | см. `--show-args` | служебные аргументы включаемого `gz_sim.launch.py`; обычно не нужны |

### 2.5. Что поднимается

Узлы (по группам):

| Группа | Узлы | Зачем |
|---|---|---|
| Симуляция | `gazebo`, `create` (спавн), `robot_state_publisher`, `gz_bridge`, `gz_bridge_gripper_camera`, `clock_throttle`, `gripper_camera_proc` (+ `gripper_camera_points`) | мир, рука, TF, мост данных, часы `/clock` 200 Гц, облако точек камеры |
| Контроллеры | `joint_state_broadcaster`, `xarm6_traj_controller` (спавнеры) | управление суставами |
| Вакуум | `vacuum_gripper_sim`, `vacuum_rviz_button`, `scene_markers` | присоска на `DetachableJoint`, кнопка в RViz, истинная сцена в RViz |
| Восприятие | `object_perception`, `grasp_analyzer` | объекты на столе (`/objects`); старый анализатор захвата (ищет «цилиндр») |
| Планирование | `scene_manager`, `pick_place_server`, `move_group` | сцена MoveIt, зоны, «взять и поставить» (последние два — при `use_moveit:=true`) |
| Классы по картинке | `openvino_detector` | только при `detector:=true` |

При запуске MoveIt в логе будут сообщения `Could not find the planner configuration 'RRTConnect' on
the param server` и `No 3D sensor plugin(s) defined for octomap updates`. Они есть и в рабочем
запуске, а `pick_place` при них проходил (проверка пользователя, 2026-10-27).

### 2.6. Что делать после запуска

**Посмотреть, что видит система.**

```bash
ros2 topic echo /objects --field objects     # id, класс, форма, поза, grasp_pose, detector_label
ros2 topic hz /gripper_camera/points         # облако камеры
```

Если консоль пишет `Unknown topic '/objects'`, подождите 10–20 секунд и повторите: объекты
появляются после первого облака, а консоль узнаёт о топике с задержкой.

**Включить и выключить вакуум** (подробности — в заголовке `arm_table_gz.launch.py` и
`docs/CAMERA_VACUUM_SIM.md`):

```bash
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"   # включить
ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{data: false}" # выключить
ros2 topic echo /vacuum_gripper/ready        # можно ли брать (касание есть)
ros2 topic echo /vacuum_gripper/held         # что держим
```

В RViz — кнопка-кубик у присоски (инструмент Interact): клик включает и выключает вакуум, цвет —
«светофор» готовности.

**Взять и поставить** (нужен `use_moveit:=true`):

```bash
ros2 action send_goal /pick_place xarm_grasping_msgs/action/PickPlace \
    "{object_id: pick_box_0}" --feedback                                  # зона по правилам
ros2 action send_goal /pick_place xarm_grasping_msgs/action/PickPlace \
    "{object_id: cylinder_block_0, zone: zone_cylinders}" --feedback
```

Шаги (`stage` в обратной связи): `detect` → поиск места в зоне → `plan_pregrasp` → `approach` →
`grip` → `lift` → `transfer` → `lower` → `release` → `retreat` → `detect`. Подробно —
[PICK_PLACE.md](../PICK_PLACE.md).

**Другие действия:** `/detect_objects` (распознать сейчас), `/find_place_pose` (где поставить).

### 2.7. Справочник интерфейсов (с живого запуска)

| Тип | Что |
|---|---|
| Камера запястья | `/gripper_camera/color/image_raw`, `/gripper_camera/aligned_depth/image_raw`, `/gripper_camera/points`, `/gripper_camera/color/camera_info` |
| Объекты | `/objects` (`DetectedObjectArray`), `/objects/markers`, `/scene_markers` |
| Классы по картинке | `/detections`, `/detections/image` (при `detector:=true`) |
| Вакуум | `/vacuum_gripper/{cmd,state,held,ready,contact,status,markers}`, сервис `/vacuum_gripper/set`, `/vacuum/<объект>/{attach,detach,state}` |
| Захват (старый) | `/xarm_grasping/{best_grasp_pose,grasp_candidates,grasp_markers}` |
| Действия | `/pick_place`, `/detect_objects`, `/find_place_pose`, `/move_action`, `/execute_trajectory`, `/xarm6_traj_controller/follow_joint_trajectory` |
| Рука | `/joint_states`, `/dynamic_joint_states`, `/tf` |

### 2.8. Что должно быть, чтобы считать запуск удачным

1. В логе нет `process has died` (кроме завершения по Ctrl+C).
2. `ros2 topic echo /objects --field objects` показывает 8 объектов; в каждом `id`, `label` и
   `num_points` > 0.
3. `ros2 topic echo /vacuum_gripper/ready` отвечает.
4. (С MoveIt) `ros2 action list` содержит `/pick_place` и `/move_action`.
5. (С `detector:=true`) `ros2 topic hz /detections` ≈ 2 Гц (`max_rate_hz` в конфиге).

---

## 3. Сценарий Б: рука на тележке

Тележка `c2l-v3` (два ведущих колеса сзади, два кастера спереди) с рукой на верхней плите, на
стороне ручки. В симуляции привод — `DiffDrive` на колёсах; рука стоит на тормозе (контроллер
удержания), когда её не используют. Детали — [NAV.md](NAV.md), CLAUDE.md («Gotchas»).

### 3.1. Б1. Рука на тележке, ручное управление (`cart_xarm6_gz`)

**Для чего:** проверить тележку с рукой целиком: датчики, камеры, MoveIt, вакуум, ручную езду
командами `/cmd_vel`. Конвейера восприятия и `/pick_place` на тележке нет.

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py                           # всё, с MoveIt
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false         # RViz без MoveIt
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false use_rviz:=false headless:=true
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm_parked:=true          # рука на тормозе с самого старта
```

| Аргумент | По умолчанию | Что делает и когда менять |
|---|---|---|
| `world` | `worlds/cart_table_offline.world` | мир Gazebo. Варианты: `cart_nav.world` (комната 9×7 м с препятствиями), `cart_table.world` (нужен Fuel, интернет). Стол в `cart_table_offline` на x = 1.05, верх z = 0.925 |
| `use_moveit` | `true` | `true` — `move_group` и RViz MoveIt (`cart_xarm_moveit.rviz`); `false` — обычный RViz (`cart_xarm.rviz`) |
| `use_rviz` | `true` | открыть RViz |
| `headless` | `false` | Gazebo без окна |
| `robot_model` | `c2l-v3` | тип тележки: `c2l-v3` или `c2` (файл `quasi_cart_description/urdf/<модель>.urdf.xacro`, см. `CART_MODELS.md`). Точка крепления руки зависит от модели. `c2` в текущей сборке не проверялся |
| `spawn_x`, `spawn_y`, `spawn_z`, `spawn_yaw` | `-0.5`, `0.0`, `0.07`, `0.0` | где появляется тележка (м, рад). Стол на x = 1.05: при `spawn_x` ближе 0.3 м к столу кузов в него упрётся |
| `arm_initial_positions` | `1.5708 -0.9 -1.3 0 1.0 0` | начальные углы руки; по умолчанию камера смотрит вбок (K8 в STATUS.md). Для опытов с камерой — своя поза |
| `arm_parked` | `false` | `true` — рука под тормозом с самого старта и «не включается, пока нет запроса»; снять: `ros2 service call /arm_parking/set std_srvs/srv/SetBool "{data: false}"`. При `false` рука паркуется сама только на время езды |
| `manipulation_nodes` | `true` | ноды захвата (`vacuum_gripper_sim`, `vacuum_rviz_button`, `scene_markers`, `grasp_analyzer`, облако камеры запястья). `false` — только езда (`arm_parking` остаётся); при этом `pick_box` не приваривается к руке (`vacuum_targets:=none`) |
| `camera_model` | `gemini335le` | камера на запястье: Orbbec 335Le или `d435i` (старая RealSense, топики `/camera/...`) |
| `gripper_camera_preview` | `false` | камера запястья 640×400 вместо 1280×800 |
| `front_camera` | `true` | передняя камера тележки (`/front_camera/...`, 640×400). `false` — меньше нагрузка, но Nav2 теряет облако столешниц |
| `arm_offset_xyz`, `arm_offset_rpy` | `0 0 0` | дополнительное смещение и поворот базы руки относительно штатной точки крепления, м и рад |
| служебные | см. `--show-args` | `gz_args`, `gz_version`, `ign_*`, `debugger`, `debug_env`, `on_exit_shutdown` — от `gz_sim.launch.py` |

**Что поднимается:** `gazebo`, `robot_state_publisher`, `joint_state_publisher`
(`joint_state_merger`: суставы руки + колёса), `clock_throttle`; мосты `gz_bridge`,
`gz_bridge_gripper_camera`, `gz_bridge_cart_sensors` (лидар, ToF), `gz_bridge_front_camera`;
облака камер (`gripper_camera_proc`, `front_camera_proc`); `lidar_blackout_filter`
(`/scan_raw` → `/scan` без стоек корпуса); контроллеры `joint_state_broadcaster`,
`xarm6_traj_controller` и `arm_hold_controller` (тормоз руки, спавнится выключенным);
`arm_parking`; `vacuum_gripper_sim`, `vacuum_rviz_button`, `scene_markers`, `grasp_analyzer`;
при `use_moveit:=true` — `move_group` и `moveit_world_tf`.

**Интерфейсы тележки:**

| Что | Топики и сервисы |
|---|---|
| Езда | `/cmd_vel` (`geometry_msgs/Twist`, без штампа), `/odom` |
| Лидар | `/scan` (без стоек), `/scan_raw` (сырой), `/scan/points` |
| ToF 1–8 | `/range1_pcl` … `/range8_pcl` (облака; никем не используются) |
| Передняя камера | `/front_camera/color/image_raw`, `/front_camera/aligned_depth/image_raw`, `/front_camera/points` |
| Камера запястья | `/gripper_camera/color/image_raw`, `.../aligned_depth/image_raw`, `.../points` |
| Рука | `/joint_states`, `/arm_parking/state` (`true` — запаркована), сервис `/arm_parking/set` |
| Вакуум | `/vacuum_gripper/{cmd,state,held,ready,contact,status,markers}`, сервис `/vacuum_gripper/set` |
| Планирование | действия `/move_action`, `/execute_trajectory`, `/xarm6_traj_controller/follow_joint_trajectory` |

**Проехать вручную:**

```bash
ros2 topic pub -r 10 /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2}}"        # вперёд 0.2 м/с
ros2 topic pub -r 10 /cmd_vel geometry_msgs/msg/Twist "{angular: {z: 0.5}}"       # поворот на месте
```

(`Ctrl+C` останавливает публикацию; тележка встанет.)

**Парковка руки:**

```bash
ros2 topic echo /arm_parking/state                                                  # true — на тормозе
ros2 service call /arm_parking/set std_srvs/srv/SetBool "{data: true}"             # запарковать
ros2 service call /arm_parking/set std_srvs/srv/SetBool "{data: false}"            # включить руку
ros2 control list_controllers      # при парковке: arm_hold_controller active, xarm6_traj_controller inactive
```

### 3.2. Б2. Рука на тележке, навигация (`cart_nav_gz`)

**Для чего:** автономная езда по целям в комнате 9×7 м (`worlds/cart_nav.world`): карта SLAM
(slam_toolbox), Nav2 (MPPI, costmap по лидару и передней камере), OctoMap по передней камере.
Подробное устройство, карты и неполадки — [NAV.md](NAV.md).

```bash
ros2 launch cart_xarm_bringup cart_nav_gz.launch.py                              # с окнами
ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true               # рекомендуется для езды
ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_octomap:=false  # ещё легче
```

| Аргумент | По умолчанию | Что делает и когда менять |
|---|---|---|
| `use_rviz` | `true` | RViz навигации (`cart_nav.rviz`, Fixed Frame `map`): карта, costmap, путь, кнопка **Nav2 Goal** |
| `use_moveit` | `false` | MoveIt для руки. Для езды не нужен и тяжёлый |
| `headless` | `false` | **Рекомендуется `true`**: окно Gazebo ест 0.6–0.75 ядра, вместе с RViz и OctoMap процессор забивается (2026-10-29). Тележку и карты смотрите в RViz |
| `use_octomap` | `true` | 3D-карта по передней камере (`octomap_server`). `false` — меньше нагрузка; Nav2 её не использует |
| `arm_initial_positions` | `1.5708 0.06 -0.48 0 0.47 0` | поза руки на время езды: «перевёрнутая U» поперёк тележки на стороне ручки, присоска в 1.4 см над верхней плитой, пик руки 1.50 м (прежняя поза давала 1.78 м и торчала за борт) |
| `arm_parked` | `true` | рука под тормозом с самого старта и не включается, пока нет запроса (сервис `/arm_parking/set`) |
| `manipulation_nodes` | `false` | ноды захвата выключены: при езде рука ничего не держит. `true` — вернуть все (вакуум, анализатор, облако камеры запястья) |
| `gripper_camera_preview` | `true` | камера запястья 640×400 (экономит GPU и процессор) |
| `spawn_x`, `spawn_y`, `spawn_yaw` | `-2.0`, `0.0`, `0.0` | где стартует тележка (м, рад) |
| проходят в `cart_xarm6_gz` | | `camera_model`, `spawn_z`, `arm_offset_xyz`, `arm_offset_rpy` — объявлены во вложенном launch и доходят до него |
| **не менять** | | `world` (нужен `cart_nav.world`), `front_camera`, а также `autostart`, `params_file`, `slam_params_file`, `use_lifecycle_manager`, `use_composition`, `namespace`, `graph`, `use_respawn`, `log_level`, `container_name`: их выставляет сам launch (`config/nav2_cart.yaml`, `config/slam_cart.yaml`) |

`robot_model:=c2` для навигации **не использовать**: контур тележки в `nav2_cart.yaml` задан под
c2l-v3 (при c2 Nav2 к тому же в моём пробном запуске не поднялся с первого раза).

**Порядок старта** (таймеры в launch): Gazebo и спавн тележки (4 с), мост TF одометрии (6 с),
SLAM, OctoMap и RViz (10 с), Nav2 и **страж запуска** (30 с). Nav2 готов, когда в терминале
появилась строка `lifecycle_manager_navigation: Managed nodes are active`. Страж
`nav_bringup_watchdog` через ~85 секунд после запуска проверяет SLAM и Nav2 и, если они не
поднялись (гонка при старте), сам делает сброс и запуск (до 4 раз); его сообщения в терминале
начинаются с `nav_bringup_watchdog`.

**Поставить цель.** В RViz кнопкой **Nav2 Goal** (щелчок — точка, протяжка — направление). Без RViz:

```bash
ros2 action send_goal /navigate_to_pose nav2_msgs/action/NavigateToPose \
  "{pose: {header: {frame_id: map}, pose: {position: {x: 0.0, y: -2.2}, orientation: {w: 1.0}}}}"
ros2 run nav2_map_server map_saver_cli -f ~/cart_map          # сохранить карту
```

**Что должно быть:** после `Managed nodes are active` кнопка **Nav2 Goal** принимает цели;
`ros2 topic echo --once /map --field info.width` даёт 179 (карта комнаты); цель из примера
выполняется (`status: SUCCEEDED`); `ros2 topic echo /arm_parking/state` — `true`.

**Рука при езде.** В Б2 она запаркована тормозом в позе «U». Чтобы работать рукой: снять
парковку (`/arm_parking/set {data: false}`), двигать, затем вернуть в позу и запарковать.
Автоматического возврата в «U» нет.

---

## 4. Распознавание объектов (OpenVINO)

Платформа тележки — Intel NUC, поэтому движок OpenVINO. Подробности, замеры скорости и
ограничения — [RECOGNITION_OPENVINO.md](RECOGNITION_OPENVINO.md). Здесь — как это включается.

### 4.1. Установка (один раз)

```bash
python3 -m venv --system-site-packages ~/.venvs/openvino
~/.venvs/openvino/bin/pip install openvino
M=~/.local/share/quasi/models/yolox; mkdir -p $M && cd $M
for n in yolox_nano yolox_tiny yolox_s; do
  curl -sLO https://github.com/Megvii-BaseDetection/YOLOX/releases/download/0.1.1rc0/$n.onnx; done
cd ~/ros2k_ws && colcon build --symlink-install --packages-select xarm_grasping_msgs xarm_grasping cart_xarm_bringup
```

Модель — YOLOX на 80 бытовых классов COCO. **Зелёные коробки и цилиндры сцены она не узнаёт**
(в симуляции детекций не будет): для них нужна своя обученная модель.

### 4.2. В сценарии А

```bash
ros2 launch cart_xarm_bringup arm_table_gz.launch.py detector:=true
ros2 topic hz /detections                       # ≈ 2 Гц (max_rate_hz)
ros2 topic echo /objects --field objects | grep -E "id=|detector_label="
```

Параметры (устройство `CPU|GPU|NPU|AUTO`, число потоков, порог, частота) —
`xarm_grasping/config/openvino_detector.yaml`. На этом ПК выбирать `CPU`: `GPU` здесь — это
NVIDIA, и OpenVINO на ней в 8 раз медленнее процессора; `GPU` и `NPU` проверяют на NUC.

### 4.3. В сценарии Б

В Б1/Б2 конвейера объектов нет, но детектор можно запустить отдельно и смотреть рамки в
`/detections` и `/detections/image`:

```bash
~/.venvs/openvino/bin/python ~/ros2k_ws/install/xarm_grasping/lib/xarm_grasping/openvino_detector.py \
  --ros-args --params-file ~/ros2k_ws/install/xarm_grasping/share/xarm_grasping/config/openvino_detector.yaml \
  -p image_topic:=/front_camera/color/image_raw
```

---

## 5. Как повторить мои проверки

Скрипты лежат в `src/r2_robot/cart_xarm_bringup/tools/` и запускаются по пути (не устанавливаются).
Все они проверены на живых симуляциях 2026-11-02; в таблице — результат, который получил я.

| № | Что проверяет | Как запустить | Ожидаемо |
|---|---|---|---|
| 1 | OpenVINO, сопоставление рамок с кластерами (синтетика), детекция на фото; **без ROS и симуляции** | `~/.venvs/openvino/bin/python tools/check_recognition.py --model ~/.local/share/quasi/models/yolox/yolox_tiny.onnx --image ~/.local/share/quasi/test_images/000000039769.jpg` | все пункты `ок`, найдены 2 кошки, пульт, кровать; ≈ 40 мс на кадр; код возврата 0 |
| 2 | Нода `openvino_detector` на картинке, без симуляции | в одном окне `python3 tools/publish_test_image.py фото.jpg`, в другом нода (§4.3 с `image_topic` по умолчанию), смотреть `ros2 topic echo /detections --field detections` | рамки с классами и score, ≈ 2 Гц |
| 3 | Привязка класса к кластеру облака на реальной геометрии камеры | окно 1: `ros2 launch cart_xarm_bringup arm_table_gz.launch.py detector:=stub use_moveit:=false headless:=true use_rviz:=false`; через ~30 с окно 2: `python3 tools/stub_detections.py --duration 30` | в `/objects` у каждого из 8 объектов `detector_label` = `stub_<id>` (свой), перепутанных нет |
| 4 | Колёсный привод: проскальзывание и наклон | окно 1: `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py headless:=true use_rviz:=false use_moveit:=false manipulation_nodes:=false world:=$(ros2 pkg prefix cart_xarm_bringup)/share/cart_xarm_bringup/worlds/cart_nav.world spawn_x:=-3.0 spawn_y:=-0.5`; через ~50 с окно 2: `python3 tools/drive_traction_probe.py` | четыре строки: проскальзывание 0%, тангаж 0.0°, крен 0.0° |
| 5 | Навигация и устойчивость (то, что раньше опрокидывало тележку) | окно 1: `ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_rviz:=false use_octomap:=false`; когда Nav2 active, окно 2: `python3 tools/nav_tilt_probe.py` (по умолчанию 4 цели, ≈ 6 мин; быстро: `--goals "0.15,1.76;3.44,1.15"`) | у каждой цели статус 4 («выполнена»), наклон ≈ 0.0°, суставы руки неизменны; в конце `ИТОГ: устойчиво` |

Пути к скриптам: `python3 ~/ros2k_ws/src/r2_robot/cart_xarm_bringup/tools/<имя>.py`. Каждый
запускается с `--help`.

**Остальные мои проверки** (каждая — просто команда после запуска Б1/Б2):

```bash
ros2 control list_controllers                 # Б2: arm_hold_controller active, xarm6_traj_controller inactive
ros2 topic echo --once /arm_parking/state     # Б2: data: true
timeout 20 ros2 run tf2_ros tf2_echo base_footprint link_tcp   # Б2: присоска x≈0.11 y≈0.22 z≈0.95
ros2 service call /lifecycle_manager_navigation/is_active std_srvs/srv/Trigger   # Б2: success=True
```

---

## 6. Нагрузка на ПК

Измерено на этом ПК (12 потоков): без окон и RViz навигация идёт при ≈ 500% CPU из 1200%; с
окнами Gazebo и RViz и OctoMap процессор забивается (простой 0.5%).

| Что сделать | Эффект | Цена |
|---|---|---|
| `headless:=true` | убирает окно Gazebo: −0.6…0.75 ядра | тележку смотреть в RViz |
| `use_octomap:=false` (Б2) | убирает обработку облака передней камеры 15 раз в секунду | нет 3D-карты |
| `use_moveit:=false` | без `move_group` и RViz MoveIt | нет `/pick_place`, нет планирования |
| `gripper_camera_preview:=true` | облако запястья в 4 раза легче | кадр для распознавания хуже |
| `front_camera:=false` (Б1) | без передней камеры и облака | Nav2 не видит столешницы |
| `manipulation_nodes:=false` | без 4 python-нод и облака камеры запястья | нет вакуума и анализатора |
| детектор: `num_threads: 2`, `max_rate_hz: 2` | ≈ 0.5 ядра вместо 1.7 | реже кадры |

Дополнительно: закрыть тяжёлые приложения (браузер) на время замеров; частота `/clock` и тип
исполнителя у Python-нод — в плане работ (`PLAN_2026-10-30.md`, `TASK_python_nodes_cpu.md`).

---

## 7. Готовые рецепты

| Задача | Команда |
|---|---|
| Проверить, что камера и облако работают | `ros2 launch cart_xarm_bringup arm_table_gz.launch.py use_moveit:=false` → `ros2 topic hz /gripper_camera/points` |
| Отладить захват без тяжёлого MoveIt | `ros2 launch cart_xarm_bringup arm_table_gz.launch.py use_moveit:=false`; руку двигать `arm_goto_tcp.py`, вакуум — сервисом `/vacuum_gripper/set` |
| Полный pick&place | `ros2 launch cart_xarm_bringup arm_table_gz.launch.py` → `ros2 action send_goal /pick_place ...` (§2.6) |
| Лёгкий режим при медленной симуляции | `... arm_table_gz.launch.py use_moveit:=false headless:=true gripper_camera_preview:=true` |
| Тележка с рукой, всё ручками | `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false` + `ros2 topic pub -r 10 /cmd_vel ...` |
| Навигация, самый лёгкий режим | `ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_octomap:=false` |
| Навигация со всеми окнами | `ros2 launch cart_xarm_bringup cart_nav_gz.launch.py` (нагрузка высокая) |
| Другая поза руки на тележке | добавить `arm_initial_positions:="0 -0.6 -1.0 0 1.6 0"` к запуску Б1/Б2 |
| Тележка в другом месте | `spawn_x:=-2.0 spawn_y:=1.0 spawn_yaw:=1.57` |
| Рука не двигается (тормоз) | `ros2 service call /arm_parking/set std_srvs/srv/SetBool "{data: false}"` |
| Попробовать распознавание без симуляции | §5, проверки 1 и 2 |
| Вторая симуляция параллельно | `export ROS_DOMAIN_ID=77 GZ_PARTITION=my_test` в каждом терминале второй |

---

## 8. Неполадки

| Что видно | Причина | Что делать |
|---|---|---|
| Сервисы и действия «не находятся», `Waiting for service ...`, `failed to send response ... timeout`, Nav2 или SLAM не поднялись | накопился мусор Fast DDS в `/dev/shm` после аварийно остановленных запусков | остановить всё, `fastdds shm clean`, запустить заново |
| Тележка не едет, `TF_OLD_DATA`, падают `spawner` | висит `gz sim server` от прошлого запуска (две тележки и двое часов) | `pgrep -a gz`, `kill <pid>`, перезапуск |
| `bt_navigator` не нашёл `follow_path`/`wait`/`compute_path_to_pose` | гонка при старте | страж запуска сам перезапустит (до 4 раз, первая проверка через ~85 с); вручную: `manage_nodes {command: 3}`, затем `{command: 0}` (см. NAV.md) |
| Нет `/clock`, всё замерло | не установлен `topic_tools` | `sudo apt install ros-kilted-topic-tools` |
| `Unknown topic '/objects'` в консоли | рано: объекты появляются после первого облака | подождать 10–20 с и повторить |
| `ros2 topic echo` из другого терминала ничего не видит | другой `ROS_DOMAIN_ID` или `GZ_PARTITION` | выставить те же значения, что у запуска |
| Рука не реагирует на MoveIt, `/pick_place` отклоняет | в Б: рука на тормозе | `/arm_parking/set {data: false}` |
| При старте MoveIt `Could not find the planner configuration 'RRTConnect'` | известный шум конфигурации OMPL | игнорировать (pick&place при нём проходил) |
| `detector:=true`: нода падает, `No module named 'openvino'` | не установлено окружение или запуск не его Python | §4.1; launch использует `~/.venvs/openvino/bin/python` |
| Детектор берёт 150–170% CPU | не ограничено число потоков | `num_threads: 2` в `openvino_detector.yaml` |
| Тележка завалилась или подпрыгивает | в симуляции колёса — `DiffDrive`, коллизии — цилиндры, рука на тормозе; старые причины — STATUS.md (K10) | `ros2 control list_controllers`: при парковке `arm_hold_controller` active; проверки 4 и 5 из §5 |
| Процессор забит | окна Gazebo и RViz + OctoMap | §6 |

---

## 9. Что не проверено

- Запуск **с окнами Gazebo и RViz на вашем ПК** после сегодняшних изменений: я проверял без экрана.
  Поза руки, вид «U» и присоска над плитой — только по числам, глазами не смотрел.
- Проброс `spawn_z`, `arm_offset_xyz/rpy` из навигации во вложенный launch (`camera_model` проверен).
- `robot_model:=c2` в Б1 и Б2 (в Б2 его использовать нельзя, см. §3.2).
- Остальные модели тележки (`c2m-v3`, `c2t-v3`, `c2w-v3`, `c2h-v3`): расстояние между колёсами
  выставлено по суставам, радиус 0.065 м предположен одинаковым.
- Устройства `GPU`, `NPU`, `AUTO` OpenVINO и INT8 — только на NUC.
- Класс по картинке в `/pick_place` не используется, только публикуется.
- Свои обученные модели для коробок и пробирок: нет модели и датасета.
- Естественный сбой старта Nav2 со стражем: проверен на искусственной поломке, а настоящий сбой с
  ним пока не наблюдался.
