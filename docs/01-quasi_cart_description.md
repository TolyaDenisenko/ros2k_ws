# Глава 1. Пакет `quasi_cart_description`: тележка и рука на ней

> Путь: `src/r2_robot/quasi_cart_description`
> В этой главе: модель робота (URDF) — тележка quasi_cart, рука xArm6 на её крыше,
> «вакуумная присоска» для симуляции, плагины Gazebo, тележка без руки (`with_arm:=false`)
> и рука без тележки (`arm_on_table.urdf.xacro`).

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
| [cart_xarm6_gz.launch.py:90-106](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L90) | запускает `xacro cart_with_arm.urdf.xacro use_sim:=true robot_model:=… with_arm:=… …`, результат уходит в `robot_description` (его читают `robot_state_publisher` и Gazebo). Через него же работает `cart_nav_gz.launch.py` |
| [cart_xarm6_rviz.launch.py:49](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py#L49) | то же, но без симуляции — только просмотр модели в RViz |
| [arm_table_gz.launch.py](../src/r2_robot/cart_xarm_bringup/launch/arm_table_gz.launch.py) | сцена «рука на столе»: `arm_on_table.urdf.xacro` (рука без тележки, см. 4.9) |
| `xarm_description` (пакет из `src/xarm_ros2`) | наоборот, **используется этим пакетом**: макрос `xarm_device` рисует саму руку, присоску и камеру |
| [vacuum_gripper_sim.py](../src/xarm_grasping/scripts/vacuum_gripper_sim.py) | управляет плагином DetachableJoint, объявленным здесь, в `cart-arm.xacro` |

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
├── CMakeLists.txt                      — только устанавливает urdf/, meshes/, config/ в share/
├── config/
│   └── xarm6_controllers_sim.yaml      — контроллеры руки для симуляции (подключает cart-arm.xacro)
├── meshes/                             — 43 файла STL (корпуса c2 и c2l, колёса, ToF 1–16 и 21–36, лидар, IMU)
└── urdf/
    ├── cart_with_arm.urdf.xacro        — ГЛАВНЫЙ файл: тележка + рука (используют наши launch); with_arm:=false — без руки
    ├── arm_on_table.urdf.xacro         — та же рука без тележки, база закреплена в мире (сцена «рука на столе»)
    ├── cart.urdf.xacro                 — только тележка (оригинал r2_robot, + ros2_control реального железа)
    ├── cart-arm.xacro                  — крепление xArm6 к тележке + DetachableJoint вакуума
    ├── cart-base.urdf.xacro            — общие звенья: base_footprint, drive_link, ToF, лидар, IMU, камеры навигации
    ├── cart-drive-differential.urdf.xacro — звенья ведущих и поворотных (caster) колёс дифф. привода (используется)
    ├── cart-drive-mecanum.urdf.xacro   — 4 меканум-колеса (НЕ используется, мешей нет)
    ├── cart.ign.xacro                  — Gazebo: цвет и трение корпуса (блоки колёс — для меканум-варианта)
    ├── cart-sensors.ign.xacro          — Gazebo: привод DiffDrive, одометрия, IMU, сенсоры ToF, лидар, realsense
    ├── cart-front-camera.xacro         — передняя камера Orbbec 335Le (front_camera:=true)
    ├── c2.urdf.xacro                   — модель тележки c2 (по умолчанию в cart.urdf.xacro)
    ├── c2-v3.urdf.xacro                — побайтно совпадает с c2.urdf.xacro
    ├── c2l-v3.urdf.xacro               — тележка c2l (длинная, ПО УМОЛЧАНИЮ в наших launch), задаёт arm_mount_xyz и front_camera_*
    ├── c2h-v3 / c2m-v3 / c2t-v3 / c2w-v3.urdf.xacro — другие модели (не проверялись; меши корпуса c2h, c2m, c2w в пакете отсутствуют)
    ├── _d455.urdf.xacro                — макрос камеры RealSense D455 (навигационная камера тележки)
    └── _d405.urdf.xacro                — макрос RealSense D405 (нигде не подключается)
```

**package.xml / CMakeLists.txt.** Пакет типа `ament_cmake`, но ничего не компилирует.
[CMakeLists.txt:14](../src/r2_robot/quasi_cart_description/CMakeLists.txt#L14)
копирует папки `urdf meshes config launch rviz worlds` (с флагом `OPTIONAL` —
отсутствующие пропускаются; реально существуют `urdf`, `meshes` и `config`) в
`install/share/quasi_cart_description`. Именно оттуда их берут `$(find …)` и
launch-файлы. Зависимость от `xarm_description` в
[package.xml:20](../src/r2_robot/quasi_cart_description/package.xml#L20) нужна, потому
что `cart-arm.xacro` подключает макрос руки.

**Меши** (`meshes/`): `base_link_c2-v3.stl` и `base_link_c2l-v3.stl` (корпуса), `drive_link.stl`,
`drive_wheel_{left,right}_link.STL`, `wheel_{rl,rr}_link.stl`,
`wheel_bracket_{rl,rr}_link.stl`, `imu_link.STL`, `lidar_link.STL`, `tof1…16_link.STL`,
`tof21…36_link.STL` (меш c2l и ToF 27–36 взяты из репозитория тележки `quasi_ros_c1`). Пути в URDF пишутся как
`file://$(find quasi_cart_description)/meshes/...` — gz не умеет `package://` для наших
пакетов (см. CLAUDE.md).

---

## 3. Как собирается модель: цепочка include

```text
launch (cart_xarm6_gz.launch.py)
  └─ xacro cart_with_arm.urdf.xacro  use_sim:=true robot_model:=c2l-v3 with_arm:=true ...
       ├─ $(arg robot_model).urdf.xacro   → c2l-v3.urdf.xacro
       │     (base_link, суставы ToF, лидара, IMU и ведущих колёс, arm_mount_xyz, front_camera_*)
       ├─ cart-drive-differential.urdf.xacro  (звенья ведущих и поворотных колёс)
       ├─ cart-base.urdf.xacro            (base_footprint → drive_link, звенья датчиков, камеры enable_camera_*)
       │     └─ _d455.urdf.xacro           (только если enable_camera_realsense)
       ├─ [front_camera] cart-front-camera.xacro (передняя камера Orbbec, /front_camera/...)
       ├─ [use_sim] cart.ign.xacro        (трение/цвет корпуса)
       ├─ [use_sim] cart-sensors.ign.xacro (привод DiffDrive, одометрия, сенсоры gz)
       ├─ [use_sim] трение колёс + JointStatePublisher колёс
       └─ [with_arm] cart-arm.xacro → макрос mount_xarm6_on_cart
             ├─ arm_mount_link (+ joint от base_link в точке arm_mount_xyz)
             ├─ $(find xarm_description)/urdf/xarm_device_macro.xacro → рука, присоска, камера
             └─ [use_sim] DetachableJoint на каждую модель из vacuum_targets + PosePublisher
```

Дерево звеньев (упрощённо):

```text
base_footprint ─base_joint→ drive_link ─drive_joint→ base_link ─arm_mount_joint→ arm_mount_link → link_base (xArm) → link1 … link6 → link_eef → присоска, камера
                               ├─ drive_wheel_left/right_link            ├─ tof*_link, lidar_link, imu_link
                               └─ caster_wheel_*                         └─ front_camera_* (передняя камера)
```

При `with_arm:=false` ветки `arm_mount_link → …` нет: дерево заканчивается на тележке
и её датчиках.

Корень — `base_footprint` (точка на полу). `drive_link` поднят на
`base_axis_z_offset` (ось ведущих колёс), `base_link` (корпус) сдвинут от неё по X.

---

## 4. Разбор файлов

### 4.1 `cart_with_arm.urdf.xacro` — главный файл

**Аргументы** ([строки 20-49](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L20)):

| Аргумент | Default | Смысл |
|---|---|---|
| `use_sim` | `false` | `true` — добавить всё для Gazebo (launch gz передаёт `true`) |
| `robot_model` | `c2l-v3` | имя файла тележки `<robot_model>.urdf.xacro` |
| `simulator` | `ignition` | единственный поддерживаемый вариант (= gz sim) |
| `enable_floor_tofs` | `true` | напольные ToF-датчики 21–26 |
| `enable_extra_floor_tofs` | `false` | доп. ToF 27–30 (есть у c2l) |
| `enable_camera_realsense` / `_oak` / `_orbbec` | `false` | старые варианты навигационной камеры из r2_robot (НЕ камера на руке!) |
| `camera_roll_offset`, `camera_pitch_offset` | `0.0` | доводка наклона навигационной камеры |
| `front_camera` | `true` | передняя камера тележки Orbbec Gemini 335Le (`cart-front-camera.xacro`, топики `/front_camera/...`) |
| `enable_pace_battery` | `true` | объявлен, но в этом файле не используется (см. ⚠️ ниже) |
| `model_sensors_plugin` | `false` | НЕ добавлять gz Sensors-систему в модель |
| `with_arm` | `true` | `false` — **только тележка** с её датчиками, без руки, камеры запястья и вакуума (лёгкая симуляция для езды и парковки; launch-аргумент `arm`) |
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
([строки 33-38](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L33)).
Здесь работает правило «первое объявление arg побеждает»: в
`cart-sensors.ign.xacro:8` default `true`, но он уже не действует.

**Свойства** ([строки 51-57](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L51)):

```xml
<xacro:property name="var_simulator" value="$(arg simulator)"/>
<xacro:property name="robot_model" value="$(arg robot_model)"/>
<xacro:property name="drive" value="differential"/>
```

`robot_model` копируется из arg в property, потому что `cart-base.urdf.xacro` пишет
выражения вида `${robot_model.startswith('c2w')}` (а в `${…}` видны только
property). `drive` выбирает файл привода — **жёстко** `differential`, аргументом
это не меняется.

**Выбор тележки** — [строки 65-71](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L65):

```xml
<xacro:include filename="$(arg robot_model).urdf.xacro"/>
<xacro:include filename="cart-drive-${drive}.urdf.xacro"/>
<xacro:include filename="cart-base.urdf.xacro"/>
<xacro:if value="$(arg front_camera)">
    <xacro:include filename="cart-front-camera.xacro"/>
    <xacro:cart_front_camera add_gz_sensor="$(arg use_sim)"/>
</xacro:if>
```

Имя файла подставляется прямо из аргумента. `robot_model:=c2` → `c2.urdf.xacro`.
Порядок важен: файл модели идёт **первым**, чтобы его свойства (`arm_mount_xyz`,
`front_camera_*`, `camera_offset_*`) уже существовали, когда их читают дальше.

**Блок симуляции** ([строки 73-132](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L73)):
при `use_sim=true` и `simulator=ignition` подключаются `cart.ign.xacro`,
`cart-sensors.ign.xacro`, трение колёс и плагин публикации углов ведущих колёс.

Трение ([строки 104-113](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L104)):
ведущие колёса держатся за пол (`mu1 = mu2 = 1.5`), поворотные колёса (кастеры)
скользят (`0.02`), иначе тележка не повернёт. Кастеры прикреплены `fixed`-суставами;
чтобы Gazebo не «склеил» их с корпусом и не потерял их трение, для этих суставов задано
`preserveFixedJoint`.

```xml
<plugin filename="gz-sim-joint-state-publisher-system"
        name="gz::sim::systems::JointStatePublisher">
  <topic>/model/cart/wheel_joint_states</topic>
  <update_rate>30</update_rate>
  <joint_name>drive_wheel_left_joint</joint_name>
  <joint_name>drive_wheel_right_joint</joint_name>
</plugin>
```
([строки 120-130](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L120)).
Без него у колёс нет TF в RViz. Мост `gz_bridge.yaml` переносит топик в ROS как
`/cart/wheel_joint_states`. `update_rate` обязателен: без него плагин публикует на
каждом шаге физики (~1000 Гц) и зря грузит процессор.

**Рука** — [строки 138-141](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L138):
`include cart-arm.xacro` + вызов макроса `<xacro:mount_xarm6_on_cart/>`, обёрнутые в
`<xacro:if value="$(arg with_arm)">`.

**Тележка без руки.** С `with_arm:=false` в модели остаются только тележка и её
датчики (лидар, ToF, IMU, передняя камера): нет руки, камеры на запястье, вакуума и
плагина `gz_ros2_control`. Такую модель запускает launch-аргумент `arm:=false`
(`cart_xarm6_gz.launch.py`, в `cart_nav_gz.launch.py` это значение по умолчанию) — он же
не запускает контроллеры руки, MoveIt и ноды захвата. Проверить без запуска:

```bash
xacro $(ros2 pkg prefix quasi_cart_description)/share/quasi_cart_description/urdf/cart_with_arm.urdf.xacro \
  with_arm:=false > /tmp/cart_only.urdf && check_urdf /tmp/cart_only.urdf | head -3
```

> ⚠️ Обрати внимание:
> - В отличие от `cart.urdf.xacro`, здесь **нет** блоков `ros2_control` батареи и
>   Dynamixel (ToF реального железа). Аргумент `enable_pace_battery` объявлен, но ни на что не влияет.
> - `var_wheel_type` (строка 76) нигде не читается — остаток оригинала.

### 4.2 `cart.urdf.xacro` — тележка без руки (оригинал)

Те же аргументы, но `robot_model` по умолчанию `c2`
([строка 4](../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L4)).
Тот же порядок include ([строки 33-41](../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L33)), но без передней камеры, трения колёс и руки.
Дополнительно описывает `ros2_control` для **реального** робота:
`CartBatteryInterface` (плагин `quasi_battery_hardware/PaceBatteryHardware`,
[строки 45-76](../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L45)) и
`CartDynamixelInterface` с сенсорами ToF через `dynamixel_hardware/DynamixelHardware`
(только при `use_sim=false`, [строки 78-443](../src/r2_robot/quasi_cart_description/urdf/cart.urdf.xacro#L78)).
Наши launch-файлы этот файл **не используют**; он полезен, чтобы посмотреть «чистую» тележку.

> ⚠️ Обрати внимание: плагины `quasi_battery_hardware` и `dynamixel_hardware` в
> этом workspace отсутствуют — на реальном железе файл рассчитан на полный r2_robot.

### 4.3 Файлы моделей тележки: `c2l-v3`, `c2`

**`c2l-v3.urdf.xacro`** — полная модель c2l (меш корпуса `base_link_c2l-v3.stl`
взят из репозитория тележки `quasi_ros_c1`):

```xml
<!-- Крепление руки (cart-arm.xacro), base_link: по CAD (C2R.000.000.000)
     база руки смещена на 250 мм от центра к ручке (-X) и на 129.5 мм
     к левому краю, если стоять у ручки лицом вперёд (+Y);
     z — верх крыши, та же высота, что у c2 -->
<xacro:property name="arm_mount_xyz" default="-0.25 -0.1295 0.849" />

<!-- Передняя камера (cart-front-camera.xacro): ОЦЕНКА — поза c2 ... + 0.1 м вперёд -->
<xacro:property name="front_camera_xyz" default="0.409124 0.0 0.839651" />
<xacro:property name="front_camera_rpy" default="${-pi} 0.041 0.0" />
```
([строки 5-15](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L5)).

> ⚠️ Обрати внимание: комментарий говорит «к левому краю (+Y)», а значение
> `y = -0.1295` — это **−Y**, то есть правый край, если стоять у ручки лицом вперёд.
> Какой край верный, надо сверить с реальной тележкой (см. [CART_MODELS.md](CART_MODELS.md), раздел 3).

Дальше в файле:
- `base_link` с мешем `base_link_c2l-v3.stl` и массой 35.5 кг ([строки 17-40](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L17));
- `drive_joint` (fixed): `drive_link → base_link`, `xyz="0.3625 0 0"` ([строка 41](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L41)) — корпус впереди оси ведущих колёс;
- суставы ToF 1–16, напольных 21–30 (с `xacro:if/unless` по `enable_floor_tofs`, `enable_extra_floor_tofs`), лидара и IMU;
- `sim_wheel_separation = 0.50386` — расстояние между ведущими колёсами для привода `DiffDrive` в Gazebo ([строка 292](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L292)); у c2 — `0.354`;
- ведущие колёса `drive_wheel_right_joint` / `drive_wheel_left_joint` с `y=∓0.25193` ([строки 294-325](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L294));
- позиция навигационной камеры: свойства `camera_offset_x/y/z`, `camera_roll/pitch/yaw` в блоках `enable_camera_realsense / oak / orbbec` ([строки 350-373](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L350)). Их читает `cart-base.urdf.xacro`.

Пример фрагмента ведущего колеса:

```xml
<joint name="drive_wheel_left_joint" type="continuous">
  <origin xyz="0 0.25193 0" rpy="0 0 0" />
  <parent link="drive_link" />
  <child link="drive_wheel_left_link" />
  <axis xyz="0 1 0" />
  <limit effort="60" velocity="20" />
</joint>
```

Колёса — `continuous` (вращаются без пределов), у обоих ось `+y`: плагин `DiffDrive`
(см. 4.7) подаёт один знак скорости на оба колеса. В исходной модели из `quasi_ros_c1`
колёса были `revolute` с пределами `0..0` и левой осью `-y` — тогда тележку двигал
`VelocityControl` целиком, а колёса не крутились.

**`c2.urdf.xacro`** (и его копия `c2-v3.urdf.xacro`) — старая/основная тележка:
меш `base_link_c2-v3.stl` без масштаба, масса 22.9 кг, ведущие колёса `y=±0.177`.
Свойство `arm_mount_xyz` **не задаёт**, поэтому срабатывает default из `cart-arm.xacro`
(центр крыши `-0.0075 0 0.849`). `front_camera_*` тоже не задаёт — берётся default из
`cart-front-camera.xacro` (`0.309124 0 0.839651`).

**Остальные** (`c2h-v3`, `c2m-v3`, `c2t-v3`, `c2w-v3`) не проверялись. `c2h`, `c2m`, `c2w`
ссылаются на меши `base_link_c2h-v3.stl`, `base_link_c2m-v3.stl`, `base_link_c2w-v3.stl`,
которых в `meshes/` нет — корпус не отобразится. `c2t-v3` использует меш c2l
(`base_link_c2l-v3.stl`), он есть. `arm_mount_xyz` ни в одной из них не задан.

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
([строки 6-12](../src/r2_robot/quasi_cart_description/urdf/cart-base.urdf.xacro#L6)).
`base_axis_z_offset` (высота оси колёс) приходит из файла привода: 0.065 для
differential, 0.07 для mecanum.

- Звенья ToF и т.п., условно: `xacro:unless robot_model.startswith('c2w')`
  (у c2w нет части датчиков, строки 158, 480), `enable_floor_tofs` (стр. 682),
  `enable_extra_floor_tofs` (стр. 822).
- **Камеры навигации** ([строки 999-1021](../src/r2_robot/quasi_cart_description/urdf/cart-base.urdf.xacro#L999)):

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
  ([строка 3](../src/r2_robot/quasi_cart_description/urdf/cart-drive-differential.urdf.xacro#L3));
  описывает звенья ведущих колёс `drive_wheel_{right,left}_link` и поворотные колёса —
  кронштейны `caster_wheel_*_bracket_link` и колёса `caster_wheel_right_link` /
  `caster_wheel_left_link` (суставы кастеров `fixed`,
  [строки 158, 253](../src/r2_robot/quasi_cart_description/urdf/cart-drive-differential.urdf.xacro#L158)).
  Суставы ведущих колёс — в файле модели (`c2l-v3` и т.п.).
  **Коллизии колёс — цилиндры** (ведущие r = 0.065 м, кастеры r = 0.062 м), а не STL-меши:
  на мешах тележка подпрыгивала и проскальзывала при езде (STATUS.md, 2026-10-29).
  Меши остались только во `<visual>`.
- **mecanum**: `base_axis_z_offset = 0.07`, 4 колеса `wheel_fr/fl/rr/rl` (revolute).

> ⚠️ Обрати внимание: mecanum-вариант нерабочий в этом пакете — меши
> `mecanum_wheel_*_link.STL` отсутствуют, а плагин `MecanumDrive` в
> `cart-sensors.ign.xacro` закомментирован. Чтобы включить, пришлось бы менять
> `drive` в `cart_with_arm.urdf.xacro:57`.

### 4.6 `cart-arm.xacro` — рука на тележке и вакуум

**Аргументы** ([строки 23-40](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L23)):
`arm_dof`, `arm_prefix`, `arm_hw_ns`, `arm_offset_xyz`, `arm_offset_rpy`,
`arm_ros2_control_plugin` (дублируют `cart_with_arm`; работают значения оттуда), плюс:

| Аргумент | Default | Смысл |
|---|---|---|
| `camera_model` | `gemini335le` | камера на фланце: `gemini335le` или `d435i` |
| `gripper_camera_preview` | `false` | облегчённый режим камеры (640x400) |
| `vacuum_targets` | `pick_box` | gz-модели, которые может «приварить» присоска, через пробел; `none` — ни одной |

Ещё один сквозной аргумент — `xarm_initial_positions` (начальная поза руки, см. ниже).

**Точка крепления** — [строки 67-78](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L67):

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
- `c2l-v3` → `-0.25 -0.1295 0.849` (из `c2l-v3.urdf.xacro:14`);
- `c2` и остальные → `-0.0075 0.0 0.849` (default здесь).

**Подключение руки** — [строки 96-120](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L96):

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
  ros2_control_params="$(find quasi_cart_description)/config/xarm6_controllers_sim.yaml"
  attach_to="arm_mount_link"
  attach_xyz="$(arg arm_offset_xyz)"
  attach_rpy="$(arg arm_offset_rpy)" ... />
```

Значит, положение базы руки = `arm_mount_xyz` (от `base_link`) **плюс**
`arm_offset_xyz/rpy` (от `arm_mount_link`). Первое — «где на этой тележке
посадочное место», второе — удобная доводка из launch без правки xacro.

Контроллеры руки берутся из `config/xarm6_controllers_sim.yaml` этого пакета — копии
`xarm_controller/config/xarm6_controllers.yaml` с двумя отличиями для Gazebo: траекторный
контроллер управляет только `position` (иначе `gz_ros2_control` исполнял только скорость и
рука «плыла»), и `position_proportional_gain: 1.0` (с 0.1 рука почти не держала позу,
с 3 и выше дрожала). Подробности — в комментарии в начале файла.

**Начальная поза суставов** в этом пакете не задаётся. Она передаётся
сквозным xacro-аргументом `xarm_initial_positions` (launch-аргумент
`arm_initial_positions`, default `1.5708 -0.9 -1.3 0 1.0 0` рад, joint1…joint6,
[cart_xarm6_gz.launch.py:66-68](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L66);
в навигации `cart_nav_gz` — своя поза `1.5708 0.06 -0.48 0 0.47 0`, «перевёрнутая U»)
и читается в
[xarm6.ros2_control.xacro:8](../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L8)
как `initial_value` для gz_ros2_control. Пустая строка — все нули.

**Симуляция вакуума** — [строки 135-150](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L135), только при `use_sim=true`.
На **каждую** модель из `vacuum_targets` макрос `vacuum_joints`
([строки 44-60](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L44)) добавляет
свой плагин (в xacro нет циклов, поэтому макрос рекурсивно вызывает сам себя для остатка списка):

```xml
<plugin filename="gz-sim-detachable-joint-system"
        name="gz::sim::systems::DetachableJoint">
  <parent_link>link6</parent_link>
  <child_model>${targets[0]}</child_model>
  <child_link>link</child_link>
  <attach_topic>/vacuum/${targets[0]}/attach</attach_topic>
  <detach_topic>/vacuum/${targets[0]}/detach</detach_topic>
  <output_topic>/vacuum/${targets[0]}/state</output_topic>
  <suppress_child_warning>true</suppress_child_warning>
</plugin>
```

- Это не присоска с физикой, а **жёсткий сустав**, который создаётся/удаляется по
  команде: gz-топик `/vacuum/<модель>/attach` → объект «прилип» в текущем положении,
  `/vacuum/<модель>/detach` → отпущен, `/vacuum/<модель>/state` → `attached`/`detached`.
- `parent_link = link6`, а не присоска: при переводе в SDF fixed-суставы
  схлопываются, и присоска/`link_eef`/камера становятся частью `link6`.
- Имена в `vacuum_targets` должны совпадать с моделями в мире (`pick_box` в
  `cart_table_offline.world`), у каждой — звено `link`, модель **не** `static`.
- Плагин приваривает объект при загрузке; нода `vacuum_gripper_sim` сразу делает
  detach и дальше решает, когда слать attach (проверяет касание).
- ⚠️ Если `vacuum_gripper_sim` не запущена, коробка так и останется приваренной к руке, и
  тележка потащит её за собой. Поэтому при `manipulation_nodes:=false` launch передаёт
  `vacuum_targets:=none` (плагинов нет вовсе).

Рядом `PosePublisher` ([строки 137-149](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L137))
публикует позу модели робота 10 Гц — для той же проверки касания.

Шапка файла ([строки 3-18](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L3))
кратко повторяет то же: точка крепления берётся из `arm_mount_xyz` (своя у
c2l-v3*, иначе центр крыши c2), доводка — через `arm_offset_xyz/rpy`.

### 4.7 `cart.ign.xacro` и `cart-sensors.ign.xacro` — плагины Gazebo

`.ign.xacro` — от «Ignition», старого имени Gazebo Sim.

**`cart.ign.xacro`**: блоки `<gazebo reference="…">` с цветом и трением
(`mu=0.05`, `mu2=0.01`) для `base_link`, `wheel_fr_link`, `wheel_fl_link`
([строки 8-66](../src/r2_robot/quasi_cart_description/urdf/cart.ign.xacro#L8)).

> ⚠️ Обрати внимание: `wheel_fr_link` / `wheel_fl_link` есть только в mecanum-приводе;
> при differential эти блоки ссылаются на несуществующие звенья. Трение колёс c2/c2l
> задаётся в `cart_with_arm.urdf.xacro` (см. 4.1).

**`cart-sensors.ign.xacro`** — плагины всей модели:

| Плагин | Строки | Что делает |
|---|---|---|
| `DiffDrive` | [17-42](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L17) | **привод по умолчанию** (`sim_drive:=diff`): крутит `drive_wheel_*_joint` по `/model/cart/cmd_vel`, тележка едет за счёт сцепления колёс с полом. Колея — `sim_wheel_separation` из файла модели, радиус `sim_wheel_radius` = 0.065. Своя одометрия уведена на `/model/cart/diff_odom` |
| `VelocityControl` | [43-51](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L43) | старый привод, только при xacro-аргументе `sim_drive:=velocity`: двигает корпус целиком, колёса не вращаются; с рукой тележка на этом приводе заваливалась |
| `MecanumDrive` | 53-68 | закомментирован |
| `OdometryPublisher` | [70-81](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L70) | `/model/cart/odom`, `/model/cart/tf`, 20 Гц, `odom → base_footprint` |
| `Sensors` | [83-90](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L83) | только если `model_sensors_plugin` (у нас `false`) |
| `Imu` | 91-94 | система IMU |

Сенсоры: IMU на `imu_link` (`/model/cart/imu`, 50 Гц, стр. 97), ToF 1–8 типа `gpu_ray`
(`/model/cart/tof_sensorN/scan`, обёрнуты в `enable_floor_tofs`, стр. 151-461),
лидар `gpu_lidar` на `lidar_link` (`/model/cart/lidar`, стр. 465), RealSense
`rgbd_camera` (`/model/cart/realsense`, только при `enable_camera_realsense`, стр. 503-547).
Имена топиков `/model/cart/*` завязаны на мосты `cart_xarm_bringup/config/gz_bridge*.yaml`.
В ROS они выходят так: `/cmd_vel`, `/odom` (`gz_bridge.yaml`); лидар `/scan_raw` → фильтр
стоек → `/scan`, ToF `/range1_pcl` … `/range8_pcl`, IMU `/imu` (только в навигации: `gz_bridge_cart_nav.yaml`);
TF одометрии `/tf` — только в навигации (`gz_bridge_cart_nav.yaml`). Подробнее — глава 2.

> ⚠️ Обрати внимание:
> - Плагины записаны старыми именами `ignition-gazebo-*` / `ignition::gazebo::…`,
>   а в `cart-arm.xacro` и в `DiffDrive` — новыми `gz-sim-*`. Gazebo 9 пока понимает оба варианта.
> - ToF 1–8 (на крыше) обёрнуты в `enable_floor_tofs`, хотя по названию это флаг
>   напольных датчиков 21–26.
> - У ToF2 имя сенсора `tof_sensocart` (стр. 193) — опечатка, на топик не влияет.

### 4.8 `_d455.urdf.xacro`, `_d405.urdf.xacro`

Макросы RealSense (копии из `realsense2_description`). `_d455` подключается из
`cart-base` при `enable_camera_realsense`. `_d405` ни один файл не подключает.

### 4.9 `cart-front-camera.xacro` — передняя камера тележки

Камера Orbbec Gemini 335Le на `base_link`, топики и кадры `/front_camera/...`,
`front_camera_*_optical_frame`. Тело камеры — общий макрос из `quasi_camera_description`
(тот же, что у камеры на запястье, глава 3), gz-сенсор — облегчённый 640×400.
Поза — свойства модели `front_camera_xyz` / `front_camera_rpy` (нижний винт камеры в
`base_link`); если модель их не задала, берётся поза c2 `0.309124 0 0.839651`.
Камера висит под передним краем крыши **вверх ногами** (`roll = -π`), как на реальной
тележке, поэтому картинка в ROS перевёрнута на 180°. Включается аргументом
`front_camera` (по умолчанию `true`).

### 4.10 `arm_on_table.urdf.xacro` — рука без тележки

Для сценария «рука на столе» (`arm_table_gz.launch.py`, глава 8): тот же макрос
`mount_xarm6_on_cart` из `cart-arm.xacro` (рука, присоска, камера, вакуум), но вместо
тележки — пустые звенья `world → base_link`, а `arm_mount_xyz = 0 0 0`. Поэтому кадры
`world`, `base_link`, `arm_mount_link` и `link_base` совпадают, и кадр `world` совпадает с
кадром планирования MoveIt. Высоту над полом задаёт поза спавна в Gazebo.
Default `vacuum_targets` в файле — `pick_box cylinder_block` (устарел), но launch передаёт
свой список — имена всех 7 предметов сцены из `cart_xarm_bringup/config/arm_table_objects.yaml`
(глава 8).

---

## 5. Что здесь можно менять

После правок: `colcon build --packages-select quasi_cart_description` (или
`--symlink-install`, тогда правки в существующих xacro подхватываются сразу).

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Выбрать тележку при запуске | launch-аргумент / [cart_with_arm.urdf.xacro:21](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L21) | `robot_model` | `robot_model:=c2` |
| Сменить тележку по умолчанию | [cart_with_arm.urdf.xacro:21](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L21) + [cart_xarm6_gz.launch.py:502-503](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L502) | `default` | launch перекрывает xacro — менять в обоих (и в `cart_xarm6_rviz.launch.py`) |
| Тележка без руки | [cart_with_arm.urdf.xacro:43](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L43) / launch-аргумент `arm` | `with_arm` | `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm:=false` |
| Сдвинуть посадочное место руки на c2l | [c2l-v3.urdf.xacro:9](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L9) | `arm_mount_xyz` | `default="-0.20 -0.1295 0.849"` |
| То же для c2 (и моделей без своего значения) | [cart-arm.xacro:67](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L67) | `arm_mount_xyz` | `default="0.05 0.0 0.849"` |
| Быстро подвинуть/повернуть руку без правки xacro | launch-аргументы → [cart-arm.xacro:118-119](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L118) | `arm_offset_xyz`, `arm_offset_rpy` | `arm_offset_rpy:="0 0 3.1416"` |
| Начальная поза руки в Gazebo | [cart_xarm6_gz.launch.py:67](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L67) | `arm_initial_positions` | `arm_initial_positions:="0 0 0 0 0 0"` |
| Сменить камеру на руке | [cart-arm.xacro:33](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L33) | `camera_model` | `camera_model:=d435i` |
| Присоска хватает другие объекты | [cart-arm.xacro:40](../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L40) | `vacuum_targets` | `vacuum_targets:="pick_box my_box"` (+ мост `/vacuum/<модель>/*` и параметры `vacuum_gripper_sim`) |
| Поза передней камеры c2l | [c2l-v3.urdf.xacro:14-15](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L14) | `front_camera_xyz`, `front_camera_rpy` | `default="0.41 0 0.84"` |
| Выключить переднюю камеру | [cart_with_arm.urdf.xacro:31](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L31) / launch-аргумент | `front_camera` | `front_camera:=false` |
| Включить доп. напольные ToF | [cart_with_arm.urdf.xacro:26](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L26) | `enable_extra_floor_tofs` | `xacro … enable_extra_floor_tofs:=true` |
| Позиция навигационной камеры c2l | [c2l-v3.urdf.xacro:350-373](../src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro#L350) | `camera_offset_*`, `camera_pitch` | `camera_pitch value="-0.1"` |
| Сцепление колёс с полом | [cart_with_arm.urdf.xacro:108-113](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L108) | `mu1`, `mu2` | ведущие 1.5, кастеры 0.02 |
| Привод тележки в Gazebo | [cart-sensors.ign.xacro:15](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L15) | xacro-аргумент `sim_drive` | `diff` (по умолчанию) \| `velocity` |
| Пределы скорости/ускорения привода | [cart-sensors.ign.xacro:27-34](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L27) | `max_linear_velocity` и др. | согласовать с `nav2_cart.yaml` |
| Топик управления движением тележки | [cart-sensors.ign.xacro:20](../src/r2_robot/quasi_cart_description/urdf/cart-sensors.ign.xacro#L20) | `<topic>` | менять синхронно с `gz_bridge.yaml` |

**Добавить новую тележку:** создать `urdf/<имя>.urdf.xacro` (полный файл по
образцу `c2l-v3`), при необходимости задать в нём `arm_mount_xyz` и `front_camera_*`, пересобрать пакет
(новый файл!), запустить с `robot_model:=<имя>`.

---

## 6. Частые ошибки

| Симптом | Причина | Что делать |
|---|---|---|
| `name 'robot_model' is not defined` | в верхнем xacro нет `xacro:property robot_model` | см. [cart_with_arm.urdf.xacro:56](../src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro#L56) |
| `Scene already exists with name: scene` + segfault | Sensors-система объявлена и в мире, и в модели | держать `model_sensors_plugin=false` |
| Тележка не появилась в Gazebo, рука «наполовину» | сенсор на несуществующем звене (так было с realsense) или меши не установлены в `share/` | проверить `xacro`/`check_urdf`, пересобрать пакет |
| Корпус невидим | у выбранной модели нет меша (`c2h/m/t/w-v3`) | использовать `c2` или `c2l-v3` |
| `enable_camera_oak:=true` ломает xacro | нет пакета `depthai_descriptions` (аналогично orbbec) | не включать или установить пакет |
| Сдвинул `arm_mount_xyz` в `cart-arm.xacro`, а на c2l ничего не изменилось | `c2l-v3.urdf.xacro` задаёт своё значение раньше, `default=` в cart-arm уже не срабатывает | менять в файле модели |
| Новый файл модели «не найден» | xacro читается из `install/` | `colcon build --packages-select quasi_cart_description` |
| Присоска не цепляет объект | имени модели нет в `vacuum_targets` или оно ≠ имени в world | сверить с `.world` |
| Тележка тащит за собой `pick_box` и опрокидывается | плагин DetachableJoint приварил коробку при загрузке, а `vacuum_gripper_sim` не запущена | `vacuum_targets:=none` (в launch: `manipulation_nodes:=false`) |
| Тележка подпрыгивает/скользит при езде | коллизии колёс вернули на STL-меши или сняли трение | цилиндры в `cart-drive-differential`, трение в `cart_with_arm` (4.1, 4.5) |
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
   ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py robot_model:=c2l-v3
   ```
   Затем найди, передаёт ли `cart_xarm6_rviz.launch.py` аргумент `arm_offset_xyz`
   (подсказка: сравни со строками 90-106 `cart_xarm6_gz.launch.py`). Если нет — как
   проверить сдвиг через голый `xacro … arm_offset_xyz:="0.1 0 0"`?

4. **Вакуум.** Почему в `DetachableJoint` родитель — `link6`, а не
   `xarm_vacuum_gripper_link`? Проверь: `gz sdf -p /tmp/c2l_sim.urdf | grep "<link name"`
   (URDF сгенерируй с `use_sim:=true`).

5. **Тележка без руки.** Сгенерируй URDF с `with_arm:=false` и сравни число звеньев:
   ```bash
   xacro $D/cart_with_arm.urdf.xacro | grep -c '<link '
   xacro $D/cart_with_arm.urdf.xacro with_arm:=false | grep -c '<link '
   xacro $D/cart_with_arm.urdf.xacro with_arm:=false use_sim:=true | grep -c 'gz_ros2_control'
   ```
   Почему в последней команде 0? Что ещё перестаёт запускать launch при `arm:=false`
   (подсказка: глава 2, `GroupAction`)?
