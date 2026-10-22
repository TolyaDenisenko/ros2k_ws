# Глава 5. `xarm_gz_bringup` — рука xArm6 одна на столе

> ⚠️ **Для отладки захвата есть более полный пример** —
> [глава 8, «Рука на столе»](08-рука-на-столе.md): та же рука, что на тележке
> (камера на кронштейне, вакуум на несколько объектов, распознавание, MoveIt,
> pick & place). Эта глава описывает старый минимальный сценарий `xarm_gz_bringup`.

> Пакет: [`src/xarm_gz_bringup`](../../src/xarm_gz_bringup)
> Запуск: `ros2 launch xarm_gz_bringup xarm6_gz.launch.py add_vacuum_gripper:=true`

## 1. Зачем этот пакет и с чем он связан

Это **самый простой сценарий** в workspace: только рука xArm6 (без тележки `quasi_cart`), стоящая у стола, в симуляторе **Gazebo Sim 9** (далее gz — физический симулятор, «виртуальный мир»), плюс **MoveIt 2** (планировщик движений: по целевой позе считает траекторию суставов без столкновений) и **RViz** (3D-визуализатор ROS).

Используй его, чтобы:
- проверить, что сама рука, её контроллеры и MoveIt работают, не отвлекаясь на тележку;
- отлаживать планирование «Plan & Execute» в RViz;
- быстро что-то попробовать: стартует быстрее и проще, чем `cart_xarm_bringup`.

Пакет сам по себе почти ничего не содержит: launch-файл **склеивает чужие куски**:

| Что | Откуда берётся | Глава |
|---|---|---|
| URDF руки (описание звеньев/суставов) | `xarm_description/launch/_robot_description.launch.py` → `xarm_device.urdf.xacro` | [гл. 6](06-xarm_ros2.md) |
| Контроллеры (ros2_control) | `xarm_controller/config/xarm6_controllers.yaml` (подгружает плагин gz_ros2_control) | гл. 6 |
| MoveIt + RViz | `xarm_moveit_config/launch/_robot_moveit_common.launch.py` | гл. 6 |
| Gazebo и мост ROS↔gz | системные пакеты `ros_gz_sim`, `ros_gz_bridge` | — |
| Мир со столом и цилиндром | **этот пакет**, `worlds/table_gz.world` | эта глава |

Термины:
- **launch-файл** — Python-скрипт, который запускает набор нод (процессов ROS) с параметрами.
- **ros2_control** — фреймворк управления приводами: «железо» (здесь — плагин `gz_ros2_control`, эмулирующий моторы внутри Gazebo) + **контроллеры** (например, `joint_trajectory_controller`, исполняющий траектории). Менеджер контроллеров (`controller_manager`) живёт **внутри процесса Gazebo**.
- **use_sim_time** — параметр ноды: брать время из топика `/clock` (время симулятора), а не системные часы.

> ⚠️ Обрати внимание: исторически пакет создан вместо пакета `xarm_gazebo` из форка UFactory, который написан под Gazebo Classic и отключён (`COLCON_IGNORE`, см. гл. 6). Комментарий прямо в коде: [xarm6_gz.launch.py:23](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L23).

## 2. Дерево папок

```text
src/xarm_gz_bringup/
├── CMakeLists.txt            # только install(DIRECTORY launch config worlds)
├── package.xml               # exec_depend на xarm_description, xarm_moveit_config, ros_gz_*, ...
├── launch/
│   └── xarm6_gz.launch.py    # ЕДИНСТВЕННЫЙ launch: Gazebo + URDF + спавн + контроллеры + мост + MoveIt
├── config/
│   └── gz_bridge.yaml        # мост ros_gz_bridge: только /clock (gz → ROS)
└── worlds/
    ├── table_gz.world        # ИСПОЛЬЗУЕТСЯ: земля, солнце, стол Fuel, цилиндр, физика
    └── cylinder_block.world  # «донор»: тот же цилиндр отдельным файлом, launch его НЕ грузит
```

Сборка: `ament_cmake`, кода нет — CMake просто копирует три папки в `install/xarm_gz_bringup/share/...` ([CMakeLists.txt](../../src/xarm_gz_bringup/CMakeLists.txt)). С `--symlink-install` правки launch/world/yaml подхватываются без пересборки; **новый** файл — пересобрать пакет.

