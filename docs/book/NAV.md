# Навигация тележки в симуляции: Nav2 + slam_toolbox + OctoMap

Первый, простой вариант навигации тележки c2l-v3 с рукой на штатных пакетах
ROS 2 Kilted. Стек тележки `quasi_ros_c1` (fuse, STVL, свой BT-навигатор,
стыковка, зоны) подключим позже. Этот вариант нужен, чтобы тележка с рукой
ездила по карте уже сейчас и чтобы было с чем сравнивать.

Содержание: [1. Установка и запуск](#1-установка-и-запуск) ·
[2. Мир](#2-мир) · [3. Цепочка данных](#3-цепочка-данных) ·
[4. Карты](#4-карты) · [5. Nav2: что и зачем настроено](#5-nav2-что-и-зачем-настроено) ·
[6. RViz](#6-rviz) · [7. Команды](#7-команды) · [8. Неполадки](#8-неполадки) ·
[9. Известное и планы](#9-известное-и-планы) · [10. Проверка](#10-проверка)

---

## 1. Установка и запуск

```bash
sudo apt install ros-kilted-navigation2 ros-kilted-nav2-bringup ros-kilted-slam-toolbox \
                 ros-kilted-octomap-server ros-kilted-octomap-rviz-plugins
cd ~/ros2k_ws && colcon build --symlink-install --packages-select quasi_cart_description cart_xarm_bringup
source install/setup.bash

pgrep -a gz            # должно быть пусто: старый сервер gz ломает TF (см. §8)
ros2 launch cart_xarm_bringup cart_nav_gz.launch.py
```

| Аргумент | По умолчанию | Что делает |
|---|---|---|
| `use_rviz` | `true` | RViz навигации `rviz/cart_nav.rviz` |
| `use_moveit` | `false` | MoveIt руки (для езды не нужен) |
| `headless` | `false` | Gazebo без окна |
| `use_octomap` | `true` | 3D-карта по передней камере |
| `gripper_camera_preview` | `true` | камера гриппера 640×400 вместо 1280×800 |
| `spawn_x` / `spawn_y` / `spawn_yaw` | `-2.0` / `0.0` / `0.0` | где появляется тележка |

Порядок старта (таймеры в `cart_nav_gz.launch.py`):

| Время | Что |
|---|---|
| 0 с | Gazebo, мосты датчиков, `robot_state_publisher` (из `cart_xarm6_gz.launch.py`) |
| 4 с | спавн тележки с рукой |
| 6 с | мост TF одометрии и IMU |
| 10 с | slam_toolbox, octomap_server, RViz |
| 30 с | Nav2 |

Цель ставится в RViz кнопкой **Nav2 Goal**: щелчок задаёт точку, протяжка —
направление. Nav2 готов, когда в терминале появилась строка
`lifecycle_manager_navigation: Managed nodes are active`.

## 2. Мир

`cart_xarm_bringup/worlds/cart_nav.world` — копия `cart_table_offline.world`
(стол, `pick_box`, `cylinder_block`) с комнатой и препятствиями. Кадр `map`
совпадает с координатами мира: одометрия gz отсчитывается от мира, и SLAM
начинает карту там же. Точку цели можно брать прямо из таблицы.

| Объект | Центр (x, y), м | Размер, м |
|---|---|---|
| Комната (стены 1.5 м) | x −4…5, y −3.5…3.5 | 9 × 7 |
| Стол `table_simple` | (1.05, 0) | столешница на 0.925 м |
| `obstacle_1` | (−2.2, 1.6) | 0.6 × 0.6 × 1.0 |
| `obstacle_2` | (2.6, −2.0) | 0.8 × 0.4 × 1.0 |
| `obstacle_3` | (−1.5, −2.2) | 0.4 × 1.2 × 1.0 |
| Тележка (спавн) | (−2.0, 0) | контур x −0.22…0.84, y ±0.32 от `base_footprint` |

## 3. Цепочка данных

### Датчики

| Датчик | Топик ROS | Где задан | Куда идёт |
|---|---|---|---|
| Лидар YDLIDAR TG15: 360°, 2000 лучей, 10 Гц, 0.38–30 м, высота 0.43 м, установлен перевёрнутым | `/scan_raw` → фильтр → `/scan` (кадр `lidar_link`) | `cart-sensors.ign.xacro`, мост `gz_bridge_cart_sensors.yaml` | SLAM, оба costmap, collision_monitor |
| Передняя камера Orbbec Gemini 335Le: 640×400, 15 Гц, глубина 0.25–6 м | `/front_camera/color/...`, `/front_camera/aligned_depth/...`, облако `/front_camera/points` | `cart-front-camera.xacro`, мост `gz_bridge_front_camera.yaml`, облако — `depth_image_proc` | оба costmap, OctoMap |
| Одометрия (gz OdometryPublisher) | `/odom`, TF `odom → base_footprint` | `cart-sensors.ign.xacro`, мост `gz_bridge_cart_nav.yaml` | всё |
| IMU | `/imu` | мост `gz_bridge_cart_nav.yaml` | пока никто (для fuse позже) |
| Напольные ToF 1–8 | `/range1_pcl` … `/range8_pcl` | мост `gz_bridge_cart_sensors.yaml` | пока никто (см. §9) |

### Стойки корпуса в поле лидара

Лидар видит собственные стойки тележки на 0.46 м. На реальной тележке их
вырезает драйвер ydlidar параметром `blackout_angles` (quasi_ros_c1:
`quasi_cart_bringup/param/ydlidar.yaml`). В симуляции драйвера нет, и то же
правило выполняет нода `cart_xarm_bringup/scripts/lidar_blackout_filter.py`.
Мост отдаёт сырой скан в `/scan_raw`, нода публикует `/scan`, лучи в секторах
становятся `inf`, как у драйвера с `invalid_range_is_inf: true`.

| Сектор | Границы, град | В симуляции стойка видна, град |
|---|---|---|
| FR | 25.3…37.5 | 30.0…35.6 |
| RR | 142.2…154.5 | 144.3…150.1 |
| RL | 205.5…217.8 | 209.9…215.7 |
| FL | 322.8…334.8 | 324.4…330.0 |

Сектора лежат в `config/lidar_blackout.yaml`. Для c2 и c2l-v3 в c1 своих
значений нет, действует общий набор. У c2m-v3 они свои
(`c2m-v3.quasi_common.yaml` в c1), их надо добавить, когда понадобится эта
модель. Набор симметричен относительно 0° и 180°, поэтому одинаково работает
в кадре драйвера и в `lidar_link` (лидар перевёрнут: rpy π 0 π).

### Дерево TF

```
map ─(slam_toolbox)→ odom ─(gz OdometryPublisher)→ base_footprint ─(robot_state_publisher)→ base_link
                                                                       ├─ lidar_link
                                                                       ├─ front_camera_* (передняя камера)
                                                                       ├─ tofN_link, imu_link
                                                                       └─ рука: link_base … link6, присоска, камера гриппера
```

### Команда скорости

```
bt_navigator → planner_server (путь) → controller_server (MPPI) ─cmd_vel_nav→
velocity_smoother ─cmd_vel_smoothed→ collision_monitor ─/cmd_vel→ мост gz → VelocityControl тележки
                                                                └→ arm_parking (рука «на тормозах», пока тележка едет)
```

`/cmd_vel` — обычный `geometry_msgs/Twist` без штампа времени
(`enable_stamped_cmd_vel: false`): его ждут мост gz и `arm_parking`.

## 4. Карты

Сейчас строятся три карты, все заново при каждом запуске.

### 4.1. 2D-карта SLAM: `/map`

- **Кто строит:** slam_toolbox в режиме `mapping` (online async) по `/scan`.
  Параметры — `config/slam_cart.yaml`.
- **Что публикует:** `/map` (OccupancyGrid, клетка 5 см, обновление раз в 2 с)
  и TF `map → odom`.
- **Ключевые параметры:** `base_frame: base_footprint`; `max_laser_range: 12`
  (с запасом для комнаты 9 м); `minimum_travel_distance` и
  `minimum_travel_heading` по 0.5 (новый скан попадает в граф после 0.5 м или
  0.5 рад движения); `do_loop_closing: true`.
- **Запуск:** в Kilted slam_toolbox — lifecycle-нода. Ею управляет отдельный
  `lifecycle_manager_slam` (autostart, `bond_timeout` 30 с). Штатный autostart
  из launch-файла slam_toolbox шлёт configure/activate один раз, и activate
  иногда терялся: не было ни карты, ни `map → odom`.
- **Цвета в RViz:** белое — свободно, чёрное — занято, серое — неизвестно.
  Серые клинья от тележки под ~±33° и ~±147° — сектора стоек, туда лидар не
  смотрит. Пока тележка едет и поворачивается, они заполняются.

### 4.2. Costmap Nav2: `/global_costmap/costmap` и `/local_costmap/costmap`

Costmap — карта «стоимостей» для планировщика и контроллера: где нельзя,
где нежелательно, где свободно.

| | Глобальная | Локальная |
|---|---|---|
| Кадр | `map`, вся карта | `odom`, окно 4 × 4 м вокруг тележки |
| Слои | `static_layer` (/map из SLAM) → `obstacle_layer` → `inflation_layer` | `voxel_layer` (3D, 16 слоёв по 8 см) → `inflation_layer` |
| Частота | обновление 1 Гц | обновление 5 Гц, публикация 2 Гц |
| Источники | `/scan` (до 5.5 м) и `/front_camera/points` (0.05–1.3 м над полом, до 3 м) | те же |
| Кто использует | планировщик NavFn | контроллер MPPI |

Камера нужна в costmap потому, что лидар стоит на 0.43 м и стол видит только
ножками. Столешницу (0.925 м) и всё, что выше лидара, даёт облако передней
камеры.

**Инфляция** (`inflation_radius 1.05`, `cost_scaling_factor 4`) — штраф за
близость к препятствию, а не запрет:

- **тёмная или красная кромка** — само препятствие;
- **голубая полоса** вокруг — сюда центр тележки ставить нельзя, тележка
  точно заденет;
- **розово-фиолетово-синий градиент** — стоимость спадает от препятствия до
  нуля на 1.05 м. Проезжать можно, планировщик просто выбирает путь подальше.

Радиус 1.05 м не может быть меньше описанного круга тележки (~1.0 м), иначе
MPPI с проверкой контура работает неверно. Если тележка слишком осторожничает в
узких местах, подними `cost_scaling_factor` до 6–8: радиус тот же, спад
круче, фиолетового меньше.

### 4.3. 3D-карта OctoMap: `/octomap_binary`, `/occupied_cells_vis_array`

- **Кто строит:** `octomap_server` по `/front_camera/points`.
- **Параметры (в launch):** кадр `map`, клетка 5 см, дальность 3 м, высота
  0.05–1.6 м (пол и потолок не нужны).
- **Зачем:** объёмная картина сцены (столешница, высокие предметы) для
  будущего планирования руки и подъезда к столу. Nav2 её пока не использует.
- В RViz — дисплей `OctoMap` (MarkerArray `/occupied_cells_vis_array`).
- Карта копит только то, что камера видела ближе 3 м (`sensor_model.max_range`)
  и на высоте 0.05–1.6 м: на старте — сотни вокселей (проверено: 323), после
  проезда по комнате — больше (1268 после одной цели, 2026-10-28). Воксели
  раскрашены по высоте, низкие — тёмно-синие; в виде сверху (`TopDownOrtho`)
  они сливаются с costmap — смотреть в виде `Orbit`.
- Выключить: `use_octomap:=false`.

### 4.4. Сохранение карты

```bash
# 2D-карта: картинка + yaml (для nav2 map_server / AMCL)
ros2 run nav2_map_server map_saver_cli -f ~/cart_map

# граф slam_toolbox (продолжить картирование или локализоваться потом)
ros2 service call /slam_toolbox/serialize_map slam_toolbox/srv/SerializePoseGraph \
  "{filename: '$HOME/cart_map'}"

# OctoMap (не проверено)
ros2 run octomap_server octomap_saver_node --ros-args -p octomap_path:=$HOME/cart_octomap.bt
```

**Загрузки сохранённой карты пока нет:** launch всегда строит карту заново.
Варианты на будущее: slam_toolbox в режиме `localization` с сохранённым графом
либо `map_server` + AMCL. Секция `amcl` в `nav2_cart.yaml` уже настроена на
`/scan` и `base_footprint`, но AMCL не запускается. Ни один вариант не
проверялся.

## 5. Nav2: что и зачем настроено

Основа — `navigation_launch.py` из nav2_bringup, без map_server и AMCL: карту и
`map → odom` даёт SLAM. Параметры лежат в `config/nav2_cart.yaml`, файл получен
из `nav2_bringup/params/nav2_params.yaml`, все отличия перечислены в его шапке.
При переходе на новую версию Nav2 возьми свежий `nav2_params.yaml` и повтори
правки по шапке.

| Узел | Настройка | Зачем |
|---|---|---|
| все | кадр робота `base_footprint` | центр шасси; `base_link` у нас не в центре |
| costmap | контур `[[-0.22,0.32],[-0.22,-0.32],[0.84,-0.32],[0.84,0.32]]` | c2l-v3 по мешу корпуса, как в c1 `c2l-v3.nav2.yaml` |
| costmap | без keepout- и speed-фильтров | их плейсхолдеры подставляет только `bringup_launch.py`; с `navigation_launch.py` они ломали запуск |
| costmap | `obstacle_min_range 0.23` | как в c1 |
| planner | NavFn, `allow_unknown: true`, допуск 0.5 м | можно планировать через неразведанные области |
| controller | MPPI: `vx` −0.2…0.4 м/с, `wz` ±0.6 рад/с, `CostCritic.consider_footprint: true` | проверяется весь прямоугольный контур, а не круг |
| controller | допуск цели 0.25 м и 0.25 рад | — |
| velocity_smoother | те же пределы скорости | — |
| collision_monitor | полигон `FootprintApproach`: контур из локального costmap, 1.2 с до столкновения, от 6 точек | замедляет и останавливает тележку перед препятствием по лидару |
| collision_monitor | `base_shift_correction: false`, `transform_tolerance 0.5`, `source_timeout 2` | с коррекцией по одометрии ловили «extrapolation into the future» и вечный стоп |
| bt_navigator | `wait_for_service_timeout 30 с`, `default_server_timeout 60` | при старте всей симуляции серверы поднимаются дольше штатной 1 с |
| lifecycle | `bond_timeout 30 с` (через `SetParameter` в launch) | `lifecycle_manager` из `navigation_launch` не читает params_file |
| запуск | без композиции (`use_composition: False`) | с композицией загрузка второго компонента зависала |

## 6. RViz

`rviz/cart_nav.rviz`, Fixed Frame `map`, вид сверху.

| Дисплей | Что показывает |
|---|---|
| Map (`/map`) | карта SLAM |
| Global / Local Costmap | стоимости (см. §4.2) |
| Path | путь планировщика |
| LaserScan (`/scan`) | точки лидара, уже без стоек |
| Polygon | контур тележки |
| FrontCamera / FrontCameraPoints | кадр и облако передней камеры |
| OctoMap | 3D-карта |
| Панель Navigation 2 | статус навигации, отмена цели |

Кадр передней камеры в RViz перевёрнут: камера висит вверх ногами (roll = π),
как на реальной тележке. Облако при этом ложится правильно.

## 7. Команды

```bash
# цель без RViz
ros2 action send_goal /navigate_to_pose nav2_msgs/action/NavigateToPose \
  "{pose: {header: {frame_id: map}, pose: {position: {x: 0.0, y: -2.2}, orientation: {w: 1.0}}}}"

# где тележка на карте
ros2 run tf2_ros tf2_echo map base_footprint

# фильтр стоек: в /scan_raw стойки есть, в /scan их нет
ros2 topic hz /scan

# ехать с клавиатуры (когда у Nav2 нет активной цели)
ros2 run teleop_twist_keyboard teleop_twist_keyboard

# состояние узлов
ros2 lifecycle get /slam_toolbox
ros2 lifecycle get /bt_navigator
ros2 topic echo /collision_monitor_state
```

## 8. Неполадки

| Что видно | Причина | Что делать |
|---|---|---|
| Тележка не едет, в логах `TF_OLD_DATA ignoring data from the past for frame base_footprint`, падают `spawner ...` | висит `gz sim server` от прошлого запуска: две тележки и двое часов в одном домене | `pgrep -a gz`, `kill <pid>`, перезапуск |
| Nav2 не поднялся: `"spin"` (`"wait"`, `"follow_path"`…) `action server not available after waiting for 30.00s`, `Failed to bring up all requested nodes`; простаивающие ноды по 30–50% CPU | служебный трафик обнаружения Fast DDS между ~50 процессами (поток `dds.asyn`, видно в `top -H -p <pid>`) | перезапустить; на Fast DDS после правок `/clock` и колёс (2026-10-28) в изолированных прогонах Nav2 поднимался. CycloneDDS проверен и не лучше (см. STATUS.md). Fast DDS Discovery Server не использовать: пропадают costmap |
| В RViz нет costmap (нет фиолетово-синих ореолов), хотя тележка ездит | костмапы публикуются, только если нода видит подписчиков; при Fast DDS Discovery Server — не видит | не включать Discovery Server (`ROS_DISCOVERY_SERVER` в окружении должен быть пустым) |
| Nav2 не поднялся: `compute_path_to_pose not available after waiting for 30.00s` | гонка старта: карта SLAM появилась поздно | перезапустить; если повторяется — сообщить |
| Нет карты и `map → odom` | slam_toolbox не активирован | `ros2 lifecycle get /slam_toolbox` должен дать `active`; смотреть лог `lifecycle_manager_slam` |
| Первая цель не принимается | Nav2 ещё поднимается (~30 с после старта) | дождаться `Managed nodes are active` |
| Тележка встала у препятствия | сработал collision_monitor (`/collision_monitor_state`) или нет пути | дать цель в обход, посмотреть costmap |
| В логе `Control loop missed its desired rate` | сим-время идёт не 1:1 с реальным | на езду не влияет |

## 9. Известное и планы

- Загрузки сохранённой карты нет (§4.4).
- ToF 1–8 в costmap не добавлены. В c1 они есть (`/rangeN_pcl` в `voxel_layer`).
- IMU никто не использует. В c1 одометрию и IMU сливает fuse — это следующий шаг.
- Гонка старта Nav2 и SLAM закрыта только тайм-аутами. Правильнее запускать
  Nav2 по событию «карта появилась», а не по таймеру 30 с.
- Переход на стек c1: fuse, STVL вместо `voxel_layer`, свой BT-навигатор,
  стыковка, зоны (`quasi_cart_zone_filter`), параметры по модели тележки
  (`<robot_model>.nav2.yaml`, `<robot_model>.quasi_common.yaml`).
- Подъезда к столу и захвата с тележки пока нет. Рука на тележке не достаёт
  до `pick_box` (см. CLAUDE.md).

## 10. Проверка

Изолированная headless-симуляция (`ROS_DOMAIN_ID=79`, `GZ_PARTITION=claude_nav`):

- **2026-10-28.** В `/scan_raw` 130 точек стоек ближе 0.8 м, в `/scan` — 0.
  Цель (0, −2.2) из (−2, 0) — `SUCCEEDED` с включённым collision_monitor,
  поза (0.04, −2.17). Передняя камера на исходной позе: глубина 1.3–5.9 м,
  облако ~130 тыс. точек. Процесс RViz стартует (окно не смотрели: нет экрана).
- **2026-10-28, ПК пользователя с GUI.** Тележка едет по цели из RViz.
- **Не проверено:** сохранение и загрузка карт, длинные
  маршруты с замыканием петли.
