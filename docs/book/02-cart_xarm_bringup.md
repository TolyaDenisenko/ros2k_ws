# Глава 2. Пакет `cart_xarm_bringup` — как всё запускается

> Пакет лежит в [`src/r2_robot/cart_xarm_bringup`](../../src/r2_robot/cart_xarm_bringup).
> Здесь нет ни одной строчки «логики робота» — только **сборка системы**: какие
> программы стартовать, с какими параметрами, в каком мире и как их соединить.

## 1. Зачем этот пакет и как он связан с остальными

Слово *bringup* в ROS означает «поднять робота»: запустить всё нужное одной командой.
Этот пакет и есть такая «кнопка включения» для симуляции тележки `quasi_cart` с
манипулятором xArm6, вакуумной присоской и камерой на запястье.

Он сам ничего не описывает и не вычисляет, а **берёт** готовое из других пакетов:

| Откуда | Что берёт |
|---|---|
| `quasi_cart_description` | URDF/xacro тележки с рукой: `urdf/cart_with_arm.urdf.xacro` (глава про описание робота) |
| `xarm_description`, `quasi_camera_description` | рука xArm6 и камера (подключаются внутри xacro, не напрямую отсюда) |
| `ros_gz_sim` | запуск Gazebo Sim и «спавн» модели в мир |
| `ros_gz_bridge` | мосты между топиками Gazebo и ROS 2 |
| `controller_manager` / `xarm_controller` | контроллеры суставов руки (ros2_control) |
| `xarm_moveit_config` | MoveIt — планировщик движений руки |
| `xarm_grasping` | ноды `grasp_analyzer.py` и `vacuum_gripper_sim.py` |
| `depth_image_proc` | сборка облака точек из цвета + глубины |

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
├── CMakeLists.txt                     # только install() папок launch/config/worlds/rviz
├── package.xml                        # список зависимостей (exec_depend)
├── launch/
│   ├── cart_xarm6_gz.launch.py        # ГЛАВНЫЙ: Gazebo + робот + мосты + MoveIt + RViz + grasp
│   └── cart_xarm6_rviz.launch.py      # только RViz + ползунки суставов (проверка URDF, без Gazebo)
├── config/
│   ├── gz_bridge.yaml                 # общий мост: /clock, колёса, /cmd_vel, /odom, вакуум, позы
│   ├── gz_bridge_gripper_camera.yaml  # мост камеры Orbbec Gemini 335Le -> /gripper_camera/...
│   └── gz_bridge_d435i.yaml           # мост старой камеры RealSense D435i -> /camera/...
├── worlds/
│   ├── cart_table_offline.world       # мир ПО УМОЛЧАНИЮ: пол, стол, pick_box, цилиндр, плагины
│   └── cart_table.world               # старый мир с моделями из Fuel (нужен интернет)
└── rviz/
    ├── cart_xarm.rviz                 # RViz без MoveIt
    └── cart_xarm_moveit.rviz          # RViz с панелью MotionPlanning (MoveIt)
```

**CMakeLists.txt и package.xml.** Пакет типа `ament_cmake`, но ничего не компилирует:
[CMakeLists.txt](../../src/r2_robot/cart_xarm_bringup/CMakeLists.txt) только копирует
папки `launch config worlds rviz` в `install/share/cart_xarm_bringup`. Именно оттуда их
находит `get_package_share_directory('cart_xarm_bringup')`. В
[package.xml](../../src/r2_robot/cart_xarm_bringup/package.xml) перечислены `exec_depend`
(всё, что запускается из launch). В описании пакета
([package.xml:8](../../src/r2_robot/cart_xarm_bringup/package.xml#L8)) указаны обе камеры:
Orbbec Gemini 335Le (по умолчанию) или D435i.

**Про `--symlink-install`.** Если собирать `colcon build --symlink-install`, то в `install/`
лежат *ссылки* на файлы из `src/`. Значит правки существующих launch/world/yaml/rviz
вступают в силу сразу (перезапусти launch). **Новый** файл (например новый мир
`my.world`) в `install/` сам не появится — нужна пересборка пакета:

```bash
cd ~/ros2k_ws && colcon build --symlink-install --packages-select cart_xarm_bringup
source install/setup.bash
```

## 3. Главный launch: `cart_xarm6_gz.launch.py`

Файл: [launch/cart_xarm6_gz.launch.py](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py).
Запуск:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py
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

Пример из файла ([строки 55–68](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55)):

```python
spawn_x_arg = DeclareLaunchArgument('spawn_x', default_value='-0.3')
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

