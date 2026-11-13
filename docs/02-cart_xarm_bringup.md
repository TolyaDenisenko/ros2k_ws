# Глава 2. Пакет `cart_xarm_bringup` — как всё запускается

> Пакет лежит в [`src/r2_robot/cart_xarm_bringup`](../src/r2_robot/cart_xarm_bringup).
> Здесь почти нет «логики робота» — в основном **сборка системы**: какие
> программы стартовать, с какими параметрами, в каком мире и как их соединить.
> Исключения — три небольшие служебные ноды в `scripts/` и инструменты проверки в `tools/`.
>
> Как запускать каждый сценарий, все аргументы и проверки «запуск удался» —
> в [RUN_SCENARIOS.md](RUN_SCENARIOS.md). Эта глава объясняет, **как устроены** launch-файлы.

## 1. Зачем этот пакет и как он связан с остальными

Слово *bringup* в ROS означает «поднять робота»: запустить всё нужное одной командой.
Этот пакет и есть такая «кнопка включения» для всех симуляций проекта:

| Launch-файл | Сценарий |
|---|---|
| `cart_xarm6_gz.launch.py` | тележка `quasi_cart` + рука xArm6 (или тележка без руки, `arm:=false`) в Gazebo |
| `cart_xarm6_rviz.launch.py` | только RViz: проверка модели тележки с рукой без Gazebo |
| `arm_table_gz.launch.py` | рука без тележки на столе, 7 предметов, конвейер pick & place ([глава 8](08-рука-на-столе.md)) |
| `cart_nav_gz.launch.py` | навигация тележки: Nav2 + SLAM + OctoMap + метка ArUco ([NAV.md](NAV.md)) |

Пакет сам ничего не описывает, а **берёт** готовое из других пакетов:

| Откуда | Что берёт |
|---|---|
| `quasi_cart_description` | URDF/xacro: `cart_with_arm.urdf.xacro` (тележка с рукой или без), `arm_on_table.urdf.xacro` ([глава 1](01-quasi_cart_description.md)) |
| `xarm_description`, `quasi_camera_description` | рука xArm6 и камеры (подключаются внутри xacro, не напрямую отсюда) |
| `ros_gz_sim` | запуск Gazebo Sim и «спавн» модели в мир |
| `ros_gz_bridge` | мосты между топиками Gazebo и ROS 2 |
| `topic_tools` | `throttle` — прореживание часов `/clock` |
| `controller_manager` / `xarm_controller` | контроллеры суставов руки (ros2_control) |
| `xarm_moveit_config` | MoveIt — планировщик движений руки |
| `xarm_grasping` | ноды вакуума, парковки руки, анализа захвата, восприятия, pick & place ([глава 4](04-xarm_grasping.md)) |
| `depth_image_proc` | сборка облака точек из цвета + глубины |
| `nav2_bringup`, `slam_toolbox`, `octomap_server` | навигация (только `cart_nav_gz`) |

Короткий словарь (только то, что понадобится в этой главе):

- **Нода (node)** — отдельная программа ROS 2 (процесс или компонент), делающая одну задачу.
- **Топик (topic)** — именованный канал сообщений, например `/joint_states`. Одни ноды
  *публикуют* в топик, другие *подписываются*. Тип сообщения фиксирован (например
  `sensor_msgs/msg/Image`).
- **Сервис** — запрос-ответ (например `/vacuum_gripper/set`: «включи присоску» → «ок/нет»).
- **Launch-файл** — Python-скрипт, который описывает, какие ноды запустить, с какими
  параметрами и в каком порядке. Запускается командой `ros2 launch <пакет> <файл>`.
- **Gazebo Sim (gz)** — физический симулятор. У него **своя** система топиков (gz transport),
  не совпадающая с ROS 2. Поэтому нужен **мост** (`ros_gz_bridge`), который переливает
  сообщения gz ↔ ROS.
- **`use_sim_time`** — параметр ноды: «бери время не с часов компьютера, а из топика
  `/clock`». В симуляции время идёт медленнее/быстрее реального, и все ноды должны жить
  по одним часам — часам Gazebo. Если хоть одна нода живёт по системному времени, её
  сообщения кажутся остальным «из будущего» или «устаревшими».
- **TF** — дерево систем координат (кадров) робота: `base_link`, `link_base`, `link6`, …
  Его публикует `robot_state_publisher` по URDF и углам суставов.

## 2. Дерево пакета

```text
cart_xarm_bringup/
├── CMakeLists.txt                     # install() папок launch/config/worlds/rviz и трёх скриптов
├── package.xml                        # список зависимостей (exec_depend)
├── launch/
│   ├── cart_xarm6_gz.launch.py        # ГЛАВНЫЙ: Gazebo + тележка (+ рука) + мосты + MoveIt + RViz
│   ├── cart_xarm6_rviz.launch.py      # только RViz + ползунки суставов (проверка URDF, без Gazebo)
│   ├── arm_table_gz.launch.py         # рука на столе (без тележки), pick & place
│   └── cart_nav_gz.launch.py          # навигация: включает cart_xarm6_gz + Nav2, SLAM, OctoMap, ArUco
├── config/
│   ├── gz_bridge.yaml                 # общий мост: /clock_gz, колёса, /cmd_vel, /odom, вакуум pick_box, позы
│   ├── gz_bridge_cart_sensors.yaml    # датчики тележки: лидар /scan_raw, ToF /rangeN_pcl
│   ├── gz_bridge_front_camera.yaml    # передняя камера -> /front_camera/...
│   ├── gz_bridge_gripper_camera.yaml  # камера Orbbec на запястье -> /gripper_camera/...
│   ├── gz_bridge_d435i.yaml           # старая камера RealSense D435i -> /camera/...
│   ├── gz_bridge_cart_nav.yaml        # навигация: TF одометрии /tf, /imu
│   ├── gz_bridge_arm_table.yaml       # мост сцены «рука на столе» (генерируется, глава 8)
│   ├── arm_table_objects.yaml         # 7 предметов сцены «рука на столе» (источник для мира и моста)
│   ├── arm_table_objects_8_old.yaml   # старый набор из 8 предметов (архив)
│   ├── arm_hold.yaml                  # контроллер-«тормоз» руки arm_hold_controller
│   ├── lidar_blackout.yaml            # сектора лидара, закрытые стойками тележки
│   ├── nav2_cart.yaml, slam_cart.yaml # параметры Nav2 и slam_toolbox (NAV.md)
├── worlds/
│   ├── cart_table_offline.world       # мир ПО УМОЛЧАНИЮ для тележки: пол, стол, pick_box, цилиндр
│   ├── cart_table.world               # старый мир с моделями из Fuel (нужен интернет)
│   ├── cart_nav.world                 # комната 9×7 м со стенами и препятствиями, стол, метка aruco_dock
│   ├── arm_table.world                # стол с 7 предметами (генерируется tools/gen_arm_table_scene.py)
│   └── materials/aruco_4x4_50_0.png   # текстура метки ArUco (tools/gen_aruco_marker.py)
├── scripts/                           # служебные ноды (ставятся в lib/, запускаются из launch)
│   ├── lidar_blackout_filter.py       # /scan_raw -> /scan без секторов стоек
│   ├── nav_bringup_watchdog.py        # страж запуска Nav2/SLAM (перезапуск, если не поднялись)
│   └── aruco_detector.py              # поза метки ArUco по передней камере
├── tools/                             # инструменты для человека: генераторы сцен, датасета, проверки
└── rviz/
    ├── cart_xarm.rviz                 # RViz без MoveIt
    ├── cart_xarm_moveit.rviz          # RViz с панелью MotionPlanning (MoveIt)
    └── cart_nav.rviz                  # RViz навигации (Fixed Frame map)
```

