# Инструкция по установке и запуску (ros2k_ws)

Рабочее пространство ROS 2 с симуляцией **xArm6 + вакуумный гриппер (присоска)**
в **Gazebo Sim** и **RViz**, анализатором вариантов захвата цилиндрического блока
и тележкой **r2_robot (quasi_cart)** с установленной по центру руки платформой.

Два независимых сценария запуска:

| Сценарий | Что запускается | Launch-файл |
|---|---|---|
| **A. Рука отдельно** | xArm6 на столе, Gazebo + RViz + MoveIt | `xarm_gz_bringup/xarm6_gz.launch.py` |
| **B. Тележка + рука** | quasi_cart с xArm6 на верхней платформе, Gazebo + RViz + grasp_analyzer | `cart_xarm_bringup/cart_xarm6_gz.launch.py` |

---

## 1. Требования к системе

- **Ubuntu 22.04 + ROS 2 Humble** (или Ubuntu 24.04 + Jazzy — тогда замените
  `gz_ros2_control`-плагин на `gz_ros2_control::GazeboSimSystem`, имя пакета то же).
- **Gazebo Sim (Harmonic/Garden)**, ставится вместе с ROS-пакетами ниже.
- Python 3.8+.

### Почему именно так
Все миры в репозитории сделаны **оффлайн-совместимыми** (меши примитивов
генерируются процедурно, без скачивания с Fuel), чтобы запуск работал
на машине без интернета и без лишних таймаутов.

## 2. Установка системных зависимостей

```bash
sudo apt update
sudo apt install -y \
  ros-humble-gazebo-ros-pkg \        # не нужен, если стоит gz_ros2_control (см. ниже)
  python3-colcon-common-extensions \
  python3-xacro \
  ros-humble-ros-gz \                # мост ROS2 <-> Gazebo Sim (gz_bridge, gz_sim)
  ros-humble-gz-ros2-control \       # контроллеры ros2_control внутри Gazebo
  ros-humble-ros2-control \
  ros-humble-ros2-controllers \
  ros-humble-moveit \
  ros-humble-visualization-msgs \
  ros-humble-sensor-msgs \
  ros-humble-pcl-conversions \
  ros-humble-tf2-tools \
  python3-numpy
```

> ⚠️ Если у вас **ROS 2 Jazzy**: пакеты называются так же, только префикс
> `ros-jazzy-`. В Humble вместо Harmonic ставится Garden — миры `.world`
> совместимы с обеими версиями (SDF 1.8+).

### Пакеты из репозитория (ставятся через colcon, не apt)
- `xarm_grasping` требует `numpy` (уже выше). Анализатор работает **только на
  стандартных сообщениях ROS** (PointCloud2/Image/MarkerArray) — это сделано
  специально, чтобы не зависеть от PCL/Open3D и собирать облако точек «в лоб»
  numpy-ом: быстрее ставится, проще отлаживается.

## 3. Клонирование и сборка

```bash
mkdir -p ~/ros2k_ws/src && cd ~/ros2k_ws
git clone https://github.com/TolyaDenisenko/ros2k_ws.git src_repo_tmp
cp -r src_repo_tmp/* . && rm -rf src_repo_tmp   # или работайте прямо в клоне
cd ~/ros2k_ws

colcon build --symlink-install
source install/setup.bash
```

**Почему `--symlink-install`:** URDF/xacro/launch/миры правятся часто —
симлинки позволяют менять их без пересборки пакета.

Если сборка падает на конкретном пакете — соберите по частям (порядок важен):

```bash
colcon build --packages-up-to xarm_description --symlink-install
colcon build --packages-select xarm_controller xarm_moveit_config --symlink-install
colcon build --packages-above-and-dependencies xarm_gz_bringup --symlink-install
colcon build --packages-above-and-dependencies cart_xarm_bringup --symlink-install
```

Проверка, что всё поднялось:

```bash
ros2 pkg list | grep -E "xarm|cart"
# ожидаем: xarm_description, xarm_gz_bringup, xarm_grasping,
#         quasi_cart_description, cart_xarm_bringup ...
```

---

## 4. Сценарий A — рука xArm6 отдельно (стол + блок в Gazebo)

### Запуск

```bash
ros2 launch xarm_gz_bringup xarm6_gz.launch.py add_vacuum_gripper:=true
```

