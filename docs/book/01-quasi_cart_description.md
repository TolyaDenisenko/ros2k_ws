# Глава 1. Пакет `quasi_cart_description`: тележка и рука на ней

> Путь: `src/r2_robot/quasi_cart_description`
> В этой главе: модель робота (URDF) — тележка quasi_cart, рука xArm6 на её крыше,
> «вакуумная присоска» для симуляции и плагины Gazebo.

---

## 1. Зачем этот пакет и кто его использует

Любой ROS-робот начинается с **описания робота** — файла URDF.

- **URDF** (Unified Robot Description Format) — XML-файл. Он описывает, из каких
  жёстких частей состоит робот, как они соединены, как выглядят (меши) и сколько весят.
- **xacro** (XML Macros) — «препроцессор» для URDF. В нём есть переменные, условия,
  макросы и `include`. Команда `xacro файл.xacro аргумент:=значение` превращает его
  в обычный URDF.

В пакете **нет кода**: только xacro-файлы и меши. Его «потребители»:

| Кто | Как использует |
|---|---|
| [cart_xarm6_gz.launch.py:79-91](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L79) | запускает `xacro cart_with_arm.urdf.xacro use_sim:=true robot_model:=… …`, результат уходит в `robot_description` (его читают `robot_state_publisher` и Gazebo) |
| [cart_xarm6_rviz.launch.py:49](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L49) | то же, но без симуляции — только просмотр модели в RViz |
| `xarm_description` (пакет из `src/xarm_ros2`) | наоборот, **используется этим пакетом**: макрос `xarm_device` рисует саму руку, присоску и камеру |
| [vacuum_gripper_sim.py](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py) | управляет плагином DetachableJoint, объявленным здесь, в `cart-arm.xacro` |

> ⚠️ Обрати внимание: MoveIt строит **свою** модель руки (без тележки) в
> `uf_ros_lib/moveit_configs_builder.py`. Поэтому сдвиг руки на тележке MoveIt
> «не видит» напрямую — см. CLAUDE.md.

### Основные термины (нужны для всей главы)

- **link (звено)** — одна жёсткая деталь: корпус, колесо, датчик. У него может быть
  `<visual>` (как выглядит), `<collision>` (форма для столкновений) и `<inertial>`
  (масса и инерция). Звено может быть и пустым — просто именованная точка
  (системой координат, «фреймом»).
- **joint (сустав)** — связь «родитель → ребёнок» между двумя звеньями. Типы:
  `fixed` (намертво), `revolute` (вращение с пределами), `continuous` (вращение без
  пределов), `prismatic` (сдвиг). Все звенья образуют дерево с одним корнем.
- **origin xyz/rpy** — где ребёнок находится относительно родителя:
  `xyz` — сдвиг в метрах (X вперёд, Y влево, Z вверх), `rpy` — поворот в радианах
  (roll вокруг X, pitch вокруг Y, yaw вокруг Z).
- **`xacro:arg` vs `xacro:property`**:
  - `xacro:arg` — **аргумент снаружи**. Задаётся из командной строки/launch
    (`robot_model:=c2`), читается как `$(arg robot_model)`. Если один и тот же arg
    объявлен несколько раз, работает **первое** объявление (поэтому «верхний» файл
    может переопределить default вложенного).
  - `xacro:property` — **внутренняя переменная**, читается как `${имя}`. Задаётся
    только внутри xacro. С атрибутом `default="…"` свойство присваивается, **только
    если ещё не было задано раньше** — на этом построен выбор точки крепления руки.
- **Gazebo (gz sim)** — физический симулятор. Блоки `<gazebo>` в URDF — это
  дополнения только для него (плагины, сенсоры, трение). RViz их игнорирует.

---

## 2. Дерево папок