**CMakeLists.txt и package.xml.** Пакет типа `ament_cmake`, но ничего не компилирует:
[CMakeLists.txt](../src/r2_robot/cart_xarm_bringup/CMakeLists.txt) копирует папки
`launch config worlds rviz` в `install/share/cart_xarm_bringup` и ставит три скрипта из
`scripts/` в `install/lib/cart_xarm_bringup` (поэтому их можно запускать как
`executable='aruco_detector.py'`). Папка `tools/` **не** устанавливается: инструменты
запускают прямо из `src/` (`python3 src/r2_robot/cart_xarm_bringup/tools/<имя>.py`).
В [package.xml](../src/r2_robot/cart_xarm_bringup/package.xml) перечислены `exec_depend`
(всё, что запускается из launch).

**Про `--symlink-install`.** Если собирать `colcon build --symlink-install`, то в `install/`
лежат *ссылки* на файлы из `src/`. Значит правки существующих launch/world/yaml/rviz
вступают в силу сразу (перезапусти launch). **Новый** файл (например новый мир
`my.world` или новый скрипт) в `install/` сам не появится — нужна пересборка пакета;
новый скрипт ещё и надо дописать в `install(PROGRAMS …)`:

```bash
cd ~/ros2k_ws && colcon build --symlink-install --packages-select cart_xarm_bringup
source install/setup.bash
```

## 3. Главный launch: `cart_xarm6_gz.launch.py`

Файл: [launch/cart_xarm6_gz.launch.py](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py).
Запуск:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py              # тележка с рукой
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm:=false   # тележка без руки
```

### 3.1. Как устроен launch-файл на Python

Launch-файл обязан содержать функцию `generate_launch_description()`, которая возвращает
`LaunchDescription([...])` — список «действий»: объявить аргумент, запустить ноду,
включить другой launch, подождать N секунд и т.д.

Два ключевых понятия:

- **`DeclareLaunchArgument('имя', default_value=...)`** — объявляет аргумент, который
  можно передать из командной строки как `имя:=значение`.
- **`LaunchConfiguration('имя')`** — «ссылка» на значение аргумента. Это не строка, а
  *подстановка* (substitution): реальное значение подставится только в момент запуска.
  Поэтому их нельзя сравнивать обычным `if` в Python — для условий используют
  `PythonExpression` + `IfCondition`.

Пример из файла ([строки 55–68](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55)):

```python
spawn_x_arg = DeclareLaunchArgument('spawn_x', default_value='-0.5')
spawn_y_arg = DeclareLaunchArgument('spawn_y', default_value='0.0')
spawn_z_arg = DeclareLaunchArgument('spawn_z', default_value='0.07')
spawn_yaw_arg = DeclareLaunchArgument(
    'spawn_yaw', default_value='0.0',
    description='Ориентация тележки, рад; стол в мире при x=1.05 (верх z=0.925)')
...
arm_initial_positions_arg = DeclareLaunchArgument(
    'arm_initial_positions', default_value='1.5708 -0.9 -1.3 0 1.0 0',
    description='Начальные углы joint1..joint6 (рад) через пробел')
```

Условия выбора камеры ([строки 74–75](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L74)):

```python
is_orbbec = IfCondition(PythonExpression(["'", camera_model, "' != 'd435i'"]))
is_d435i = IfCondition(PythonExpression(["'", camera_model, "' == 'd435i'"]))
```

`PythonExpression` склеивает куски в строку `'gemini335le' != 'd435i'` и вычисляет её.
Ноды с `condition=is_orbbec` стартуют только для Orbbec.

Целую **группу** действий можно включить или выключить одним условием — `GroupAction`.
Так сделан аргумент `arm` ([строки 545–557](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L545)):
всё, что относится к руке, лежит в одной группе с `condition=IfCondition(arm)`.

### 3.2. Таблица всех launch-аргументов

| Аргумент | По умолчанию | Где объявлен | Что делает | Пример |
|---|---|---|---|---|
| `world` | `<share>/worlds/cart_table_offline.world` | [L43](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L43) | SDF-мир Gazebo (полный путь) | `world:=$(ros2 pkg prefix cart_xarm_bringup)/share/cart_xarm_bringup/worlds/cart_nav.world` |
| `use_moveit` | `true` | [L50](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L50) | поднять MoveIt и его RViz (только при `arm:=true`) | `use_moveit:=false` |
| `use_rviz` | `true` | [L491](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L491) | обычный RViz (открывается, если MoveIt не запущен) | `use_moveit:=false use_rviz:=false` |
| `headless` | `false` | [L492](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L492) | Gazebo без окна (только сервер) | `headless:=true` |
| `camera_model` | `gemini335le` | [L495](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L495) | камера на запястье: `gemini335le` \| `d435i` | `camera_model:=d435i` |
| `gripper_camera_preview` | `false` | [L498](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L498) | лёгкий режим камеры запястья 640x400 вместо 1280x800 | `gripper_camera_preview:=true` |
| `robot_model` | `c2l-v3` | [L502](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L502) | тип тележки (`<robot_model>.urdf.xacro`) | `robot_model:=c2` |
| `arm` | `true` | [L506](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L506) | `false` — **тележка без руки** (лёгкая симуляция для езды и парковки), см. 3.14 | `arm:=false` |
| `arm_parked` | `false` | [L510](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L510) | рука на тормозе с самого старта; снять: сервис `/arm_parking/set {data: false}` | `arm_parked:=true` |
| `manipulation_nodes` | `true` | [L514](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L514) | ноды захвата (вакуум, кнопка, `scene_markers`, `grasp_analyzer`, облако камеры запястья); `false` — ещё и `vacuum_targets:=none` | `manipulation_nodes:=false` |
| `front_camera` | `true` | [L519](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L519) | передняя камера тележки `/front_camera/...` | `front_camera:=false` |
| `spawn_x` | `-0.5` | [L55](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55) | X тележки в мире, м | `spawn_x:=-0.3` |
| `spawn_y` | `0.0` | [L56](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L56) | Y тележки в мире, м | `spawn_y:=0.2` |
| `spawn_z` | `0.07` | [L57](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L57) | высота спавна (чтобы колёса не провалились в пол) | `spawn_z:=0.1` |
| `spawn_yaw` | `0.0` | [L58](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L58) | поворот тележки вокруг Z, рад | `spawn_yaw:=1.5708` |
| `arm_initial_positions` | `1.5708 -0.9 -1.3 0 1.0 0` | [L66](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L66) | стартовые углы joint1..joint6, рад | `arm_initial_positions:="0 0 0 0 0 0"` |
| `arm_offset_xyz` | `0 0 0` | [L523](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L523) | доп. смещение базы руки от точки крепления, м | `arm_offset_xyz:="0.1 0 0"` |
| `arm_offset_rpy` | `0 0 0` | [L526](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L526) | доп. поворот базы руки, рад | `arm_offset_rpy:="0 0 3.1416"` |

По умолчанию `robot_model` — **`c2l-v3`**: так задано и в
[L69](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L69), и в
[L502](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L502), и в
`cart_xarm6_rviz.launch.py`. Старую тележку запускают через `robot_model:=c2`.

Посмотреть список аргументов можно без запуска:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py --show-args
```