Что поднимется (см. `src/xarm_gz_bringup/launch/xarm6_gz.launch.py`):
1. **Gazebo Sim** с миром `table_gz.world` (стол; для цилиндрического блока
   отдельно есть мир `cylinder_block.world` — его можно передать через аргумент,
   либо использовать сценарий B, где блок уже в мире тележки);
2. **robot_description** из `xarm_description/_robot_description.launch.py`
   с плагином `gz_ros2_control/GazeboSimSystem`;
3. спавн модели `/xarm6` на столе (`-y -0.84 -z 1.0`);
4. контроллеры `joint_state_broadcaster` + `xarm6_traj_controller`;
5. **мост `/clock`** (`config/gz_bridge.yaml`) — источник сим-времени;
6. **MoveIt** (move_group + RViz) через `xarm_moveit_config`.

> 💡 Почему `SetParameter('use_sim_time', True)` стоит в начале launch-файла,
> а не передаётся в каждую ноду: ноды внутри `IncludeLaunchDescription`
> (move_group, rviz, rsp) наследуют параметр дерева; плюс есть «страховочный»
> `fix_sim_time`, который постфактум проставляет `use_sim_time=true` всем
> нодам через `ros2 param set` — иначе MoveIt считает состояния из Gazebo
> протухшими и не исполняет траектории.

### Анализ захватов (второй терминал)

```bash
source ~/ros2k_ws/install/setup.bash
ros2 run xarm_grasping grasp_analyzer.py --ros-args -p use_sim_time:=true
```

Анализатор (`src/xarm_grasping/scripts/grasp_analyzer.py`):
- подписывается на камеру гриппера: `/camera/color/image_raw` и
  `/camera/depth/color/points` (параметры `color_topic`, `points_topic` —
  можно переопределить);
- сегментирует столешницу и цилиндр по облаку точек, переводит в `link_base`
  через TF;
- генерирует варианты захвата присоской (**TOP** — центр торца, лучший score;
  **RIM** — кольцевые точки сверху; **SIDE** — боковые, с проверкой радиуса
  кривизны) и публикует:
  - `/xarm_grasping/grasp_markers` (MarkerArray для RViz: блок, точки касания,
    стрелки pre-grasp→grasp, лучший вариант подсвечен),
  - `/xarm_grasping/grasp_candidates` (JSON-список кандидатов),
  - `/xarm_grasping/best_grasp_pose` (PoseStamped),
  - `/camera/image_display` (изображение камеры для RViz/rqt).

В RViz добавьте displays: **MarkerArray** → `/xarm_grasping/grasp_markers`,
**Image** → `/camera/image_display`, Fixed Frame → `link_base`.

### Проверка без графики (CI / headless)

```bash
xvfb-run ros2 launch xarm_gz_bringup xarm6_gz.launch.py \
  add_vacuum_gripper:=true > gz.log 2>&1 &
sleep 40
timeout 10 ros2 topic list | grep -E "clock|joint_states|grasp"
```

---

## 5. Сценарий B — рука на тележке r2_robot (quasi_cart)

### Как рука крепится к тележке (URDF)

Файлы: `src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro`
(верхний уровень) и `cart-arm.xacro` (макрос `mount_xarm6_on_cart`).

```xml
<joint name="arm_mount_joint" type="fixed">
  <origin xyz="-0.0075 0 0.849" rpy="0 0 0"/>
  <parent link="base_link"/>
  <child  link="arm_mount_link"/>
</joint>
```

**Почему такие числа:** верхняя плоская платформа тележки — лист
[-0.359 .. 0.359] м по X/Y на высоте z = 0.849 м относительно `base_link`;
центр листа по X = -0.0075 (несимметрична база), Y = 0 — рука стоит ровно
по центру платформы. Меняется через аргументы xacro
`arm_offset_xyz` / `arm_offset_rpy`.

Макрос подключает `xarm_device` из `xarm_description` с:
- `add_vacuum_gripper=true` — присоска (та же модель, что в сценарии A);
- `add_realsense_d435i=true` — камера D435i на ЭФ → топики
  `/camera/color/image_raw`, `/camera/depth/points` публикуются прямо из
  Gazebo (gz-камера-плагин), поэтому **один и тот же grasp_analyzer работает
  в обоих сценариях без изменений**;
- `ros2_control_plugin=gz_ros2_control/GazeboSimSystem` — управлениеJoint-ами
  через контроллер-менеджер внутри Gazebo.