## 3. Разбор файлов

### 3.1. `launch/xarm6_gz.launch.py` (172 строки)

Файл: [launch/xarm6_gz.launch.py](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py). Состоит из шести блоков и таймеров.

**Аргументы** ([строки 15–21](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L15-L21), объявления [149–163](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L149-L163)):

| Аргумент | Дефолт | Смысл |
|---|---|---|
| `robot_type` | `xarm` | семейство (xarm/lite/uf850) |
| `dof` | `6` | число суставов |
| `add_gripper` | `false` | двухпалый захват UFactory |
| `add_vacuum_gripper` | `false` | **вакуумная присоска** (меш + `link_tcp` на 0.126 м от фланца) |
| `add_bio_gripper` | `false` | BIO-захват |
| `prefix` | `''` | префикс имён суставов (для двух рук) |
| `hw_ns` | `xarm` | неймспейс «железа» |
| `arm_initial_positions` | `0 -0.6 -0.7 0 1.3 0` | стартовые углы joint1..joint6, рад |

Обрати внимание: `add_vacuum_gripper` передаётся **дважды** — в URDF для Gazebo ([строка 55](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L55)) и в MoveIt ([строка 120](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L120)). MoveIt строит свою модель руки (гл. 6), и если флаги разойдутся, MoveIt не будет знать про присоску.

> ⚠️ Обрати внимание: в этом сценарии присоска — **только геометрия**. Плагина `DetachableJoint` и ноды `vacuum_gripper_sim.py` тут нет (они есть только в сцене с тележкой, см. `docs/CAMERA_VACUUM_SIM.md` и `docs/STATUS.md`, пункт K7).

**Блок 1 — Gazebo** ([строки 24–40](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L24-L40)):

```python
world_file = os.path.join(
    get_package_share_directory('xarm_gz_bringup'), 'worlds', 'table_gz.world')
...
launch_arguments={
    'gz_args': f'-r -v 4 {world_file}',   # -r: сразу запустить физику, -v 4: подробный лог
    'on_exit_shutdown': 'true'            # закрыл окно gz → гаснет весь launch
}.items()
```

Мир зашит в коде; аргумента `world` нет. Сменить мир — править строку 25.

**Блок 2 — URDF** ([строки 43–63](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L43-L63)): включает `_robot_description.launch.py` из `xarm_description`, который прогоняет xacro и запускает `robot_state_publisher` (нода, публикующая `/robot_description` и TF звеньев по `/joint_states`). Ключевые аргументы:

```python
'ros2_control_plugin': 'gz_ros2_control/GazeboSimSystem',   # «железо» = Gazebo
'ros2_control_pausable': 'true',
'xarm_initial_positions': LaunchConfiguration('arm_initial_positions'),
```

`ros2_control_plugin` переключает в xacro: пути мешей на `file://` (gz не понимает `package://`) и вставку плагина `gz_ros2_control-system` с yaml контроллеров (подробно в гл. 6).

> ⚠️ Обрати внимание: `ros2_control_pausable` в `_robot_description.launch.py` не читается (там нет такого `LaunchConfiguration`), т.е. аргумент фактически игнорируется.

**Блок 3 — спавн робота** ([строки 66–76](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L66-L76)) — **главная точка настройки позы**:

```python
spawn_robot = Node(
    package='ros_gz_sim',
    executable='create',
    arguments=[
        '-name', 'xarm6',
        '-topic', 'robot_description',
        '-x', '0', '-y', '-0.84', '-z', '1.015',  # верх столешницы Fuel Table = 1.0 + 0.03/2; при 1.0 база была внутри стола и контакт расталкивал суставы
    ],
    ...
```

`create` берёт URDF из топика `/robot_description` и ставит модель в мир, помещая `link_base`/`world`-корень робота в точку `(x, y, z)` мировой СК (метры). Можно также добавить `-R -P -Y` (крен/тангаж/рыскание, рад).

Основание руки ставится на высоту `z = 1.015` м — на верх столешницы стола Fuel (стол тоже стоит в `y=-0.84`), а цилиндр лежит рядом на той же столешнице. Гравитация в мире выключена (см. 3.3), так что рука держится на месте и без опоры.

Почему 1.015, а не 1.0: модель Fuel Table имеет верх на 1.0 м плюс половина толщины доски (0.03/2); при 1.0 база пересекалась со столом, контакт «расталкивал» суставы (так сказано в комментарии).