Описание `spawn_yaw` ([строка 60](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L60))
напоминает, где стоит стол в мире по умолчанию: x=1.05, верх столешницы z=0.925.

Условия выбора камеры ([строки 74–75](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L74)):

```python
is_orbbec = IfCondition(PythonExpression(["'", camera_model, "' != 'd435i'"]))
is_d435i = IfCondition(PythonExpression(["'", camera_model, "' == 'd435i'"]))
```

`PythonExpression` склеивает куски в строку `'gemini335le' != 'd435i'` и вычисляет её.
Ноды с `condition=is_orbbec` стартуют только для Orbbec.

### 3.2. Таблица всех launch-аргументов

| Аргумент | По умолчанию | Где объявлен | Что делает | Пример |
|---|---|---|---|---|
| `world` | `<share>/worlds/cart_table_offline.world` | [L43](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L43) | SDF-мир Gazebo (полный путь) | `world:=$(ros2 pkg prefix cart_xarm_bringup)/share/cart_xarm_bringup/worlds/cart_table.world` |
| `use_moveit` | `true` | [L50](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L50) | поднять MoveIt и его RViz | `use_moveit:=false` |
| `use_rviz` | `true` | [L359](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L359) | обычный RViz (работает только при `use_moveit:=false`) | `use_moveit:=false use_rviz:=false` |
| `headless` | `false` | [L361](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L361) | Gazebo без окна (только сервер) | `headless:=true` |
| `camera_model` | `gemini335le` | [L364](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L364) | камера на фланце: `gemini335le` \| `d435i` | `camera_model:=d435i` |
| `gripper_camera_preview` | `false` | [L367](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L367) | лёгкий режим камеры 640x400 вместо 1280x800 | `gripper_camera_preview:=true` |
| `robot_model` | `c2l-v3-scaled` | [L371](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L371) | тип тележки (`<robot_model>.urdf.xacro`) | `robot_model:=c2` |
| `spawn_x` | `-0.3` | [L55](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55) | X тележки в мире, м | `spawn_x:=-0.3` |
| `spawn_y` | `0.0` | [L56](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L56) | Y тележки в мире, м | `spawn_y:=0.2` |
| `spawn_z` | `0.07` | [L57](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L57) | высота спавна (чтобы колёса не провалились в пол) | `spawn_z:=0.1` |
| `spawn_yaw` | `0.0` | [L58](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L58) | поворот тележки вокруг Z, рад | `spawn_yaw:=1.5708` |
| `arm_initial_positions` | `1.5708 -0.9 -1.3 0 1.0 0` | [L66](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L66) | стартовые углы joint1..joint6, рад | `arm_initial_positions:="0 0 0 0 0 0"` |
| `arm_offset_xyz` | `0 0 0` | [L375](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L375) | доп. смещение базы руки от точки крепления, м | `arm_offset_xyz:="0.1 0 0"` |
| `arm_offset_rpy` | `0 0 0` | [L378](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L378) | доп. поворот базы руки, рад | `arm_offset_rpy:="0 0 3.1416"` |

По умолчанию `robot_model` — **`c2l-v3-scaled`**: так задано и в
[L69](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L69), и в
[L371](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L371), и в
`cart_xarm6_rviz.launch.py`. Старую тележку запускают через `robot_model:=c2`.