### 3.3. Шаг 1 — URDF из xacro ([L90–106](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L90))

**xacro** — это XML с макросами; на выходе даёт **URDF** — описание звеньев и суставов робота.
Launch вызывает программу `xacro` прямо при запуске и передаёт ей аргументы:

```python
robot_description_content = Command([
    FindExecutable(name='xacro'), ' ', cart_xacro,
    ' use_sim:=true',
    ' camera_model:=', camera_model,
    ' gripper_camera_preview:=', LaunchConfiguration('gripper_camera_preview'),
    ' robot_model:=', robot_model,
    ' front_camera:=', front_camera,
    ' simulator:=ignition',
    ' with_arm:=', arm,
    ' vacuum_targets:="', PythonExpression(
        ["'pick_box' if '", LaunchConfiguration('manipulation_nodes'), "' == 'true' else 'none'"]), '"',
    ' xarm_initial_positions:="', LaunchConfiguration('arm_initial_positions'), '"',
    ' arm_offset_xyz:="', LaunchConfiguration('arm_offset_xyz'), '"',
    ' arm_offset_rpy:="', LaunchConfiguration('arm_offset_rpy'), '"',
])
```

Результат (длинная строка URDF) становится параметром `robot_description`. Здесь видно:

- launch-аргумент `arm` уходит в xacro как `with_arm` — при `false` в модели нет руки;
- `arm_initial_positions` уходит как `xarm_initial_positions` — дальше он попадает в
  `initial_value` плагина `gz_ros2_control`, поэтому рука *сразу* появляется в нужной позе
  (комментарий [L61–65](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L61));
- без нод захвата (`manipulation_nodes:=false`) коробку не к чему «приваривать»:
  `vacuum_targets:=none`. Иначе плагин приварил бы `pick_box` к руке при загрузке, а
  отпустить её было бы некому — тележка тащила бы коробку за собой.

### 3.4. Порядок запуска

Всё возвращается в `LaunchDescription` на [L487–558](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L487).
Часть действий обёрнута в `TimerAction` — запуск с задержкой
([L477–485](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L477)):

```python
spawn_delay = TimerAction(period=4.0, actions=[spawn_robot])
controllers_delay = TimerAction(
    period=7.0, actions=[spawner_jsb, spawner_traj, spawner_hold])
parking_delay = TimerAction(period=9.0, actions=[arm_parking])
analyzer_delay = TimerAction(period=10.0, actions=[grasp_analyzer])
moveit_delay = TimerAction(period=12.0, actions=[moveit_common])
```

Итоговая хронология («рука» — только при `arm:=true`, «захват» — только при
`manipulation_nodes:=true`):

| t, с | Что стартует | Строки | Условие |
|---|---|---|---|
| 0 | `SetParameter('use_sim_time', True)` — глобально для всех нод | [L488](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L488) | — |
| 0 | `worlds/` добавляется в `GZ_SIM_RESOURCE_PATH` (текстуры миров) | [L531](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L531) | — |
| 0 | Gazebo Sim с миром | [L112](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L112) | — |
| 0 | `joint_state_merger` + `robot_state_publisher` | [L137](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L137), [L151](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L151) | — |
| 0 | мост `gz_bridge` (общий) + `clock_throttle` | [L180](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L180), [L193](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L193) | — |
| 0 | мост датчиков тележки + `lidar_blackout_filter` | [L248](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L248), [L258](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L258) | — |
| 0 | мост передней камеры + облако `/front_camera/points` | [L265](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L265), [L274](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L274) | `front_camera` |
| 0 | RViz без MoveIt | [L413](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L413) | `use_rviz` и (`use_moveit!=true` или `arm!=true`) |
| 0 | мост камеры запястья Orbbec **или** D435i | [L201](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L201), [L210](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L210) | рука, `camera_model` |
| 0 | `depth_image_proc` → облако `/gripper_camera/points` | [L224](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L224) | рука, захват, только Orbbec |
| 0 | `vacuum_gripper_sim`, `scene_markers`, `vacuum_rviz_button` | [L330](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L330), [L305](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L305), [L294](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L294) | рука, захват |
| 0 | статический TF `link_base → world` | [L467](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L467) | рука, `use_moveit` |
| 4 | спавн робота `ros_gz_sim create` | [L161](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L161) | — |
| 7 | спаунеры `joint_state_broadcaster`, `xarm6_traj_controller`, `arm_hold_controller` (выключенным) | [L345](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L345) | рука |
| 9 | `arm_parking` (тормоз руки) | [L317](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L317) | рука |
| 10 | `grasp_analyzer` | [L381](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L381) | рука, захват |
| 12 | MoveIt (`_robot_moveit_common.launch.py`) + его RViz | [L427](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L427) | рука, `use_moveit` |

Задержки подобраны «на глаз»: на медленной машине Gazebo может не успеть подняться за 4 с,
и спавн/контроллеры упадут. Тогда увеличь `period` в этих строках.

### 3.5. Gazebo Sim ([L112–126](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L112))

Включается чужой launch `ros_gz_sim/launch/gz_sim.launch.py` (`IncludeLaunchDescription`).
Ему передаются флаги `gz sim`:

```python
'gz_args': [PythonExpression(
    ["'-r -v 3 -s --headless-rendering ' if '",
     LaunchConfiguration('headless'), "' == 'true' else '-r -v 3 '"]),
    LaunchConfiguration('world')],
'on_exit_shutdown': 'true',
```

- `-r` — сразу запустить симуляцию (не на паузе), `-v 3` — уровень логов.
- `headless:=true` добавляет `-s` (только сервер, без GUI) и `--headless-rendering`
  (рендер камер без окна) — камеры при этом продолжают работать.
- `on_exit_shutdown: true` — закрыл окно Gazebo → завершается весь launch.

Перед Gazebo launch добавляет папку `worlds/` пакета в переменную `GZ_SIM_RESOURCE_PATH`
([L531–532](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L531)). Так
Gazebo находит текстуры, которые миры указывают относительным путём, — например картинку
метки ArUco `materials/aruco_4x4_50_0.png` в `cart_nav.world`.

### 3.6. robot_state_publisher и слияние joint_states ([L128–158](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L128))

`robot_state_publisher` (rsp) читает URDF и углы суставов и публикует TF. Углы приходят
из двух мест: рука — `/joint_states` (от `joint_state_broadcaster`), колёса —
`/cart/wheel_joint_states` (из Gazebo через мост). Поэтому стоит промежуточная нода
`joint_state_publisher` с именем `joint_state_merger` в namespace `merger`:

```python
jsp = Node(
    package='joint_state_publisher',
    executable='joint_state_publisher',
    name='joint_state_merger',
    namespace='merger',
    parameters=[{
        'source_list': ['/joint_states', '/cart/wheel_joint_states'],
        'rate': 50,
        'use_sim_time': True,
    }],
    remappings=[('robot_description', '/robot_description')],
```