```text
quasi_cart_description/
├── package.xml                         — метаданные пакета, зависимости (xacro, robot_state_publisher, xarm_description)
├── CMakeLists.txt                      — только устанавливает urdf/ и meshes/ в share/
├── meshes/                             — 32 файла STL (корпус c2, колёса, ToF, лидар, IMU)
└── urdf/
    ├── cart_with_arm.urdf.xacro        — ГЛАВНЫЙ файл: тележка + рука (используют наши launch)
    ├── cart.urdf.xacro                 — только тележка (оригинал r2_robot, + ros2_control реального железа)
    ├── cart-arm.xacro                  — крепление xArm6 к тележке + DetachableJoint вакуума
    ├── cart-base.urdf.xacro            — общие звенья: base_footprint, drive_link, колёса, ToF, лидар, IMU, камеры навигации
    ├── cart-drive-differential.urdf.xacro — поворотные (caster) колёса для дифф. привода (используется)
    ├── cart-drive-mecanum.urdf.xacro   — 4 меканум-колеса (НЕ используется, мешей нет)
    ├── cart.ign.xacro                  — Gazebo: цвет и трение корпуса/колёс
    ├── cart-sensors.ign.xacro          — Gazebo: плагины движения/одометрии/IMU и сенсоры ToF, лидар, realsense
    ├── c2.urdf.xacro                   — модель тележки c2 (по умолчанию в cart.urdf.xacro)
    ├── c2-v3.urdf.xacro                — побайтно совпадает с c2.urdf.xacro
    ├── c2l-v3.urdf.xacro               — тележка c2l (длинная), задаёт arm_mount_xyz
    ├── c2l-v3-scaled.urdf.xacro        — c2l с растянутым мешем c2 (ПО УМОЛЧАНИЮ в наших launch)
    ├── c2h-v3 / c2m-v3 / c2t-v3 / c2w-v3.urdf.xacro — другие модели (их меши корпуса в пакете отсутствуют)
    ├── _d455.urdf.xacro                — макрос камеры RealSense D455 (навигационная камера тележки)
    └── _d405.urdf.xacro                — макрос RealSense D405 (нигде не подключается)
```