**Блок 4 — спавнеры контроллеров** ([строки 79–93](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L79-L93)): `controller_manager/spawner` просит менеджер (он уже поднят плагином внутри gz) активировать `joint_state_broadcaster` (публикует `/joint_states`) и `xarm6_traj_controller` (исполняет траектории; action `/xarm6_traj_controller/follow_joint_trajectory`). Имена должны совпадать с `xarm_controller/config/xarm6_controllers.yaml`.

**Блок 5 — мост** ([строки 96–105](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L96-L105)): `ros_gz_bridge/parameter_bridge` с `config/gz_bridge.yaml`.

**Блок 6 — MoveIt** ([строки 108–131](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L108-L131)):

```python
'use_sim_time': 'true',
'controllers_name': 'fake_controllers',
'moveit_controller_manager_key': 'moveit_simple_controller_manager',
'moveit_controller_manager_value': 'moveit_simple_controller_manager/MoveItSimpleControllerManager',
```

- `use_sim_time` передаётся **явно**: `_robot_moveit_common.launch.py` ставит нодам `{'use_sim_time': <arg>}` (дефолт False), и это перекрывает глобальный `SetParameter` (комментарий [124–125](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L124-L125)).
- `controllers_name: fake_controllers` — это лишь имя yaml со списком контроллеров для MoveIt (`xarm_moveit_config/config/xarm6/fake_controllers.yaml`); по содержимому он совпадает с `controllers.yaml`, а `MoveItSimpleControllerManager` реально шлёт траектории в `xarm6_traj_controller` в Gazebo.

**Тайминги** ([строки 134–142](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L134-L142)):

| Что | Задержка | Зачем |
|---|---|---|
| спавн робота | 3 с | gz должен успеть загрузить мир |
| спавнеры контроллеров | 5 с | менеджер появляется после спавна модели |
| MoveIt + RViz | 12 с (было 7) | ждать, пока контроллеры активны и есть `/joint_states` |

**Глобальный `SetParameter('use_sim_time', True)`** ([строка 146](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L146)) — всем нодам дерева. Без него MoveIt считает состояния из Gazebo «протухшими».

**Начальная поза** ([строки 156–163](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L156-L163)): `0 -0.6 -0.7 0 1.3 0` — инструмент смотрит вертикально вниз, кончик ~0.29 м над столом и ~0.31 м впереди основания. Нулевая поза плоха: присоска (0.126 м) уходила под столешницу и MoveIt-траектории обрывались с `PATH_TOLERANCE_VIOLATED`. Значения уходят в `initial_value` интерфейсов ros2_control (см. гл. 6, `xarm6.ros2_control.xacro`). Работает только в симуляции.

> ⚠️ Обрати внимание: RViz/MoveIt показывают руку в своём кадре `world`, который совпадает с `link_base` (static TF `world → link_base` с нулями, публикует `_robot_moveit_common`). Смещение `-y -0.84` из Gazebo в RViz **не видно** — это нормально.

### 3.2. `config/gz_bridge.yaml`

[config/gz_bridge.yaml](../../src/xarm_gz_bringup/config/gz_bridge.yaml) — один мост:

```yaml
- ros_topic_name: "/clock"
  gz_topic_name: "/clock"
  ros_type_name: "rosgraph_msgs/msg/Clock"
  gz_type_name: "gz.msgs.Clock"
  direction: GZ_TO_ROS
```

`/clock` — источник сим-времени. `/joint_states` мост не нужен: его публикует `joint_state_broadcaster` прямо в ROS (менеджер внутри gz — ROS-нода). Хочешь камеру/другие топики из gz — добавляй сюда записи того же формата (пример — `cart_xarm_bringup/config/gz_bridge.yaml`).

### 3.3. `worlds/table_gz.world` (используется)

[worlds/table_gz.world](../../src/xarm_gz_bringup/worlds/table_gz.world) — SDF-файл (формат описания мира Gazebo).