Она сливает оба источника в `/merger/joint_states`, а rsp подписан именно на него
(`remappings=[('joint_states', '/merger/joint_states')]`, [L156](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L156)).
**Ремап** (remapping) — переименование топика для конкретной ноды. Комментарий на
[L134–136](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L134)
объясняет, почему использован namespace, а не ремап: иначе нода слушала бы сама себя.

### 3.7. Спавн робота — точка появления тележки ([L161–174](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L161))

```python
spawn_robot = Node(
    package='ros_gz_sim',
    executable='create',
    arguments=[
        '-name', 'cart_xarm6',
        '-topic', 'robot_description',
        '-x', LaunchConfiguration('spawn_x'),
        '-y', LaunchConfiguration('spawn_y'),
        '-z', LaunchConfiguration('spawn_z'),
        '-Y', LaunchConfiguration('spawn_yaw'),
    ],
```

`create` берёт URDF из топика `/robot_description` (его публикует rsp) и создаёт в мире
модель с именем **`cart_xarm6`**. Это имя важно: на него завязаны мост поз
(`/cart_xarm6/pose` в `gz_bridge.yaml`) и параметр `robot_model_name` у `vacuum_gripper_sim`.
Не переименовывай его в одном месте.

**Как поменять точку спавна:**

1. Разово — аргументом: `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py spawn_x:=-0.2 spawn_yaw:=0.3`.
2. Навсегда — поменять `default_value` на [L55–59](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55).

Помни, что стол и `pick_box` стоят в *мировых* координатах: если отодвинешь тележку,
нос тележки может въехать в стол (ближний край стола x=0.45). Рука до `pick_box` (x=0.6)
с тележки сейчас **не достаёт** и при спавне по умолчанию: её база в ~1.1 м от коробки
(CLAUDE.md, «Gotchas»).

### 3.8. Мосты gz ↔ ROS и часы ([L176–218](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L176), [L245–273](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L245))

Несколько нод `ros_gz_bridge/parameter_bridge`, каждая со своим yaml (`config_file`):

| Нода | yaml | Когда |
|---|---|---|
| `gz_bridge` | `gz_bridge.yaml` | всегда |
| `gz_bridge_cart_sensors` | `gz_bridge_cart_sensors.yaml` | всегда |
| `gz_bridge_front_camera` | `gz_bridge_front_camera.yaml` | `front_camera:=true` |
| `gz_bridge_gripper_camera` | `gz_bridge_gripper_camera.yaml` | рука и `camera_model != d435i` |
| `gz_bridge_d435i` | `gz_bridge_d435i.yaml` | рука и `camera_model == d435i` |

Содержимое yaml разобрано в разделе 5.

**Часы.** Gazebo публикует время на каждом шаге физики — 1000 раз в секунду. Если отдать
это прямо в `/clock`, каждая из десятков нод с `use_sim_time` получает 1000 сообщений в
секунду и процессор уходит на часы. Поэтому мост кладёт часы в `/clock_gz`, а нода
`clock_throttle` (`topic_tools throttle`, [L193–198](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L193))
пересылает их в `/clock` с частотой 200 Гц. Сама она живёт по системному времени
(`use_sim_time: False`) — иначе ждала бы `/clock`, который сама же и публикует.

**Лидар.** Лидар тележки видит её собственные стойки. Мост отдаёт сырой скан как `/scan_raw`,
а `lidar_blackout_filter.py` ([L258–264](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L258))
публикует `/scan` без секторов стоек — по тем же углам `blackout_angles`, что у драйвера
ydlidar на реальной тележке (`config/lidar_blackout.yaml`).

### 3.9. Облака точек: depth_image_proc ([L220–243](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L220), [L274–291](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L274))

Для камер Orbbec облако строится не в Gazebo, а компонентом `depth_image_proc::PointCloudXyzrgbNode`
(как на реальном железе это делает драйвер). Для камеры запястья:

```python
remappings=[
    ('rgb/image_rect_color', '/gripper_camera/color/image_raw'),
    ('rgb/camera_info', '/gripper_camera/color/camera_info'),
    ('depth_registered/image_rect', '/gripper_camera/aligned_depth/image_raw'),
    ('points', '/gripper_camera/points'),
],
```

**Компонент** (composable node) — нода, загружаемая как плагин в общий процесс-контейнер
(`component_container_mt`); так меньше копирований больших картинок. Выход —
`/gripper_camera/points` в кадре `gripper_camera_color_optical_frame`. Облако камеры
запястья строится только при `manipulation_nodes:=true`. Точно так же контейнер
`front_camera_proc` строит `/front_camera/points` для передней камеры.

### 3.10. Вакуум в симуляции ([L328–342](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L328))

```python
vacuum_sim = Node(
    package='xarm_grasping',
    executable='vacuum_gripper_sim.py',
    name='vacuum_gripper_sim',
    respawn=True, respawn_delay=2.0,
    condition=has_manip,
    parameters=[{'use_sim_time': True,
                 'robot_model_name': 'cart_xarm6',
                 'object_names': ['pick_box'],
                 'object_sizes': [0.10, 0.05, 0.05]}],
```

Нода даёт сервис `/vacuum_gripper/set` (`std_srvs/SetBool`) и топик `/vacuum_gripper/cmd`,
проверяет касание присоски с коробкой по позам из `/gz/robot_pose` и `/gz/object_pose` и
шлёт `/vacuum/pick_box/attach` или `/vacuum/pick_box/detach` в плагин `DetachableJoint`
(он объявлен в `cart-arm.xacro`, глава 1). Это *логическая сварка*, а не физика присоски.
`object_sizes` должен совпадать с размером `pick_box` в мире (раздел 4). Если нода упала,
launch перезапустит её через 2 с (`respawn`). Подробнее — [глава 9](09-вакуум-кнопка-готовность.md).

### 3.11. Контроллеры руки и парковка ([L314–371](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L314))

`controller_manager` (часть **ros2_control** — фреймворка управления приводами) здесь живёт
*внутри* Gazebo, в плагине `gz_ros2_control` из URDF. Launch лишь вызывает `spawner`,
который загружает контроллеры:

- `joint_state_broadcaster` — публикует `/joint_states` руки;
- `xarm6_traj_controller` — исполняет траектории суставов (их шлёт MoveIt);
- `arm_hold_controller` — «тормоз»: держит запомненные углы руки. Создаётся **выключенным**
  (`--inactive`, параметры `config/arm_hold.yaml`).

Нода `arm_parking` (из `xarm_grasping`) переключает руку на тормоз, пока тележка едет
(`/cmd_vel` не ноль), и обратно. Вручную — сервис `/arm_parking/set` (`std_srvs/SetBool`),
состояние — `/arm_parking/state`. С `arm_parked:=true` рука на тормозе с самого старта.

⚠️ Нельзя «парковать» руку, просто выключив траекторный контроллер: рука без управления
складывается под своим весом и опрокидывает тележку. Поэтому и нужен отдельный
`arm_hold_controller`.

### 3.12. grasp_analyzer ([L373–405](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L373))

Топики и кадр камеры выбираются по `camera_model`:

| Параметр | Orbbec (по умолчанию) | d435i |
|---|---|---|
| `color_topic` | `/gripper_camera/color/image_raw` | `/camera/color/image_raw` |
| `points_topic` | `/gripper_camera/points` | `/camera/depth/points` |
| `camera_frame` | `gripper_camera_color_optical_frame` | `camera_color_optical_frame` |