**package.xml / CMakeLists.txt.** Пакет типа `ament_cmake`, но ничего не компилирует.
[CMakeLists.txt:14](../../src/r2_robot/quasi_cart_description/CMakeLists.txt#L14)
копирует папки `urdf meshes config launch rviz worlds` (с флагом `OPTIONAL` —
отсутствующие пропускаются; реально существуют только `urdf` и `meshes`) в
`install/share/quasi_cart_description`. Именно оттуда их берут `$(find …)` и
launch-файлы. Зависимость от `xarm_description` в
[package.xml:20](../../src/r2_robot/quasi_cart_description/package.xml#L20) нужна, потому
что `cart-arm.xacro` подключает макрос руки.

**Меши** (`meshes/`): `base_link_c2-v3.stl` (корпус), `drive_link.stl`,
`drive_wheel_{left,right}_link.STL`, `wheel_{rl,rr}_link.stl`,
`wheel_bracket_{rl,rr}_link.stl`, `imu_link.STL`, `lidar_link.STL`, `tof1…16_link.STL`,
`tof21…26_link.STL`. Пути в URDF пишутся как
`file://$(find quasi_cart_description)/meshes/...` — gz не умеет `package://` для наших
пакетов (см. CLAUDE.md).

---

## 3. Как собирается модель: цепочка include

```text
launch (cart_xarm6_gz.launch.py)
  └─ xacro cart_with_arm.urdf.xacro  use_sim:=true robot_model:=c2l-v3-scaled ...
       ├─ $(arg robot_model).urdf.xacro   → c2l-v3-scaled.urdf.xacro
       │     └─ c2l-v3.urdf.xacro         (base_link, ToF, лидар, IMU, ведущие колёса, arm_mount_xyz)
       ├─ cart-drive-differential.urdf.xacro  (поворотные колёса)
       ├─ cart-base.urdf.xacro            (base_footprint → drive_link, звенья датчиков, камеры enable_camera_*)
       │     └─ _d455.urdf.xacro           (только если enable_camera_realsense)
       ├─ [use_sim] cart.ign.xacro        (трение/цвет)
       ├─ [use_sim] cart-sensors.ign.xacro (плагины и сенсоры gz)
       ├─ [use_sim] JointStatePublisher колёс
       └─ cart-arm.xacro → макрос mount_xarm6_on_cart
             ├─ arm_mount_link (+ joint от base_link в точке arm_mount_xyz)
             ├─ $(find xarm_description)/urdf/xarm_device_macro.xacro → рука, присоска, камера
             └─ [use_sim] DetachableJoint + PosePublisher
```

Дерево звеньев (упрощённо):

```text
base_footprint ─base_joint→ drive_link ─drive_joint→ base_link ─arm_mount_joint→ arm_mount_link → link_base (xArm) → link1 … link6 → link_eef → присоска, камера
                               ├─ drive_wheel_left/right_link            ├─ tof*_link, lidar_link, imu_link
                               └─ caster_wheel_*                         
```

Корень — `base_footprint` (точка на полу). `drive_link` поднят на
`base_axis_z_offset` (ось ведущих колёс), `base_link` (корпус) сдвинут от неё по X.

---

## 4. Разбор файлов

### 4.1 `cart_with_arm.urdf.xacro` — главный файл

**Аргументы** ([строки 20-44](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L20)):

| Аргумент | Default | Смысл |
|---|---|---|
| `use_sim` | `false` | `true` — добавить всё для Gazebo (launch gz передаёт `true`) |
| `robot_model` | `c2l-v3-scaled` | имя файла тележки `<robot_model>.urdf.xacro` |
| `simulator` | `ignition` | единственный поддерживаемый вариант (= gz sim) |
| `enable_floor_tofs` | `true` | напольные ToF-датчики 21–26 |
| `enable_extra_floor_tofs` | `false` | доп. ToF 27–30 (есть у c2l) |
| `enable_camera_realsense` / `_oak` / `_orbbec` | `false` | навигационная камера тележки (НЕ камера на руке!) |
| `camera_roll_offset`, `camera_pitch_offset` | `0.0` | доводка наклона навигационной камеры |
| `enable_pace_battery` | `true` | объявлен, но в этом файле не используется (см. ⚠️ ниже) |
| `model_sensors_plugin` | `false` | НЕ добавлять gz Sensors-систему в модель |
| `arm_dof` | `6` | число суставов xArm |
| `arm_prefix` | `""` | префикс имён звеньев руки |
| `arm_hw_ns` | `xarm` | namespace железа |
| `arm_offset_xyz` / `arm_offset_rpy` | `0 0 0` | доп. смещение/поворот руки от точки крепления |
| `arm_ros2_control_plugin` | `gz_ros2_control/GazeboSimSystem` | «драйвер» суставов руки в симуляции |

Почему `model_sensors_plugin` = `false` — объяснено прямо в коде:

```xml
<!-- Sensors-система gz объявлена в мирах cart_xarm_bringup/worlds/*.world.
     Дублирование в модели (...) давало 3 экземпляра -> "Scene already exists
     with name: scene" и segfault в libgz-sim-sensors-system. Объявлено ДО
     include'ов, чтобы этот default перекрыл default=true во вложенных файлах. -->
<xacro:arg name="model_sensors_plugin" default="false"/>
```
([строки 31-36](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L31)).
Здесь работает правило «первое объявление arg побеждает»: в
`cart-sensors.ign.xacro:8` default `true`, но он уже не действует.

**Свойства** ([строки 46-52](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L46)):

```xml
<xacro:property name="var_simulator" value="$(arg simulator)"/>
<xacro:property name="robot_model" value="$(arg robot_model)"/>
<xacro:property name="drive" value="differential"/>
```

`robot_model` копируется из arg в property, потому что `cart-base.urdf.xacro` пишет
выражения вида `${robot_model.startswith('c2w')}` (а в `${…}` видны только
property). `drive` выбирает файл привода — **жёстко** `differential`, аргументом
это не меняется.

**Выбор тележки** — [строки 60-62](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L60):

```xml
<xacro:include filename="$(arg robot_model).urdf.xacro"/>
<xacro:include filename="cart-drive-${drive}.urdf.xacro"/>
<xacro:include filename="cart-base.urdf.xacro"/>
```

Имя файла подставляется прямо из аргумента. `robot_model:=c2` → `c2.urdf.xacro`.
Порядок важен: файл модели идёт **первым**, чтобы его свойства (`arm_mount_xyz`,
`base_mesh`, `camera_offset_*`) уже существовали, когда их читают дальше.

**Блок симуляции** ([строки 64-110](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L64)):
при `use_sim=true` и `simulator=ignition` подключаются `cart.ign.xacro`,
`cart-sensors.ign.xacro` и плагин публикации углов ведущих колёс:

```xml
<plugin filename="gz-sim-joint-state-publisher-system"
        name="gz::sim::systems::JointStatePublisher">
  <topic>/model/cart/wheel_joint_states</topic>
  <joint_name>drive_wheel_left_joint</joint_name>
  <joint_name>drive_wheel_right_joint</joint_name>
</plugin>
```
([строки 101-108](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L101)).
Без него у колёс нет TF в RViz. Мост `gz_bridge.yaml` переносит топик в ROS как
`/cart/wheel_joint_states`.

**Рука** — [строки 116-117](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L116):
`include cart-arm.xacro` + вызов макроса `<xacro:mount_xarm6_on_cart/>`.

> ⚠️ Обрати внимание:
> - В отличие от `cart.urdf.xacro`, здесь **нет** блоков `ros2_control` батареи и
>   Dynamixel (ToF реального железа). Аргумент `enable_pace_battery` объявлен, но ни на что не влияет.
> - `var_wheel_type` (строка 67) нигде не читается — остаток оригинала.

### 4.2 `cart.urdf.xacro` — тележка без руки (оригинал)

Те же аргументы, но `robot_model` по умолчанию `c2`
([строка 4](../../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L4)).
Тот же порядок include ([строки 33-43](../../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L33)).
Дополнительно описывает `ros2_control` для **реального** робота:
`CartBatteryInterface` (плагин `quasi_battery_hardware/PaceBatteryHardware`,
[строки 45-76](../../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L45)) и
`CartDynamixelInterface` с сенсорами ToF через `dynamixel_hardware/DynamixelHardware`
(только при `use_sim=false`, [строки 78-443](../../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L78)).
Наши launch-файлы этот файл **не используют**; он полезен, чтобы посмотреть «чистую» тележку.

> ⚠️ Обрати внимание: плагины `quasi_battery_hardware` и `dynamixel_hardware` в
> этом workspace отсутствуют — на реальном железе файл рассчитан на полный r2_robot.

### 4.3 Файлы моделей тележки: `c2l-v3-scaled`, `c2l-v3`, `c2`

**`c2l-v3-scaled.urdf.xacro`** — всего две строки по делу
([строки 17-20](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3-scaled.urdf.xacro#L17)):

```xml
<xacro:property name="base_mesh" value="base_link_c2-v3.stl" />
<xacro:property name="base_mesh_scale" value="1.278 1.319 1" />

<xacro:include filename="c2l-v3.urdf.xacro" />
```

Он задаёт меш и масштаб **до** include, а `c2l-v3.urdf.xacro` объявляет те же
свойства с `default=` — поэтому значения из scaled-файла сохраняются. Масштаб
считается из габаритов (`0.46/0.36`, `0.31/0.235`, см. комментарий строк 8-12).
Масштаб идёт от начала `base_link`, поэтому ручка (-X) выходит на ~3 см дальше реальной.

**`c2l-v3.urdf.xacro`** — полная модель c2l:

```xml
<xacro:property name="base_mesh" default="base_link_c2l-v3.stl" />
<xacro:property name="base_mesh_scale" default="1 1 1" />

<!-- Крепление руки (cart-arm.xacro), base_link: по CAD (C2R.000.000.000)
     база руки смещена на 250 мм от центра к ручке (-X) и на 129.5 мм
     к левому краю, если стоять у ручки лицом вперёд (+Y); z — верх крыши -->
<xacro:property name="arm_mount_xyz" default="-0.25 -0.1295 0.849" />
```
([строки 7-14](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L7)).

Дальше в файле:
- `base_link` с мешем `${base_mesh}` и массой 35.5 кг ([строки 16-40](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L16));
- `drive_joint` (fixed): `drive_link → base_link`, `xyz="0.3625 0 0"` ([строка 42](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L42)) — корпус впереди оси ведущих колёс;
- суставы ToF 1–16, напольных 21–30 (с `xacro:if/unless` по `enable_floor_tofs`, `enable_extra_floor_tofs`), лидара (`z=0.428`) и IMU;
- ведущие колёса `drive_wheel_right_joint` / `drive_wheel_left_joint` с `y=∓0.25193` ([строки 290-325](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L290));
- позиция навигационной камеры: свойства `camera_offset_x/y/z`, `camera_roll/pitch/yaw` в блоках `enable_camera_realsense / oak / orbbec` ([строки 354-377](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L354)). Их читает `cart-base.urdf.xacro`.

Пример фрагмента ведущего колеса:

```xml
<joint name="drive_wheel_left_joint" type="revolute">
  <origin xyz="0 0.25193 0" rpy="0 0 0" />
  <parent link="drive_link" />
  <child link="drive_wheel_left_link" />
  <axis xyz="0 -1 0" />
  <limit lower="0" upper="0" effort="0" velocity="0" />
</joint>
```

> ⚠️ Обрати внимание: у ведущих колёс `revolute` с пределами `0..0` — в Gazebo они
> фактически не вращаются. Тележка ездит не колёсами, а плагином `VelocityControl`
> (см. 4.7), который двигает модель целиком.

**`c2.urdf.xacro`** (и его копия `c2-v3.urdf.xacro`) — старая/основная тележка:
меш `base_link_c2-v3.stl` без масштаба, масса 22.9 кг. Свойство `arm_mount_xyz`
**не задаёт**, поэтому срабатывает default из `cart-arm.xacro` (центр крыши
`-0.0075 0 0.849`).

**Остальные** (`c2h-v3`, `c2m-v3`, `c2t-v3`, `c2w-v3`) ссылаются на меши
`base_link_c2h-v3.stl`, `base_link_c2m-v3.stl`, `base_link_c2l-v3.stl`,
`base_link_c2w-v3.stl`, которых в `meshes/` нет — корпус не отобразится.
`arm_mount_xyz` в них не задан.

> ⚠️ Обрати внимание: `c2h-v3.urdf.xacro:2` называется `quasi_c2l-v3` (копипаста).
> Это имя `<robot>` во вложенном файле ни на что не влияет, но сбивает с толку.

### 4.4 `cart-base.urdf.xacro` — общая часть всех тележек

1023 строки, в основном CAD-экспорт звеньев датчиков. Важное:

```xml
<link name="base_footprint" />
<joint name="base_joint" type="fixed">
  <parent link="base_footprint" />
  <child link="drive_link" />
  <origin xyz="0 0 ${base_axis_z_offset}" rpy="0 0 0" />
</joint>
```
([строки 6-12](../../src/r2_robot/quasi_cart_description/urdf/cart-base.urdf.xacro#L6)).
`base_axis_z_offset` (высота оси колёс) приходит из файла привода: 0.065 для
differential, 0.07 для mecanum.

- Звенья ToF и т.п., условно: `xacro:unless robot_model.startswith('c2w')`
  (у c2w нет части датчиков, строки 158, 480), `enable_floor_tofs` (стр. 682),
  `enable_extra_floor_tofs` (стр. 822).
- **Камеры навигации** ([строки 999-1021](../../src/r2_robot/quasi_cart_description/urdf/cart-base.urdf.xacro#L999)):

```xml
<xacro:if value="$(arg enable_camera_realsense)">
  <xacro:include filename="_d455.urdf.xacro" />
  <xacro:sensor_d455 name="realsense" parent="base_link" use_nominal_extrinsics="true">
    <origin xyz="${camera_offset_x} ${camera_offset_y} ${camera_offset_z}"
            rpy="${camera_roll + camera_roll_offset} ${camera_pitch + camera_pitch_offset} ${camera_yaw}"/>
  </xacro:sensor_d455>
</xacro:if>
```

  `enable_camera_oak` подключает `depthai_descriptions`, `enable_camera_orbbec` —
  `orbbec_description` (joint `orbbec_camera_joint` на `camera_bottom_screw_frame`).

> ⚠️ Обрати внимание: пакетов `depthai_descriptions` и `orbbec_description` в
> workspace нет — `enable_camera_oak:=true` / `enable_camera_orbbec:=true` упадут в
> xacro. `_d455.urdf.xacro` требует `realsense2_description`. Все три флага по
> умолчанию `false` — это камеры **тележки**; камера на руке настраивается
> аргументом `camera_model` (см. 4.6 и главу про `quasi_camera_description`).

### 4.5 Приводы: `cart-drive-differential` и `cart-drive-mecanum`

- **differential** (используется, `drive="differential"`):
  `base_axis_z_offset = 0.065`
  ([строка 3](../../src/r2_robot/quasi_cart_description/urdf/cart-drive-differential.urdf.xacro#L3));
  описывает задние поворотные колёса — кронштейны `caster_wheel_*_bracket_link` и
  колёса `caster_wheel_right_link` / `caster_wheel_left_link`, все суставы `fixed`
  ([строки 166, 264](../../src/r2_robot/quasi_cart_description/urdf/cart-drive-differential.urdf.xacro#L166)).
  Сами ведущие колёса — в файле модели (`c2l-v3` и т.п.).
- **mecanum**: `base_axis_z_offset = 0.07`, 4 колеса `wheel_fr/fl/rr/rl` (revolute).

> ⚠️ Обрати внимание: mecanum-вариант нерабочий в этом пакете — меши
> `mecanum_wheel_*_link.STL` отсутствуют, а плагин `MecanumDrive` в
> `cart-sensors.ign.xacro` закомментирован. Чтобы включить, пришлось бы менять
> `drive` в `cart_with_arm.urdf.xacro:52`.

### 4.6 `cart-arm.xacro` — рука на тележке и вакуум

**Аргументы** ([строки 23-37](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L23)):
`arm_dof`, `arm_prefix`, `arm_hw_ns`, `arm_offset_xyz`, `arm_offset_rpy`,
`arm_ros2_control_plugin` (дублируют `cart_with_arm`; работают значения оттуда), плюс:

| Аргумент | Default | Смысл |
|---|---|---|
| `camera_model` | `gemini335le` | камера на фланце: `gemini335le` или `d435i` |
| `gripper_camera_preview` | `false` | облегчённый режим камеры (640x400) |
| `vacuum_target_model` | `pick_box` | имя gz-модели, которую может «приварить» присоска |

**Точка крепления** — [строки 44-55](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L44):

```xml
<xacro:property name="arm_mount_xyz" default="-0.0075 0.0 0.849"/>

<xacro:macro name="arm_mount_link" params="">
  <link name="arm_mount_link"/>
  <joint name="arm_mount_joint" type="fixed">
    <parent link="base_link"/>
    <child link="arm_mount_link"/>
    <origin xyz="${arm_mount_xyz}" rpy="0 0 0"/>
  </joint>
</xacro:macro>
```

`arm_mount_link` — пустое звено-«фланец» на крыше. Итоговое значение:
- `c2l-v3`, `c2l-v3-scaled` → `-0.25 -0.1295 0.849` (из `c2l-v3.urdf.xacro:14`);
- `c2` и остальные → `-0.0075 0.0 0.849` (default здесь).

**Подключение руки** — [строки 73-97](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L73):

```xml
<xacro:xarm_device
  dof="$(arg arm_dof)" robot_type="xarm"
  add_vacuum_gripper="true"
  add_realsense_d435i="${'$(arg camera_model)' == 'd435i'}"
  add_gripper_camera="${'$(arg camera_model)' != 'd435i'}"
  gripper_camera_model="$(arg camera_model)"
  gripper_camera_nominal="$(arg use_sim)"
  gripper_camera_gz="$(arg use_sim)"
  ros2_control_plugin="$(arg arm_ros2_control_plugin)"
  attach_to="arm_mount_link"
  attach_xyz="$(arg arm_offset_xyz)"
  attach_rpy="$(arg arm_offset_rpy)" ... />
```

Значит, положение базы руки = `arm_mount_xyz` (от `base_link`) **плюс**
`arm_offset_xyz/rpy` (от `arm_mount_link`). Первое — «где на этой тележке
посадочное место», второе — удобная доводка из launch без правки xacro.

**Начальная поза суставов** в этом пакете не задаётся. Она передаётся
сквозным xacro-аргументом `xarm_initial_positions` (launch-аргумент
`arm_initial_positions`, default `1.5708 -0.9 -1.3 0 1.0 0` рад, joint1…joint6,
[cart_xarm6_gz.launch.py:66-67](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L66))
и читается в
[xarm6.ros2_control.xacro:8](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L8)
как `initial_value` для gz_ros2_control. Пустая строка — все нули.

**Симуляция вакуума** — [строки 112-136](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L112), только при `use_sim=true`:

```xml
<plugin filename="gz-sim-detachable-joint-system"
        name="gz::sim::systems::DetachableJoint">
  <parent_link>link6</parent_link>
  <child_model>$(arg vacuum_target_model)</child_model>
  <child_link>link</child_link>
  <attach_topic>/vacuum/attach</attach_topic>
  <detach_topic>/vacuum/detach</detach_topic>
  <output_topic>/vacuum/state</output_topic>
  <suppress_child_warning>true</suppress_child_warning>
</plugin>
```

- Это не присоска с физикой, а **жёсткий сустав**, который создаётся/удаляется по
  команде: gz-топик `/vacuum/attach` → объект «прилип» в текущем положении,
  `/vacuum/detach` → отпущен, `/vacuum/state` → `attached`/`detached`.
- `parent_link = link6`, а не присоска: при переводе в SDF fixed-суставы
  схлопываются, и присоска/`link_eef`/камера становятся частью `link6`.
- `child_model` должен совпадать с моделью в мире (`pick_box` в
  `cart_table_offline.world`), `child_link` — имя её звена (`link`).
- Плагин приваривает объект при загрузке; нода `vacuum_gripper_sim` сразу делает
  detach и дальше решает, когда слать attach (проверяет касание).

Рядом `PosePublisher` ([строки 125-134](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L125))
публикует позу модели робота 10 Гц — для той же проверки касания.

Шапка файла ([строки 3-11](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L3))
кратко повторяет то же: точка крепления берётся из `arm_mount_xyz` (своя у
c2l-v3*, иначе центр крыши c2), доводка — через `arm_offset_xyz/rpy`.

### 4.7 `cart.ign.xacro` и `cart-sensors.ign.xacro` — плагины Gazebo

`.ign.xacro` — от «Ignition», старого имени Gazebo Sim.

**`cart.ign.xacro`**: блоки `<gazebo reference="…">` с цветом и трением
(`mu=0.05`, `mu2=0.01`) для `base_link`, `wheel_fr_link`, `wheel_fl_link`
([строки 8-66](../../src/r2_robot/quasi_cart_description/urdf/cart.ign.xacro#L8)).

> ⚠️ Обрати внимание: `wheel_fr_link` / `wheel_fl_link` есть только в mecanum-приводе;
> при differential эти блоки ссылаются на несуществующие звенья.

**`cart-sensors.ign.xacro`** — плагины всей модели:

| Плагин | Строки | Что делает |
|---|---|---|
| `VelocityControl` | [11-15](../../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L11) | двигает тележку по `/model/cart/cmd_vel` |
| `MecanumDrive` | 18-33 | закомментирован |
| `OdometryPublisher` | [36-45](../../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L36) | `/model/cart/odom`, `/model/cart/tf`, 20 Гц, `odom → base_footprint` |
| `Sensors` | [49-55](../../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L49) | только если `model_sensors_plugin` (у нас `false`) |
| `Imu` | 56-59 | система IMU |

Сенсоры: IMU на `imu_link` (`/model/cart/imu`, стр. 62), ToF 1–8 типа `gpu_ray`
(`/model/cart/tof_sensorN/scan`, обёрнуты в `enable_floor_tofs`, стр. 115-417),
лидар `gpu_lidar` на `lidar_link` (`/model/cart/lidar`, стр. 419), RealSense
`rgbd_camera` (`/model/cart/realsense`, только при `enable_camera_realsense`, стр. 458-501).
Имена топиков `/model/cart/*` завязаны на `cart_xarm_bringup/config/gz_bridge.yaml`.

> ⚠️ Обрати внимание:
> - Плагины записаны старыми именами `ignition-gazebo-*` / `ignition::gazebo::…`,
>   а в `cart-arm.xacro` — новыми `gz-sim-*`. Gazebo 9 пока понимает оба варианта.
> - ToF 1–8 (на крыше) обёрнуты в `enable_floor_tofs`, хотя по названию это флаг
>   напольных датчиков 21–26.
> - У ToF2 имя сенсора `tof_sensocart` (стр. 156) — опечатка, на топик не влияет.

### 4.8 `_d455.urdf.xacro`, `_d405.urdf.xacro`

Макросы RealSense (копии из `realsense2_description`). `_d455` подключается из
`cart-base` при `enable_camera_realsense`. `_d405` ни один файл не подключает.

---

## 5. Что здесь можно менять

После правок: `colcon build --packages-select quasi_cart_description` (или
`--symlink-install`, тогда правки в существующих xacro подхватываются сразу).

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Выбрать тележку при запуске | launch-аргумент / [cart_with_arm.urdf.xacro:21](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L21) | `robot_model` | `robot_model:=c2` |
| Сменить тележку по умолчанию | [cart_with_arm.urdf.xacro:21](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L21) + [cart_xarm6_gz.launch.py:371](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L371) | `default` | launch перекрывает xacro — менять в обоих |
| Сдвинуть посадочное место руки на c2l | [c2l-v3.urdf.xacro:14](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L14) | `arm_mount_xyz` | `default="-0.20 -0.1295 0.849"` |
| То же для c2 (и моделей без своего значения) | [cart-arm.xacro:44](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L44) | `arm_mount_xyz` | `default="0.05 0.0 0.849"` |
| Быстро подвинуть/повернуть руку без правки xacro | launch-аргументы → [cart-arm.xacro:95-96](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L95) | `arm_offset_xyz`, `arm_offset_rpy` | `arm_offset_rpy:="0 0 3.1416"` |
| Начальная поза руки в Gazebo | [cart_xarm6_gz.launch.py:67](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L67) | `arm_initial_positions` | `arm_initial_positions:="0 0 0 0 0 0"` |
| Сменить камеру на руке | [cart-arm.xacro:33](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L33) | `camera_model` | `camera_model:=d435i` |
| Присоска хватает другой объект | [cart-arm.xacro:37](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L37), [:118](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L118) | `vacuum_target_model`, `child_link` | `vacuum_target_model:=my_box` |
| Масштаб корпуса c2l-scaled | [c2l-v3-scaled.urdf.xacro:18](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3-scaled.urdf.xacro#L18) | `base_mesh_scale` | `"1.278 1.319 1"` |
| Включить доп. напольные ToF | [cart_with_arm.urdf.xacro:26](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L26) | `enable_extra_floor_tofs` | `xacro … enable_extra_floor_tofs:=true` |
| Позиция навигационной камеры c2l | [c2l-v3.urdf.xacro:354-377](../../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L354) | `camera_offset_*`, `camera_pitch` | `camera_pitch value="-0.1"` |
| Скользкость корпуса в Gazebo | [cart.ign.xacro:20](../../src/r2_robot/quasi_cart_description/urdf/cart.ign.xacro#L20) | `mu`, `mu2` | `<mu>0.5</mu>` |
| Топик управления движением тележки | [cart-sensors.ign.xacro:14](../../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L14) | `<topic>` | менять синхронно с `gz_bridge.yaml` |

**Добавить новую тележку:** создать `urdf/<имя>.urdf.xacro` (по образцу
`c2l-v3-scaled` — задать свойства и include базового файла, или полный файл как
`c2l-v3`), при необходимости задать в нём `arm_mount_xyz`, пересобрать пакет
(новый файл!), запустить с `robot_model:=<имя>`.

---

## 6. Частые ошибки

| Симптом | Причина | Что делать |
|---|---|---|
| `name 'robot_model' is not defined` | в верхнем xacro нет `xacro:property robot_model` | см. [cart_with_arm.urdf.xacro:51](../../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L51) |
| `Scene already exists with name: scene` + segfault | Sensors-система объявлена и в мире, и в модели | держать `model_sensors_plugin=false` |
| Тележка не появилась в Gazebo, рука «наполовину» | сенсор на несуществующем звене (так было с realsense) или меши не установлены в `share/` | проверить `xacro`/`check_urdf`, пересобрать пакет |
| Корпус невидим | у выбранной модели нет меша (`c2l-v3`, `c2h/m/t/w-v3`) | использовать `c2` или `c2l-v3-scaled` |
| `enable_camera_oak:=true` ломает xacro | нет пакета `depthai_descriptions` (аналогично orbbec) | не включать или установить пакет |
| Сдвинул `arm_mount_xyz` в `cart-arm.xacro`, а на c2l ничего не изменилось | `c2l-v3.urdf.xacro` задаёт своё значение раньше, `default=` в cart-arm уже не срабатывает | менять в файле модели |
| Новый файл модели «не найден» | xacro читается из `install/` | `colcon build --packages-select quasi_cart_description` |
| Присоска не цепляет объект | `vacuum_target_model` ≠ имени модели в world | сверить с `.world` |
| MoveIt планирует, будто руку не двигали | MoveIt использует свою модель руки | см. CLAUDE.md |

---

## 7. Проверь себя

1. **Сгенерируй URDF и найди точку крепления.**
   ```bash
   cd ~/ros2k_ws && source install/setup.bash
   D=$(ros2 pkg prefix quasi_cart_description)/share/quasi_cart_description/urdf
   xacro $D/cart_with_arm.urdf.xacro > /tmp/c2l.urdf && check_urdf /tmp/c2l.urdf | head
   grep -A4 'name="arm_mount_joint"' /tmp/c2l.urdf
   xacro $D/cart_with_arm.urdf.xacro robot_model:=c2 | grep -A4 'name="arm_mount_joint"'
   ```
   Почему значения разные? Какое правило xacro это обеспечивает?

2. **Сравни use_sim.** Посчитай `grep -c '<plugin'` в выводе xacro с
   `use_sim:=true` и без. Какие плагины добавились и из каких файлов?

3. **Сдвинь руку без правки файлов** в RViz:
   ```bash
   ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py robot_model:=c2l-v3-scaled
   ```
   Затем найди, передаёт ли `cart_xarm6_rviz.launch.py` аргумент `arm_offset_xyz`
   (подсказка: сравни со строками 82-91 `cart_xarm6_gz.launch.py`). Если нет — как
   проверить сдвиг через голый `xacro … arm_offset_xyz:="0.1 0 0"`?

4. **Вакуум.** Почему в `DetachableJoint` родитель — `link6`, а не
   `xarm_vacuum_gripper_link`? Проверь: `gz sdf -p /tmp/c2l_sim.urdf | grep "<link name"`
   (URDF сгенерируй с `use_sim:=true`).