| Строки | Что | Значение |
|---|---|---|
| [4–13](../../src/xarm_gz_bringup/worlds/table_gz.world#L4-L13) | Ground Plane, Sun | модели из **Fuel** (онлайн-каталог; при первом запуске качаются из интернета, потом кешируются в `~/.gz/fuel`) |
| [14–18](../../src/xarm_gz_bringup/worlds/table_gz.world#L14-L18) | стол `Table` (Fuel) | `<pose>0.0 -0.84 0 0 0 0</pose>`, верх ≈ 1.015 м |
| [20–26](../../src/xarm_gz_bringup/worlds/table_gz.world#L20-L26) | физика ODE | шаг 1 мс, RTF 1, **гравитация `0 0 0`** |
| [32–59](../../src/xarm_gz_bringup/worlds/table_gz.world#L32-L59) | `cylinder_block` | static, `<pose>0.35 -0.75 1.045 ...>`, R=0.04, h=0.09, оранжевый |

```xml
<physics type="ode">
  <max_step_size>0.001</max_step_size>
  <real_time_factor>1</real_time_factor>
  <real_time_update_rate>1000</real_time_update_rate>
  <!-- (2022-12-20) humble: change gravity from [0 0 -9.81] to [0 0 0] -->
  <gravity>0 0 0</gravity>
</physics>
```

> ⚠️ Обрати внимание: **гравитации нет** (наследие UFactory). Объекты не падают, рука «висит». Для этого сценария это упрощение; в мире тележки (`cart_table_offline.world`) — своя физика.

> ⚠️ Обрати внимание: в мире **нет** плагинов `Physics/SceneBroadcaster/UserCommands/Sensors` явно — gz подставляет дефолтные системы, когда в мире `<plugin>` нет вовсе. Если добавишь камеру с gz-сенсором, нужна система Sensors **ровно одна** (иначе `Scene already exists` + segfault; см. CLAUDE.md). D435i-xacro по умолчанию сам добавляет Sensors (`model_sensors_plugin`, гл. 6).

Цилиндр встроен в мир копией: `<include>model://cylinder_block</include>` не работал, т.к. `cylinder_block.world` — это мир, а не модель в `GZ_SIM_RESOURCE_PATH` (gz падал с Error Code 14) — комментарий [строки 29–31](../../src/xarm_gz_bringup/worlds/table_gz.world#L29-L31). Высота 1.045 = 1.0 + 0.09/2.

Положение цилиндра относительно основания руки (если рука в `0, -0.84`): `Δx = 0.35`, `Δy = +0.09`.

### 3.4. `worlds/cylinder_block.world` (не используется launch-ом)

[worlds/cylinder_block.world](../../src/xarm_gz_bringup/worlds/cylinder_block.world) — тот же цилиндр, но `<pose>0.35 -0.75 0.97 ...>` ([строка 10](../../src/xarm_gz_bringup/worlds/cylinder_block.world#L10)). Остался как исходник. Если правишь цилиндр — правь в `table_gz.world`.

> ⚠️ Обрати внимание: высоты расходятся (0.97 vs 1.045). Правильная для текущего стола — 1.045.

### 3.5. `package.xml`, `CMakeLists.txt`

Кратко: `ament_cmake`, `exec_depend` на `xarm_description`, `xarm_moveit_config`, `xarm_controller`, `ros_gz_sim`, `ros_gz_bridge`, `gz_ros2_control`, `robot_state_publisher`, `controller_manager`, `joint_state_broadcaster`, `joint_trajectory_controller`. Поле maintainer — заглушка `user@example.com`.

## 4. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Поставить руку на столешницу | [xarm6_gz.launch.py:72](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L72) | `-z` | `'-z', '1.015'` |
| Сдвинуть руку в мире | [xarm6_gz.launch.py:72](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L72) | `-x`, `-y` | `'-x', '0.1', '-y', '-0.8'` |
| Повернуть руку | [xarm6_gz.launch.py:72](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L72) | добавить `-Y` | `'-Y', '1.5708'` |
| Включить присоску | командная строка | `add_vacuum_gripper` | `add_vacuum_gripper:=true` |
| Другая стартовая поза суставов | [xarm6_gz.launch.py:162](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L162) или CLI | `arm_initial_positions` | `arm_initial_positions:="0 -0.3 -0.9 0 1.2 0"` |
| Другой мир | [xarm6_gz.launch.py:25](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L25) | имя файла | `'my.world'` (положи в `worlds/`, пересобери) |
| Подвинуть стол | [table_gz.world:17](../../src/xarm_gz_bringup/worlds/table_gz.world#L17) | `<pose>` | `0.0 -0.9 0 0 0 0` |
| Подвинуть цилиндр | [table_gz.world:34](../../src/xarm_gz_bringup/worlds/table_gz.world#L34) | `<pose>` | `0.30 -0.80 1.045 0 0 0` |
| Размер цилиндра | [table_gz.world:40-41](../../src/xarm_gz_bringup/worlds/table_gz.world#L40-L41), [53-54](../../src/xarm_gz_bringup/worlds/table_gz.world#L53-L54) | `radius`, `length` | 0.03 / 0.12 (и z = 1.0 + h/2) |
| Включить гравитацию | [table_gz.world:25](../../src/xarm_gz_bringup/worlds/table_gz.world#L25) | `<gravity>` | `0 0 -9.81` |
| Подробность лога gz | [xarm6_gz.launch.py:37](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L37) | `-v` | `-v 1` |
| Медленная машина: MoveIt стартует рано | [xarm6_gz.launch.py:142](../../src/xarm_gz_bringup/launch/xarm6_gz.launch.py#L142) | `period` | `15.0` |
| Пробросить ещё топик из gz | [gz_bridge.yaml](../../src/xarm_gz_bringup/config/gz_bridge.yaml) | новая запись | `- ros_topic_name: ...` |

Правило: всё, что меняет **модель руки** (присоска, захват), меняй синхронно в блоке 2 и блоке 6.

## 5. Частые ошибки

| Симптом | Причина | Что делать |
|---|---|---|
| gz долго висит на старте / пустой мир | Fuel-модели качаются из интернета | дождаться; офлайн — нужен кеш `~/.gz/fuel` |
| `Error Code 14` при загрузке мира | `<include model://...>` на файл, который не модель | встраивать `<model>` прямо в world |
| Спавнер: `Could not contact service /controller_manager/...` | менеджер ещё не поднялся (gz медленный) | увеличить задержку на строке 135 |
| MoveIt «Plan» ок, «Execute» — ошибка / `PATH_TOLERANCE_VIOLATED` | контакт инструмента со столом, рука сбивается | проверить `-z` и `arm_initial_positions` |
| MoveIt пишет, что состояние устарело / не исполняет | ноды без `use_sim_time` | не убирать строки 126 и 146 |
| Рука в RViz без присоски, а в gz — с ней | флаг передан только в один include | см. строки 55 и 120 |
| Рука «внутри» стола | `-z` меньше высоты столешницы (например `0` или `1.0`) | вернуть `'-z', '1.015'` |
| `Scene already exists` + segfault | две системы Sensors | одна Sensors на мир |

## 6. Проверь себя

```bash
cd ~/ros2k_ws && source /opt/ros/kilted/setup.bash
colcon build --symlink-install --packages-select xarm_gz_bringup && source install/setup.bash

# 1) Где фактически стоит рука? (ожидается -0.84 по y и текущий z)
grep -n "'-z'" src/xarm_gz_bringup/launch/xarm6_gz.launch.py

# 2) Синтаксис мира
gz sdf -k src/xarm_gz_bringup/worlds/table_gz.world

# 3) URDF руки с присоской — тот, что получит gz
xacro src/xarm_ros2/xarm_description/urdf/xarm_device.urdf.xacro dof:=6 \
  add_vacuum_gripper:=true ros2_control_plugin:=gz_ros2_control/GazeboSimSystem > /tmp/xarm.urdf
check_urdf /tmp/xarm.urdf | head
grep -c "gz_ros2_control-system" /tmp/xarm.urdf   # ожидается 1

# 4) Аргументы launch
ros2 launch xarm_gz_bringup xarm6_gz.launch.py --show-args
```

После запуска (в другом терминале):

```bash
ros2 control list_controllers           # оба контроллера active
ros2 topic echo /clock --once           # сим-время идёт
ros2 topic echo /joint_states --once    # стартовые углы ≈ arm_initial_positions
ros2 param get /move_group use_sim_time # True
```

Вопросы:
1. Почему `add_vacuum_gripper` передаётся в два include?
2. Что изменится в RViz, если поменять `-y -0.84` на `-y 0`? (Подсказка: ничего — почему?)
3. Зачем мост только для `/clock`, а `/joint_states` не мостится?
4. Почему цилиндр стоит на z=1.045?