Посмотреть список аргументов можно без запуска:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py --show-args
```

### 3.3. Шаг 1 — URDF из xacro ([L77–95](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L77))

**xacro** — это XML с макросами; на выходе даёт **URDF** — описание звеньев и суставов робота.
Launch вызывает программу `xacro` прямо при запуске и передаёт ей аргументы:

```python
robot_description_content = Command([
    FindExecutable(name='xacro'), ' ', cart_xacro,
    ' use_sim:=true',
    ' camera_model:=', camera_model,
    ' gripper_camera_preview:=', LaunchConfiguration('gripper_camera_preview'),
    ' robot_model:=', robot_model,
    ' simulator:=ignition',
    ' xarm_initial_positions:="', LaunchConfiguration('arm_initial_positions'), '"',
    ' arm_offset_xyz:="', LaunchConfiguration('arm_offset_xyz'), '"',
    ' arm_offset_rpy:="', LaunchConfiguration('arm_offset_rpy'), '"',
])
```

Результат (длинная строка URDF) становится параметром `robot_description`. Именно здесь
`arm_initial_positions` уходит в xacro как `xarm_initial_positions` — дальше он
попадает в `initial_value` плагина `gz_ros2_control`, поэтому рука *сразу* появляется в
нужной позе (комментарий [L61–65](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L61)).

### 3.4. Порядок запуска

Всё возвращается в `LaunchDescription` на [L355–394](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L355).
Часть действий обёрнута в `TimerAction` — запуск с задержкой
([L347–353](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L347)):

```python
spawn_delay = TimerAction(period=4.0, actions=[spawn_robot])
controllers_delay = TimerAction(
    period=7.0, actions=[spawner_jsb, spawner_traj])