Плюс `base_frame: link_base`, `analyze_period: 2.0` (с) и параметры сцены
([L400–402](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L400)):

| Параметр | Значение | Смысл |
|---|---|---|
| `table_height` | `0.015` | высота столешницы относительно `link_base`, м |
| `block_x`, `block_y` | `1.14`, `0.135` | запасная поза объекта в `link_base`, если облако не дало объект |

Откуда эти числа — объясняет комментарий над нодой
([L374–380](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L374)).
Для тележки `c2l-v3` при спавне в (-0.3, 0) `link_base` руки в мире находился примерно
в точке (-0.54, -0.135, 0.910) — это **измерено** по `/gz/robot_pose` и TF.
Дальше простое вычитание:

- `table_height` = 0.925 (верх стола) − 0.910 = 0.015;
- `block_x/y` = позиция `pick_box` (0.60, 0) − (−0.54, −0.135) = (1.14, 0.135).

⚠️ Обрати внимание: сейчас `spawn_x` по умолчанию `-0.5`, а числа посчитаны для `-0.3`,
поэтому `block_x` устарел на ~0.2 м (`table_height` от X не зависит). `block_*` — только
запасная поза, она используется лишь при `use_prior_pose:=true`. Если меняешь тележку,
точку спавна или крепление руки — пересчитай их.

### 3.13. RViz и MoveIt ([L407–474](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L407))

Одновременно открывается **только одно** окно RViz:

```python
condition=IfCondition(PythonExpression(
    ["'", use_rviz, "' == 'true' and ('", use_moveit, "' != 'true' or '", arm, "' != 'true')"])),
```

- `use_moveit:=true` (по умолчанию) и рука есть — RViz открывает MoveIt с конфигом `cart_xarm_moveit.rviz`.
- `use_moveit:=false` или `arm:=false` — обычный RViz с `cart_xarm.rviz`.

MoveIt включается из `xarm_moveit_config/launch/_robot_moveit_common.launch.py`
([L427–465](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L427)).
Ключевые аргументы:

- `use_sim_time: 'true'` — передаётся **явно**. Иначе общий launch ставит нодам `False`,
  move_group живёт по системным часам и не исполняет траектории (комментарий L446–449).
- `publish_attach_tf: 'false'` — MoveIt знает только руку (`world → link_base`). У нас
  `link_base` уже ребёнок тележки; второй родитель сломал бы TF. Вместо этого публикуется
  статический `link_base → world` (нода `moveit_world_tf`, [L467](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L467)).
- `add_vacuum_gripper: 'true'`, `add_gripper_camera` (корпус камеры Orbbec учитывается
  при планировании), `dof: '6'`, `ros2_control_plugin: 'gz_ros2_control/GazeboSimSystem'`.

⚠️ Обрати внимание: модель для MoveIt собирается отдельно (`uf_ros_lib/moveit_configs_builder.py`)
и **не знает** про тележку и `arm_initial_positions` — правки руки в URDF надо
дублировать там.

### 3.14. Тележка без руки: `arm:=false`

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm:=false
```

- В xacro уходит `with_arm:=false`: в модели нет руки, камеры запястья и вакуума (глава 1).
- `GroupAction` с `condition=IfCondition(arm)` ([L545–557](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L545))
  не запускает: мосты и облако камеры запястья, `vacuum_gripper_sim`, `scene_markers`,
  кнопку вакуума, контроллеры руки, `arm_parking`, `grasp_analyzer`, `moveit_world_tf` и MoveIt.
- Остаются: Gazebo, rsp, мосты тележки, часы, лидар с фильтром, передняя камера, RViz
  (`cart_xarm.rviz`).

Для чего: езда и парковка без лишней нагрузки. По замеру в навигации (headless, 20 с) без
руки — 27 процессов и 333 % ядра, с рукой — 30 процессов и 825 %; Nav2 без руки поднимается,
с рукой за 150 с не поднялся (STATUS.md, 2026-11-13). Поэтому в `cart_nav_gz` по умолчанию
`arm:=false`.

## 4. Мир `worlds/cart_table_offline.world`

Файл: [worlds/cart_table_offline.world](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world).
Формат **SDF** — XML-описание мира Gazebo. Все позы — `<pose>x y z roll pitch yaw</pose>`
в метрах/радианах, в системе мира.

| Объект | Строка | Поза | Размеры / заметки |
|---|---|---|---|
| `ground_plane` | [L10](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L10) | 0 | плоскость 20×20 м |
| `table_simple` | [L57](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L57) | `1.05 0.0 0` | столешница 1.2×0.8×0.05, центр z=0.9 ([L63](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L63)) → **верх z=0.925**; 4 ножки; `static` |
| `pick_box` | [L124](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L124) | `0.60 0.0 0.9505` | 0.10×0.05×0.05 м, **динамическая**, плагин `PosePublisher` 10 Гц |
| `cylinder_block` | [L159](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L159) | `0.80 -0.28 0.9705` | r=0.04, h=0.09, `static` |
| `physics` | [L174](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L174) | — | шаг 1 мс, RTF 1 |

Стол с центром x=1.05 и шириной 1.2 м: ближний край на x=0.45 — при спавне в 0,0 корпус c2l-v3
в него упирается, поэтому тележку ставят дальше (по умолчанию `spawn_x=-0.5`).
Высоты объектов посчитаны «верх стола + половина высоты»: `pick_box` 0.925+0.025+0.0005,
цилиндр 0.925+0.045+0.0005.

`pick_box` описан так ([L124–](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L124)):

```xml
<model name="pick_box">
  <pose>0.60 0.0 0.9505 0 0 0</pose>
  <plugin filename="gz-sim-pose-publisher-system" name="gz::sim::systems::PosePublisher">
    <publish_model_pose>true</publish_model_pose>
    ...
    <use_pose_vector_msg>true</use_pose_vector_msg>
    <update_frequency>10</update_frequency>
  </plugin>