### Запуск

```bash
# базовый: Gazebo + RViz + контроллеры + grasp_analyzer (без MoveIt)
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py

# с MoveIt для планирования движений руки на тележке
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=true

# своя поза спавна тележки (мир: стол при x≈+1.7)
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py spawn_x:=0.5 spawn_yaw:=0.0
```

Что поднимется (`cart_xarm6_gz.launch.py`):
1. мир `cart_table_offline.world` (стол + цилиндрический блок, оффлайн-меши;
   аргумент `world:=...` позволяет выбрать `cart_table.world`, ему нужен Fuel);
2. xacro-развёртка `cart_with_arm.urdf.xacro` (`use_sim:=true robot_model:=c2
   simulator:=ignition`) → `robot_state_publisher`;
3. спавн модели `cart_xarm6` в Gazebo;
4. `gz_bridge` (конфиг пакета) — `/clock`, одометрия, камеры;
5. `joint_state_broadcaster` + `xarm6_traj_controller` (spawner-ы);
6. нода `grasp_analyzer` уже включена в launch (параметры топиков камеры
   прописаны: `camera_frame=camera_color_optical_frame`);
7. RViz с маркерами захвата.

### Управление с тележки: отдельные команды

```bash
# едем вперед (дифференциальный привод)
ros2 topic pub -r 10 /cmd_vel geometry_msgs/msg/Twist \
  "{linear: {x: 0.2}}"

# двигаем руку (траекторный контроллер)
ros2 topic pub -1 /xarm6_traj_controller/followed_joint_trajectory \
  trajectory_msgs/msg/JointTrajectory \
  "{header: {frame_id: ''}, joint_names: [joint1,joint2,joint3,joint4,joint5,joint6],
    points: [{positions: [0.0, 0.5, -0.5, 0.0, 1.0, 0.0], time_from_start: {sec: 5}}]}"

# включить присоску (газетто-порт вакуума)
gz service -s /xarm/vacuum_gripper/set_do --reqtype gz.msgs.Bool \
  --reptype gz.msgs.Bool --timeout 2000 --values 'data: true'
```

> 💡 Почему vacuum-гриппер управляется сервисом Gazebo, а не ROS-контроллером:
> в этой сборке присоска — фиксированное звено с визуализацией; физическое
> «прилипание» моделируется контактом в SDF (friction/mu high). Для реального
> робота тот же интерфейс предоставляет `xarm_controller` (DO-пины GripperIO).

---

## 6. Полезные проверки / диагностика

```bash
# дерево TF (рука должна висеть на arm_mount_link тележки)
ros2 run tf2_tools view_frames
ros2 run tf2_ros tf2_echo base_link link_base   # z ≈ 0.849 — центр платформы

# живы ли топики камеры
ros2 topic hz /camera/depth/color/points

# результаты анализа захвата
ros2 topic echo /xarm_grasping/grasp_candidates --once

# логи сборки при проблемах
colcon build --event-listeners console_cohesion
```

Типичные проблемы:

| Симптом | Причина и решение |
|---|---|
| `package 'xarm_gz_bringup' not found` | не сделали `source install/setup.bash` в новом терминале |
| RViz серая / нет маркеров | Fixed Frame ≠ `link_base`; MarkerArray display не добавлен |
| MoveIt игнорирует цели | где-то нода получила `use_sim_time=false` — перезапустите launch (там есть fix_sim_time) или прогоните `ros2 param set` вручную |
| Gazebo не видит модель | спавнер стартует раньше мира — увеличьте `TimerAction period` в launch |
| xacro ругается на `$(find xarm_description)` | пакет `xarm_description` не собран/не засорсен — соберите его первым |

## 7. Структура репозитория (коротко)

```
src/
├── xarm_ros2/            # upstream UFactory: description, controller, moveit_config
├── xarm_gz_bringup/      # сценарий A: запуск руки в Gazebo (+ миры table_gz/cylinder_block)
├── xarm_grasping/        # grasp_analyzer.py + grasp_generator.py (ТОП/RIM/SIDE)
└── r2_robot/
    ├── quasi_cart_description/   # URDF тележки + cart-arm.xacro (крепление руки)
    └── cart_xarm_bringup/        # сценарий B: launch, миры cart_table*, gz_bridge
```