analyzer_delay = TimerAction(period=10.0, actions=[grasp_analyzer])
moveit_delay = TimerAction(period=12.0, actions=[moveit_common])
```

Итоговая хронология:

| t, с | Что стартует | Строки | Условие |
|---|---|---|---|
| 0 | `SetParameter('use_sim_time', True)` — глобально для всех нод | [L356](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L356) | — |
| 0 | Gazebo Sim с миром | [L98](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L98) | — |
| 0 | `joint_state_merger` + `robot_state_publisher` | [L123](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L123), [L137](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L137) | — |
| 0 | мост `gz_bridge` (общий) | [L166](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L166) | — |
| 0 | мост камеры Orbbec **или** D435i | [L176](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L176), [L185](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L185) | `camera_model` |
| 0 | `depth_image_proc` → облако `/gripper_camera/points` | [L199](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L199) | только Orbbec |
| 0 | `vacuum_gripper_sim` | [L220](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L220) | — |
| 0 | RViz без MoveIt | [L288](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L288) | `use_rviz==true` и `use_moveit!=true` |
| 0 | статический TF `link_base → world` | [L337](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L337) | `use_moveit` |
| 4 | спавн робота `ros_gz_sim create` | [L147](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L147) | — |
| 7 | спаунеры `joint_state_broadcaster`, `xarm6_traj_controller` | [L232](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L232) | — |
| 10 | `grasp_analyzer` | [L257](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L257) | — |
| 12 | MoveIt (`_robot_moveit_common.launch.py`) + его RViz | [L302](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L302) | `use_moveit` |

Задержки подобраны «на глаз»: на медленной машине Gazebo может не успеть подняться за 4 с,
и спавн/контроллеры упадут. Тогда увеличь `period` в этих строках.

### 3.5. Gazebo Sim ([L98–112](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L98))

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
  (рендер камер без окна) — камера при этом продолжает работать.
- `on_exit_shutdown: true` — закрыл окно Gazebo → завершается весь launch.

### 3.6. robot_state_publisher и слияние joint_states ([L114–144](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L114))

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
(`remappings=[('joint_states', '/merger/joint_states')]`, [L142](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L142)).
**Ремап** (remapping) — переименование топика для конкретной ноды. Комментарий на
[L121–122](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L121)
объясняет, почему использован namespace, а не ремап: иначе нода слушала бы сама себя.

### 3.7. Спавн робота — точка появления тележки ([L147–160](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L147))

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
2. Навсегда — поменять `default_value` на [L55–59](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55).

Помни, что стол и `pick_box` стоят в *мировых* координатах: если отодвинешь тележку,
рука может перестать доставать до коробки (x=0.6), а нос тележки может въехать в стол
(ближний край стола x=0.45).

### 3.8. Мосты gz ↔ ROS ([L162–193](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L162))

Три ноды `ros_gz_bridge/parameter_bridge`, каждая со своим yaml (`config_file`):

| Нода | yaml | Когда |
|---|---|---|
| `gz_bridge` | `gz_bridge.yaml` | всегда |
| `gz_bridge_gripper_camera` | `gz_bridge_gripper_camera.yaml` | `camera_model != d435i` |
| `gz_bridge_d435i` | `gz_bridge_d435i.yaml` | `camera_model == d435i` |

Содержимое yaml разобрано в разделе 5.

### 3.9. Облако точек: depth_image_proc ([L199–216](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L199))

Для Orbbec облако строится не в Gazebo, а компонентом `depth_image_proc::PointCloudXyzrgbNode`
(как на реальном железе это делает драйвер):

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
`/gripper_camera/points` в кадре `gripper_camera_color_optical_frame`.

### 3.10. Вакуум в симуляции ([L220–229](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L220))

```python
vacuum_sim = Node(
    package='xarm_grasping',
    executable='vacuum_gripper_sim.py',
    name='vacuum_gripper_sim',
    parameters=[{'use_sim_time': True,
                 'robot_model_name': 'cart_xarm6',
                 'object_model_name': 'pick_box',
                 'object_size': [0.10, 0.05, 0.05]}],
```

Нода даёт сервис `/vacuum_gripper/set` (`std_srvs/SetBool`), проверяет касание присоски с
коробкой по позам из `/gz/robot_pose` и `/gz/object_pose` и шлёт `/vacuum/attach` или
`/vacuum/detach` в плагин `DetachableJoint` (он объявлен в `cart-arm.xacro`).
Это *логическая сварка*, а не физика присоски. `object_size` должен совпадать с размером
`pick_box` в мире (раздел 4).

### 3.11. Контроллеры руки ([L232–247](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L232))

`controller_manager` (часть **ros2_control** — фреймворка управления приводами) здесь живёт
*внутри* Gazebo, в плагине `gz_ros2_control` из URDF. Launch лишь вызывает `spawner`,
который загружает и активирует два контроллера:

- `joint_state_broadcaster` — публикует `/joint_states` руки;
- `xarm6_traj_controller` — исполняет траектории суставов (их шлёт MoveIt).

### 3.12. grasp_analyzer ([L257–280](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L257))

Топики и кадр камеры выбираются по `camera_model`:

| Параметр | Orbbec (по умолчанию) | d435i |
|---|---|---|
| `color_topic` | `/gripper_camera/color/image_raw` | `/camera/color/image_raw` |
| `points_topic` | `/gripper_camera/points` | `/camera/depth/points` |
| `camera_frame` | `gripper_camera_color_optical_frame` | `camera_color_optical_frame` |

Плюс `base_frame: link_base`, `analyze_period: 2.0` (с) и параметры сцены
([L275–277](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L275)):

| Параметр | Значение | Смысл |
|---|---|---|
| `table_height` | `0.015` | высота столешницы относительно `link_base`, м |
| `block_x`, `block_y` | `1.14`, `0.135` | запасная поза объекта в `link_base`, если облако не дало объект |

Откуда эти числа — объясняет комментарий над нодой
([L250–256](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L250)).
В дефолтной сцене (тележка `c2l-v3-scaled`, спавн в (-0.3, 0)) `link_base` руки в мире
находится примерно в точке (-0.54, -0.135, 0.910) — это **измерено** по `/gz/robot_pose` и TF.
Дальше простое вычитание:

- `table_height` = 0.925 (верх стола) − 0.910 = 0.015;
- `block_x/y` = позиция `pick_box` (0.60, 0) − (−0.54, −0.135) = (1.14, 0.135).

⚠️ Обрати внимание: числа верны только для дефолтной сцены. Если меняешь тележку,
точку спавна или крепление руки — пересчитай их.

### 3.13. RViz и MoveIt ([L282–344](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L282))

Одновременно открывается **только одно** окно RViz:

```python
condition=IfCondition(PythonExpression(
    ["'", use_rviz, "' == 'true' and '", use_moveit, "' != 'true'"])),
```

- `use_moveit:=true` (по умолчанию) — RViz открывает MoveIt с конфигом `cart_xarm_moveit.rviz`.
- `use_moveit:=false` — обычный RViz с `cart_xarm.rviz`.

MoveIt включается из `xarm_moveit_config/launch/_robot_moveit_common.launch.py`
([L302–335](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L302)).
Ключевые аргументы:

- `use_sim_time: 'true'` — передаётся **явно**. Иначе общий launch ставит нодам `False`,
  move_group живёт по системным часам и не исполняет траектории (комментарий L311–314).
- `publish_attach_tf: 'false'` — MoveIt знает только руку (`world → link_base`). У нас
  `link_base` уже ребёнок тележки; второй родитель сломал бы TF. Вместо этого публикуется
  статический `link_base → world` (нода `moveit_world_tf`, [L337](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L337)).
- `add_vacuum_gripper: 'true'`, `dof: '6'`, `ros2_control_plugin: 'gz_ros2_control/GazeboSimSystem'`.

⚠️ Обрати внимание: модель для MoveIt собирается отдельно (`uf_ros_lib/moveit_configs_builder.py`)
и **не знает** про тележку, камеру и `arm_initial_positions` — правки руки в URDF надо
дублировать там.

## 4. Мир `worlds/cart_table_offline.world`

Файл: [worlds/cart_table_offline.world](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world).
Формат **SDF** — XML-описание мира Gazebo. Все позы — `<pose>x y z roll pitch yaw</pose>`
в метрах/радианах, в системе мира.

| Объект | Строка | Поза | Размеры / заметки |
|---|---|---|---|
| `ground_plane` | [L10](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L10) | 0 | плоскость 20×20 м, трение 1.0 |
| свет `sun` | [L24](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L24) | `0 0 10` | направленный, с тенями |
| `table_simple` | [L42](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L42) | `1.05 0.0 0` | столешница 1.2×0.8×0.05, центр z=0.9 ([L46](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L46)) → **верх z=0.925**; 4 ножки; `static` |
| `pick_box` | [L107](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L107) | `0.60 0.0 0.9505` | 0.10×0.05×0.05 м, 0.1 кг, **динамическая**, плагин `PosePublisher` 10 Гц |
| `cylinder_block` | [L144](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L144) | `0.80 -0.28 0.9705` | r=0.04, h=0.09, `static` |
| `physics` | [L156](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L156) | — | шаг 1 мс, RTF 1 |

Стол с центром x=1.05 и шириной 1.2 м: ближний край на x=0.45 — при спавне в 0,0 корпус c2l-v3-scaled в него упирается, поэтому по умолчанию `spawn_x=-0.3`.
Высоты объектов посчитаны «верх стола + половина высоты»: `pick_box` 0.925+0.025+0.0005,
цилиндр 0.925+0.045+0.0005.

`pick_box` описан так ([L107–122](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L107)):

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
`pick_box` и link `link` зашиты в плагин (`cart-arm.xacro`, аргумент `vacuum_target_model`)
и в `vacuum_gripper_sim` (launch, L226). Переименовал — меняй во всех трёх местах.

**Системные плагины мира** ([L172–178](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L172)):

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
- `Sensors` — рендер камер (без него камера гриппера молчит). **Объявляется только в мире!**
  Модель собирается с `model_sensors_plugin:=false`; второй экземпляр Sensors даёт
  `Scene already exists` и segfault (комментарий [L169–171](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L169)).

Комментарий над Sensors ([L175](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L175))
напоминает: `/clock` публикует сам сервер gz sim, а не этот плагин. Версия симулятора —
gz sim 9 (Ionic), см. [L163](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L163).

**`cart_table.world`** — старый вариант: пол, солнце и стол тянутся из Gazebo Fuel по
URL (нужен интернет), стол при x=1.7, `cylinder_block` при `1.85 0.10 1.00`, нет `pick_box`.
⚠️ С ним вакуум работать не будет (нет `pick_box`), а рука не дотянется до стола из 0,0.

## 5. Конфиги мостов `config/gz_bridge*.yaml`

Каждая запись — один топик: `ros_topic_name`, `gz_topic_name`, типы с обеих сторон и
`direction`: `GZ_TO_ROS` (из симулятора в ROS) или `ROS_TO_GZ` (команды в симулятор).

### 5.1. [gz_bridge.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml) — общий

| ROS-топик | gz-топик | Тип ROS | Направление | Кто использует |
|---|---|---|---|---|
| `/clock` | `/clock` | `rosgraph_msgs/Clock` | gz→ROS | все ноды с `use_sim_time` |
| `/cart/wheel_joint_states` | `/model/cart/wheel_joint_states` | `sensor_msgs/JointState` | gz→ROS | `joint_state_merger` |
| `/vacuum/attach` | `/vacuum/attach` | `std_msgs/Empty` | ROS→gz | `vacuum_gripper_sim` |
| `/vacuum/detach` | `/vacuum/detach` | `std_msgs/Empty` | ROS→gz | `vacuum_gripper_sim` |
| `/vacuum/state` | `/vacuum/state` | `std_msgs/String` | gz→ROS | «attached»/«detached» |
| `/gz/robot_pose` | `/cart_xarm6/pose` | `tf2_msgs/TFMessage` | gz→ROS | `vacuum_gripper_sim` |
| `/gz/object_pose` | `/model/pick_box/pose` | `tf2_msgs/TFMessage` | gz→ROS | `vacuum_gripper_sim` |
| `/cmd_vel` | `/model/cart/cmd_vel` | `geometry_msgs/Twist` | ROS→gz | езда тележки |
| `/odom` | `/model/cart/odom` | `nav_msgs/Odometry` | gz→ROS | одометрия тележки |

Важные решения, записанные в комментариях:

- `/joint_states` **не** мостится ([L10](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml#L10)) —
  его публикует `joint_state_broadcaster`; мост дал бы второго издателя.
- `/gz/robot_pose` и `/gz/object_pose` — **не** `/tf` ([L51–63](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml#L51));
  полный `/world/.../pose/info` не мостится из-за нагрузки на CPU.

### 5.2. [gz_bridge_gripper_camera.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge_gripper_camera.yaml) — Orbbec (по умолчанию)

| ROS-топик | gz-топик | Тип |
|---|---|---|
| `/gripper_camera/color/image_raw` | `/gripper_camera/gz/image` | `sensor_msgs/Image` |
| `/gripper_camera/aligned_depth/image_raw` | `/gripper_camera/gz/depth_image` | `sensor_msgs/Image` (32FC1, метры) |
| `/gripper_camera/color/camera_info` | `/gripper_camera/gz/camera_info` | `sensor_msgs/CameraInfo` |

Все — `GZ_TO_ROS`. Имена на стороне ROS выбраны такими же, какие даст реальный драйвер
Orbbec, чтобы код не отличал симуляцию от железа. Облако `/gripper_camera/points` здесь
**не** мостится — его строит `depth_image_proc` (раздел 3.9).

### 5.3. [gz_bridge_d435i.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge_d435i.yaml) — старая RealSense

`/camera/color/image_raw`, `/camera/color/camera_info`, `/camera/depth/depth_info`,
`/camera/depth/points` (`PointCloudPacked` → `PointCloud2`), все `GZ_TO_ROS`, имена
gz и ROS совпадают. Оставлен для обратной совместимости (`camera_model:=d435i`).

## 6. `cart_xarm6_rviz.launch.py` — проверка модели без Gazebo

Файл: [launch/cart_xarm6_rviz.launch.py](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py).
Запускает только `robot_state_publisher`, `joint_state_publisher_gui` (окно с ползунками
суставов) и RViz с `cart_xarm.rviz`. Удобно проверять крепление руки и TF за секунды.

- xacro вызывается с `use_sim:=false` ([L51](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L51)).
- Аргументы: `use_gui` (`true`), `robot_model` (`c2l-v3-scaled`), `arm_offset_xyz`, `arm_offset_rpy` (`0 0 0`),
  `arm_initial_positions` ([L71–73](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L71)) —
  тот же аргумент, что и в gz-launch: шесть углов joint1…joint6 через пробел, рад.
- Его дефолт — константа `ARM_INITIAL_POSITIONS = '1.5708 -0.9 -1.3 0 1.0 0'`
  ([L25](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L25)).
- Ноды `joint_state_publisher(_gui)` создаются в функции `_joint_state_publishers`
  ([L28–40](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L28)), которую
  запускает `OpaqueFunction`. Зачем так: строку аргумента нужно разрезать на числа
  (`.perform(context).split()`), а это возможно только в момент запуска, когда значение
  уже известно. Углы передаются параметром `zeros` в виде `{'joint1': …, …}`
  (несмотря на имя — это стартовые значения).

Пример: `ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py arm_initial_positions:="0 0 0 0 0 0"`.

⚠️ Обрати внимание: дефолт `ARM_INITIAL_POSITIONS` **дублирует** дефолт `arm_initial_positions`
из gz-launch. Меняешь один — поменяй и второй, иначе RViz-проверка и Gazebo разойдутся.
Аргумента `camera_model` здесь нет — используется значение по умолчанию из xacro.

## 7. Конфиги RViz

- [rviz/cart_xarm.rviz](../../src/r2_robot/cart_xarm_bringup/rviz/cart_xarm.rviz): Fixed Frame `base_link`;
  RobotModel (`/robot_description`), TF, MarkerArray `/xarm_grasping/grasp_markers`,
  Image `/gripper_camera/color/image_raw`, PointCloud2 `/gripper_camera/points`,
  Image `/gripper_camera/aligned_depth/image_raw`.
- [rviz/cart_xarm_moveit.rviz](../../src/r2_robot/cart_xarm_bringup/rviz/cart_xarm_moveit.rviz): то же
  плюс `moveit_rviz_plugin/MotionPlanning` и `Trajectory`; Fixed Frame `base_link`.

⚠️ При `camera_model:=d435i` дисплеи камеры в обоих конфигах останутся пустыми — они
подписаны на `/gripper_camera/...`, а D435i публикует `/camera/...`.

## 8. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Поставить тележку в другое место (разово) | командная строка | `spawn_x/y/z/yaw` | `spawn_x:=-0.2 spawn_yaw:=0.3` |
| Поменять точку спавна по умолчанию | [cart_xarm6_gz.launch.py:55](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L55) | `default_value` у `spawn_*` | `'-0.2'` |
| Другая стартовая поза руки | [cart_xarm6_gz.launch.py:67](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L67) | `arm_initial_positions` | `'0 0 0 0 0 0'` |
| То же для RViz-проверки | [cart_xarm6_rviz.launch.py:25](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L25) | `ARM_INITIAL_POSITIONS` / аргумент `arm_initial_positions` | `arm_initial_positions:="0 0 0 0 0 0"` |
| Другая камера | [cart_xarm6_gz.launch.py:364](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L364) | `camera_model` | `camera_model:=d435i` |
| Ускорить камеру при низком RTF | [cart_xarm6_gz.launch.py:367](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L367) | `gripper_camera_preview` | `gripper_camera_preview:=true` |
| Gazebo без окна (SSH/CI) | [cart_xarm6_gz.launch.py:361](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L361) | `headless` | `headless:=true` |
| Другая тележка | [cart_xarm6_gz.launch.py:371](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L371) | `robot_model` | `robot_model:=c2` |
| Сдвинуть базу руки на тележке | [cart_xarm6_gz.launch.py:375](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L375) | `arm_offset_xyz/rpy` | `arm_offset_xyz:="0.05 0 0"` |
| Другой мир | [cart_xarm6_gz.launch.py:43](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L43) | `world` | `world:=/abs/path/my.world` |
| Без MoveIt / без RViz | [cart_xarm6_gz.launch.py:50](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L50) | `use_moveit`, `use_rviz` | `use_moveit:=false use_rviz:=false` |
| Увеличить задержки старта | [cart_xarm6_gz.launch.py:347](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L347) | `period=` у `TimerAction` | `period=8.0` |
| Высота стола / запасная поза объекта для grasp_analyzer | [cart_xarm6_gz.launch.py:275](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L275) | `table_height`, `block_x`, `block_y` | пересчитать при другой тележке/спавне |
| Частота анализа захвата | [cart_xarm6_gz.launch.py:274](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L274) | `analyze_period` | `1.0` |
| Передвинуть стол | [cart_table_offline.world:42](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L42) | `<pose>` модели `table_simple` | `1.10 0.0 0 0 0 0` |
| Передвинуть коробку | [cart_table_offline.world:107](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L107) | `<pose>` `pick_box` | `0.65 0.1 0.9505 0 0 0` |
| Изменить размер коробки | [cart_table_offline.world:130](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L130) + [launch:227](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L227) | `<size>` (visual+collision), `object_size`, инерция | синхронно в обоих файлах |
| Передвинуть цилиндр | [cart_table_offline.world:144](../../src/r2_robot/cart_xarm_bringup/worlds/cart_table_offline.world#L144) | `<pose>` `cylinder_block` | `0.75 0.25 0.9705 0 0 0` |
| Добавить мост нового топика | [gz_bridge.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge.yaml) | новая запись `- ros_topic_name: ...` | см. формат раздела 5 |

Если меняешь высоту стола — пересчитай z у `pick_box` и `cylinder_block` и обнови
`docs/STATUS.md` (раздел «Сцена»), как просит комментарий в мире.

## 9. Частые ошибки

1. **Добавили плагин Sensors в модель робота** → `Scene already exists`, segfault. Sensors — только в мире.
2. **Нода без `use_sim_time`** (особенно в чужих include) → TF «из будущего», MoveIt не
   исполняет траектории. В этом launch стоит глобальный `SetParameter` + явная передача в MoveIt.
3. **Создали новый `.world` или `.yaml` и не пересобрали** → `file not found`. Новые файлы
   требуют `colcon build --packages-select cart_xarm_bringup`.
4. **Мир без `pick_box`** (например `cart_table.world`) → вакуум не работает, `vacuum_gripper_sim` не находит объект.
5. **`pkill -f cart_xarm6`** в той же команде, где это слово встречается, убивает саму оболочку.
   Завершай launch через Ctrl+C.
6. **Gazebo не успел стартовать** → `create` или `spawner` падают с таймаутом. Увеличь задержки в `TimerAction`.
7. **Два издателя `/joint_states`** — не добавляй `/joint_states` в мост.
8. **Аргумент со пробелами без кавычек**: `arm_initial_positions:=0 0 0 0 0 0` не сработает —
   нужно `arm_initial_positions:="0 0 0 0 0 0"`.

## 10. Проверь себя

**Упражнение 1. Аргументы.** Выведи список аргументов и найди дефолт `robot_model`.
Сравни с таблицей моделей в `CART_MODELS.md`.

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
ros2 topic echo /clock --once
ros2 param get /robot_state_publisher use_sim_time
```

**Упражнение 3. Мосты.** При запущенной симуляции найди все топики камеры и узнай их частоту.
Затем перезапусти с `camera_model:=d435i` и сравни список.

```bash
ros2 topic list | grep -E 'gripper_camera|camera'
ros2 topic hz /gripper_camera/points
ros2 topic info /cmd_vel -v      # кто публикует, кто подписан (мост)
```

**Упражнение 4. Мир и вакуум.** Сдвинь `pick_box` в мире на y=0.1 (L108), перезапусти
(пересборка не нужна — файл существующий), проверь позу коробки и состояние присоски:

```bash
ros2 topic echo /gz/object_pose --once
ros2 topic echo /vacuum/state --once
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
```

Без подвода TCP к коробке сервис должен отказать (нет касания) — почему? (Подсказка: раздел 3.10.)