```

Важно: коробка **не static** — иначе `DetachableJoint` не смог бы её унести. Имя
`pick_box` и link `link` зашиты в плагин (`cart-arm.xacro`, аргумент `vacuum_targets`),
в мост (`/vacuum/pick_box/*`, `/model/pick_box/pose` в `gz_bridge.yaml`) и в
`vacuum_gripper_sim` (`object_names` в launch). Переименовал — меняй во всех местах.

**Системные плагины мира** ([L189–](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L189)):

```xml
<plugin filename="gz-sim-physics-system" name="gz::sim::systems::Physics"/>
<plugin filename="gz-sim-user-commands-system" name="gz::sim::systems::UserCommands"/>
<plugin filename="gz-sim-scene-broadcaster-system" name="gz::sim::systems::SceneBroadcaster"/>
<plugin filename="gz-sim-sensors-system" name="gz::sim::systems::Sensors">
  <render_engine>ogre2</render_engine>
</plugin>
```

- `Physics` — физика; `UserCommands` — нужен для спавна (`create`) и команд из GUI;
  `SceneBroadcaster` — передаёт сцену в GUI.
- `Sensors` — рендер камер и лидара (без него камеры молчат). **Объявляется только в мире!**
  Модель собирается с `model_sensors_plugin:=false`; второй экземпляр Sensors даёт
  `Scene already exists` и segfault.

**Другие миры** (в launch передаются аргументом `world:=<полный путь>`):

- `cart_nav.world` — комната 9×7 м (стены, три препятствия), тот же стол с `pick_box` и
  `cylinder_block`, на торце стола — метка ArUco `aruco_dock` (картинку делает
  `tools/gen_aruco_marker.py`). Используется `cart_nav_gz` ([NAV.md](NAV.md)).
- `arm_table.world` — стол на высоте 0.75 м с 7 предметами для сценария «рука на столе».
  Не правится руками: его и мост `gz_bridge_arm_table.yaml` генерирует
  `tools/gen_arm_table_scene.py` из `config/arm_table_objects.yaml` ([глава 8](08-рука-на-столе.md)).
- `cart_table.world` — старый вариант: пол, солнце и стол тянутся из Gazebo Fuel по
  URL (нужен интернет), стол при x=1.7, нет `pick_box`. ⚠️ С ним вакуум работать не будет.

## 5. Конфиги мостов `config/gz_bridge*.yaml`

Каждая запись — один топик: `ros_topic_name`, `gz_topic_name`, типы с обеих сторон и
`direction`: `GZ_TO_ROS` (из симулятора в ROS) или `ROS_TO_GZ` (команды в симулятор).

### 5.1. [gz_bridge.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml) — общий

| ROS-топик | gz-топик | Тип ROS | Направление | Кто использует |
|---|---|---|---|---|
| `/clock_gz` | `/clock` | `rosgraph_msgs/Clock` | gz→ROS | `clock_throttle` → `/clock` (200 Гц) |
| `/cart/wheel_joint_states` | `/model/cart/wheel_joint_states` | `sensor_msgs/JointState` | gz→ROS | `joint_state_merger` |
| `/vacuum/pick_box/attach` | `/vacuum/pick_box/attach` | `std_msgs/Empty` | ROS→gz | `vacuum_gripper_sim` |
| `/vacuum/pick_box/detach` | `/vacuum/pick_box/detach` | `std_msgs/Empty` | ROS→gz | `vacuum_gripper_sim` |
| `/vacuum/pick_box/state` | `/vacuum/pick_box/state` | `std_msgs/String` | gz→ROS | «attached»/«detached» |
| `/gz/robot_pose` | `/cart_xarm6/pose` | `tf2_msgs/TFMessage` | gz→ROS | `vacuum_gripper_sim` |
| `/gz/object_pose` | `/model/pick_box/pose` | `tf2_msgs/TFMessage` | gz→ROS | `vacuum_gripper_sim` |
| `/cmd_vel` | `/model/cart/cmd_vel` | `geometry_msgs/Twist` | ROS→gz | езда тележки (`DiffDrive`) |
| `/odom` | `/model/cart/odom` | `nav_msgs/Odometry` | gz→ROS | одометрия тележки |

Важные решения, записанные в комментариях:

- `/clock` **не** мостится напрямую ([L3–6](../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml#L3)) — см. 3.8.
- `/joint_states` **не** мостится ([L13–14](../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml#L13)) —
  его публикует `joint_state_broadcaster`; мост дал бы второго издателя.
- `/gz/robot_pose` и `/gz/object_pose` — **не** `/tf` ([L51–55](../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml#L51));
  полный `/world/.../pose/info` не мостится из-за нагрузки на CPU.

### 5.2. [gz_bridge_cart_sensors.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge_cart_sensors.yaml) — датчики тележки

| ROS-топик | gz-топик | Что |
|---|---|---|
| `/scan_raw` | `/model/cart/lidar` | сырой скан лидара (`LaserScan`); `/scan` без стоек делает `lidar_blackout_filter` |
| `/scan/points` | `/model/cart/lidar/points` | облако лидара |
| `/range1_pcl` … `/range8_pcl` | `/model/cart/tof_sensorN/scan/points` | облака ToF 1–8 |
| `/imu` | `/model/cart/imu` | IMU тележки (`sensor_msgs/Imu`, 50 Гц) |

Имена — как на реальной тележке (`quasi_ros_c1`).

### 5.3. Камеры: [gz_bridge_front_camera.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge_front_camera.yaml) и [gz_bridge_gripper_camera.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge_gripper_camera.yaml)

| ROS-топик | gz-топик | Тип |
|---|---|---|
| `/<камера>/color/image_raw` | `/<камера>/gz/image` | `sensor_msgs/Image` |
| `/<камера>/aligned_depth/image_raw` | `/<камера>/gz/depth_image` | `sensor_msgs/Image` (32FC1, метры) |
| `/<камера>/color/camera_info` | `/<камера>/gz/camera_info` | `sensor_msgs/CameraInfo` |

`<камера>` — `front_camera` или `gripper_camera`. Все — `GZ_TO_ROS`. Имена на стороне ROS
выбраны такими же, какие даст реальный драйвер Orbbec, чтобы код не отличал симуляцию от
железа. Облака `/…/points` здесь **не** мостятся — их строит `depth_image_proc` (раздел 3.9).

### 5.4. [gz_bridge_d435i.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge_d435i.yaml) — старая RealSense

`/camera/color/image_raw`, `/camera/color/camera_info`, `/camera/depth/depth_info`,
`/camera/depth/points` (`PointCloudPacked` → `PointCloud2`), все `GZ_TO_ROS`, имена
gz и ROS совпадают. Оставлен для обратной совместимости (`camera_model:=d435i`).

### 5.5. [gz_bridge_cart_nav.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge_cart_nav.yaml) — навигация

`/tf` ← `/model/cart/tf` (TF одометрии `odom → base_footprint`) и `/imu` ← `/model/cart/imu`.
Запускается только из `cart_nav_gz`: в обычном сценарии TF `odom → base_footprint` в ROS
не публикуется.

## 6. `cart_xarm6_rviz.launch.py` — проверка модели без Gazebo

Файл: [launch/cart_xarm6_rviz.launch.py](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py).
Запускает только `robot_state_publisher`, `joint_state_publisher_gui` (окно с ползунками
суставов) и RViz с `cart_xarm.rviz`. Удобно проверять крепление руки и TF за секунды.

- xacro вызывается с `use_sim:=false` ([L51](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L51)).
- Аргументы: `use_gui` (`true`), `robot_model` (`c2l-v3`), `arm_offset_xyz`, `arm_offset_rpy` (`0 0 0`),
  `arm_initial_positions` ([L71–73](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L71)) —
  тот же аргумент, что и в gz-launch: шесть углов joint1…joint6 через пробел, рад.
- Его дефолт — константа `ARM_INITIAL_POSITIONS = '1.5708 -0.9 -1.3 0 1.0 0'`
  ([L25](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L25)).
- Ноды `joint_state_publisher(_gui)` создаются в функции `_joint_state_publishers`
  ([L28–40](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L28)), которую
  запускает `OpaqueFunction`. Зачем так: строку аргумента нужно разрезать на числа
  (`.perform(context).split()`), а это возможно только в момент запуска, когда значение
  уже известно. Углы передаются параметром `zeros` в виде `{'joint1': …, …}`
  (несмотря на имя — это стартовые значения).

Пример: `ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py arm_initial_positions:="0 0 0 0 0 0"`.

⚠️ Обрати внимание: дефолт `ARM_INITIAL_POSITIONS` **дублирует** дефолт `arm_initial_positions`
из gz-launch. Меняешь один — поменяй и второй, иначе RViz-проверка и Gazebo разойдутся.
Аргументов `camera_model`, `front_camera` и `arm` здесь нет — используются значения по
умолчанию из xacro (камера Orbbec, передняя камера есть, рука есть).

## 7. Другие launch-файлы пакета

Здесь — только устройство. Аргументы и проверки — в [RUN_SCENARIOS.md](RUN_SCENARIOS.md).

### 7.1. `arm_table_gz.launch.py` — рука на столе

Рука без тележки (`arm_on_table.urdf.xacro`) в мире `arm_table.world`, верх стола z = 0.75.
Список предметов (имена, формы, размеры) читается из `config/arm_table_objects.yaml` и
передаётся и в xacro (`vacuum_targets` — все 7 предметов), и в `vacuum_gripper_sim`. Кроме
вакуума запускаются восприятие объектов, сцена MoveIt, pick & place, команды по названию
(`pick_by_name`), осмотр с разных сторон (`survey_objects`) и, по желанию, детектор OpenVINO
(`detector:=true`). Подробно — [глава 8](08-рука-на-столе.md) и [PICK_PLACE.md](PICK_PLACE.md).

### 7.2. `cart_nav_gz.launch.py` — навигация

Файл [cart_nav_gz.launch.py](../src/r2_robot/cart_xarm_bringup/launch/cart_nav_gz.launch.py)
**включает** `cart_xarm6_gz.launch.py` (в `GroupAction`, чтобы его аргументы не перезаписали
свои) с миром `cart_nav.world` и добавляет: мост `gz_bridge_cart_nav.yaml`, slam_toolbox,
Nav2, OctoMap, страж запуска `nav_bringup_watchdog.py`, детектор метки `aruco_detector.py`
и свой RViz `cart_nav.rviz`.

Аргументы ([L142–174](../src/r2_robot/cart_xarm_bringup/launch/cart_nav_gz.launch.py#L142)):

| Аргумент | По умолчанию | Что делает |
|---|---|---|
| `arm` | **`false`** | тележка без руки (лёгкая симуляция); `true` — с рукой в позе `arm_initial_positions` на тормозе |
| `aruco` | `true` | `aruco_detector`: поза метки по передней камере, 5 Гц → `/aruco/marker_pose`, `/detected_dock_pose`, `/aruco/status`, `/aruco/image` |
| `use_rviz` | `true` | RViz навигации |
| `use_moveit` | `false` | MoveIt для руки |
| `headless` | `false` | Gazebo без окна (для езды рекомендуется `true`) |
| `use_octomap` | `true` | 3D-карта по передней камере |
| `arm_initial_positions` | `1.5708 0.06 -0.48 0 0.47 0` | поза руки на время езды: «перевёрнутая U» поперёк тележки |
| `arm_parked` | `true` | рука на тормозе с самого старта |
| `manipulation_nodes` | `false` | ноды захвата выключены (и `pick_box` не приваривается) |
| `gripper_camera_preview` | `true` | камера запястья 640×400 |
| `spawn_x`, `spawn_y`, `spawn_yaw` | `-2.0`, `0.0`, `0.0` | поза спавна в комнате |

Порядок: тележка сразу, мост навигации через 6 с, SLAM/OctoMap/RViz и детектор ArUco через
10 с, Nav2 и страж через 30 с. Подробно — [NAV.md](NAV.md).

### 7.3. Служебные ноды `scripts/`

| Скрипт | Где запускается | Что делает |
|---|---|---|
| `lidar_blackout_filter.py` | `cart_xarm6_gz` | `/scan_raw` → `/scan`: лучи в секторах стоек → `inf` (`config/lidar_blackout.yaml`) |
| `nav_bringup_watchdog.py` | `cart_nav_gz` | через 55 с после старта Nav2 проверяет, что SLAM и Nav2 активны; если нет — RESET + STARTUP (до 4 попыток) |
| `aruco_detector.py` | `cart_nav_gz` (`aruco:=true`) | метка ArUco 4×4_50 id 0 (сторона 0.16 м) на кадре передней камеры; поза по углам метки и плоскости глубины |

### 7.4. Инструменты `tools/`

Запускаются вручную (`python3 src/r2_robot/cart_xarm_bringup/tools/<имя>.py`), не из launch.
Генераторы: `gen_arm_table_scene.py` (мир и мост «руки на столе»), `gen_aruco_marker.py`
(картинка и модель метки). Датасет для нейросети: `capture_dataset.py`, `merge_datasets.py`
([MODEL_TRAINING.md](MODEL_TRAINING.md)). Проверки: `check_recognition.py`,
`check_survey_accuracy.py`, `check_aruco_accuracy.py`, `show_objects.py`, `nav_tilt_probe.py`,
`drive_traction_probe.py`, `test_pick_by_name.py`; заглушки: `stub_detections.py`,
`publish_test_image.py`, `fake_xarm_vacuum.py`. Описание каждого — в
[RUN_SCENARIOS.md](RUN_SCENARIOS.md), раздел «Как повторить мои проверки».

## 8. Конфиги RViz

- [rviz/cart_xarm.rviz](../src/r2_robot/cart_xarm_bringup/rviz/cart_xarm.rviz): Fixed Frame `base_link`;
  RobotModel (`/robot_description`), TF, MarkerArray `/xarm_grasping/grasp_markers`,
  `/scene_markers`, `/vacuum_gripper/markers`, `/objects/markers`, `/place_zones/markers`,
  Image `/gripper_camera/color/image_raw`, PointCloud2 `/gripper_camera/points`,
  Image `/gripper_camera/aligned_depth/image_raw`.
- [rviz/cart_xarm_moveit.rviz](../src/r2_robot/cart_xarm_bringup/rviz/cart_xarm_moveit.rviz): то же
  плюс `moveit_rviz_plugin/MotionPlanning`, `Trajectory` и интерактивные маркеры (кнопка
  вакуума); Fixed Frame `base_link`.
- [rviz/cart_nav.rviz](../src/r2_robot/cart_xarm_bringup/rviz/cart_nav.rviz): Fixed Frame `map`;
  карта, costmap, путь, передняя камера и её облако, кнопка **Nav2 Goal**.

⚠️ При `camera_model:=d435i` дисплеи камеры в `cart_xarm*.rviz` останутся пустыми — они
подписаны на `/gripper_camera/...`, а D435i публикует `/camera/...`.

## 9. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Поставить тележку в другое место (разово) | командная строка | `spawn_x/y/z/yaw` | `spawn_x:=-0.2 spawn_yaw:=0.3` |
| Поменять точку спавна по умолчанию | [cart_xarm6_gz.launch.py:55](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55) | `default_value` у `spawn_*` | `'-0.2'` |
| Тележка без руки | командная строка / [cart_xarm6_gz.launch.py:506](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L506) | `arm` | `arm:=false` |
| Навигация с рукой | командная строка / [cart_nav_gz.launch.py:164](../src/r2_robot/cart_xarm_bringup/launch/cart_nav_gz.launch.py#L164) | `arm` | `arm:=true` |
| Другая стартовая поза руки | [cart_xarm6_gz.launch.py:67](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L67) | `arm_initial_positions` | `'0 0 0 0 0 0'` |
| То же для RViz-проверки | [cart_xarm6_rviz.launch.py:25](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L25) | `ARM_INITIAL_POSITIONS` / аргумент `arm_initial_positions` | `arm_initial_positions:="0 0 0 0 0 0"` |
| Другая камера | [cart_xarm6_gz.launch.py:495](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L495) | `camera_model` | `camera_model:=d435i` |
| Ускорить камеру при низком RTF | [cart_xarm6_gz.launch.py:498](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L498) | `gripper_camera_preview` | `gripper_camera_preview:=true` |
| Gazebo без окна (SSH/CI) | [cart_xarm6_gz.launch.py:492](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L492) | `headless` | `headless:=true` |
| Другая тележка | [cart_xarm6_gz.launch.py:502](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L502) | `robot_model` | `robot_model:=c2` |
| Сдвинуть базу руки на тележке | [cart_xarm6_gz.launch.py:523](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L523) | `arm_offset_xyz/rpy` | `arm_offset_xyz:="0.05 0 0"` |
| Другой мир | [cart_xarm6_gz.launch.py:43](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L43) | `world` | `world:=/abs/path/my.world` |
| Без MoveIt / без RViz | [cart_xarm6_gz.launch.py:50](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L50) | `use_moveit`, `use_rviz` | `use_moveit:=false use_rviz:=false` |
| Только езда, без нод захвата | [cart_xarm6_gz.launch.py:514](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L514) | `manipulation_nodes` | `manipulation_nodes:=false` |
| Увеличить задержки старта | [cart_xarm6_gz.launch.py:477](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L477) | `period=` у `TimerAction` | `period=8.0` |
| Частота `/clock` | [cart_xarm6_gz.launch.py:195](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L195) | `'200.0'` у `clock_throttle` | не ниже 50 Гц (самый частый таймер нод) |
| Сектора лидара, закрытые стойками | [lidar_blackout.yaml](../src/r2_robot/cart_xarm_bringup/config/lidar_blackout.yaml) | `blackout_angles` | брать с реальной тележки, не «на глаз» |
| Высота стола / запасная поза объекта для grasp_analyzer | [cart_xarm6_gz.launch.py:400](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L400) | `table_height`, `block_x`, `block_y` | пересчитать при другой тележке/спавне |
| Частота анализа захвата | [cart_xarm6_gz.launch.py:399](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L399) | `analyze_period` | `1.0` |
| Передвинуть стол | [cart_table_offline.world:59](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L59) | `<pose>` модели `table_simple` | `1.10 0.0 0 0 0 0` |
| Передвинуть коробку | [cart_table_offline.world:125](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L125) | `<pose>` `pick_box` | `0.65 0.1 0.9505 0 0 0` |
| Изменить размер коробки | [cart_table_offline.world:147](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L147) + [launch:340](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L340) | `<size>` (visual+collision), `object_sizes`, инерция | синхронно в обоих файлах |
| Передвинуть цилиндр | [cart_table_offline.world:161](../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L161) | `<pose>` `cylinder_block` | `0.75 0.25 0.9705 0 0 0` |
| Предметы сцены «рука на столе» | [arm_table_objects.yaml](../src/r2_robot/cart_xarm_bringup/config/arm_table_objects.yaml) | список `objects` | затем `python3 src/r2_robot/cart_xarm_bringup/tools/gen_arm_table_scene.py` |
| Добавить мост нового топика | [gz_bridge.yaml](../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml) | новая запись `- ros_topic_name: ...` | см. формат раздела 5 |

Если меняешь высоту стола — пересчитай z у `pick_box` и `cylinder_block` и запиши изменение
в `docs/STATUS.md`.

## 10. Частые ошибки

1. **Добавили плагин Sensors в модель робота** → `Scene already exists`, segfault. Sensors — только в мире.
2. **Нода без `use_sim_time`** (особенно в чужих include) → TF «из будущего», MoveIt не
   исполняет траектории. В этом launch стоит глобальный `SetParameter` + явная передача в MoveIt.
3. **Замостили gz `/clock` прямо в `/clock`** → 1000 сообщений/с на каждую ноду, процессор
   занят часами, Nav2 не успевает подняться. Только через `/clock_gz` + `clock_throttle`.
4. **Создали новый `.world`, `.yaml` или скрипт и не пересобрали** → `file not found`. Новые файлы
   требуют `colcon build --packages-select cart_xarm_bringup` (скрипт — ещё и строку в `CMakeLists.txt`).
5. **Мир без `pick_box`** (например `cart_table.world`) → вакуум не работает, `vacuum_gripper_sim` не находит объект.
6. **Сим без `vacuum_gripper_sim`, но с плагином вакуума** → `pick_box` остаётся приваренным
   к руке, тележка тащит его. Без нод захвата используйте `manipulation_nodes:=false`
   (он же ставит `vacuum_targets:=none`).
7. **`pkill -f cart_xarm6`** в той же команде, где это слово встречается, убивает саму оболочку.
   Завершай launch через Ctrl+C.
8. **Gazebo не успел стартовать** → `create` или `spawner` падают с таймаутом. Увеличь задержки в `TimerAction`.
9. **Два издателя `/joint_states`** — не добавляй `/joint_states` в мост.
10. **Аргумент со пробелами без кавычек**: `arm_initial_positions:=0 0 0 0 0 0` не сработает —
    нужно `arm_initial_positions:="0 0 0 0 0 0"`.
11. **Текстура метки ArUco не видна (белый квадрат)** → Gazebo не нашёл `materials/…png`:
    мир запущен не через `cart_xarm6_gz.launch.py`, и `worlds/` нет в `GZ_SIM_RESOURCE_PATH` (не проверено).

## 11. Проверь себя

**Упражнение 1. Аргументы.** Выведи список аргументов и найди дефолт `robot_model` и `arm`.
Сравни с таблицей моделей в [CART_MODELS.md](CART_MODELS.md).

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py --show-args
```

**Упражнение 2. Спавн и поза.** Запусти без MoveIt, с другой точкой спавна и нулевой позой:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false \
  spawn_x:=-0.3 spawn_yaw:=0.2 arm_initial_positions:="0 0 0 0 0 0"
```

Во втором терминале проверь углы руки и что время идёт из симуляции:

```bash
ros2 topic echo /joint_states --once
ros2 topic hz /clock               # ≈ 200 Гц (после clock_throttle)
ros2 param get /robot_state_publisher use_sim_time
```

**Упражнение 3. Мосты.** При запущенной симуляции найди все топики камер и узнай их частоту.
Затем перезапусти с `camera_model:=d435i` и сравни список.

```bash
ros2 topic list | grep -E 'gripper_camera|front_camera|camera'
ros2 topic hz /gripper_camera/points
ros2 topic info /cmd_vel -v      # кто публикует, кто подписан (мост)
```

**Упражнение 4. Тележка без руки.** Запусти `arm:=false` и сравни `ros2 node list` с обычным
запуском. Каких нод нет? Есть ли `/joint_states`? Почему RViz открылся без MoveIt?

**Упражнение 5. Мир и вакуум.** Сдвинь `pick_box` в мире на y=0.1 (L125), перезапусти
(пересборка не нужна — файл существующий), проверь позу коробки и состояние присоски:

```bash
ros2 topic echo /gz/object_pose --once
ros2 topic echo /vacuum/pick_box/state --once
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
```

Без подвода TCP к коробке сервис должен отказать (нет касания) — почему? (Подсказка: раздел 3.10.)
