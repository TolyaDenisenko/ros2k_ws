# Установка и сборка ros2k_ws

Рабочее пространство ROS 2 с симуляцией тележки **quasi_cart** и манипулятора
**xArm6** на ней: на фланце руки — вакуумная присоска и камера глубины
**Orbbec Gemini 335Le** (старая RealSense D435i — по `camera_model:=d435i`). Есть
отдельная сцена «рука на столе» с конвейером «найти предметы камерой → взять →
поставить в зону» и распознаванием предметов нейросетью (OpenVINO), и навигация
тележки (Nav2 + SLAM).

Этот документ — **установка, зависимости, сборка и проверка сборки**. Как запускать
сценарии, все их аргументы и проверки — в [RUN_SCENARIOS.md](RUN_SCENARIOS.md).
Оглавление всей документации — [README.md](README.md).

---

## 1. Требования

Проверенная конфигурация:

- **Ubuntu 24.04** (x86_64)
- **ROS 2 Kilted Kaiju**
- **Gazebo Sim 9** (ставится вместе с `ros-kilted-ros-gz`, отдельно ставить не нужно)
- видеокарта с OpenGL 3.3+ (камеры и лидар в симуляции рендерятся через ogre2)

Другие дистрибутивы ROS (Humble/Jazzy) не проверялись: launch-файлы и плагины
настроены под `gz_ros2_control` и gz sim 9 из Kilted.

## 2. Установка ROS 2 Kilted

Если ROS 2 уже стоит, переходите к разделу 3.

```bash
# локаль UTF-8
sudo apt update && sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# репозиторий ROS 2
sudo apt install -y software-properties-common curl
sudo add-apt-repository universe
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F\" '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo $VERSION_CODENAME)_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb

sudo apt update
sudo apt install -y ros-kilted-desktop ros-dev-tools
```

Чтобы не вводить `source` в каждом терминале:

```bash
echo "source /opt/ros/kilted/setup.bash" >> ~/.bashrc
```

## 3. Зависимости проекта

Основное (симуляция, рука, MoveIt, камеры):

```bash
sudo apt install -y \
  python3-colcon-common-extensions python3-numpy python3-opencv python3-scipy python3-yaml \
  ros-kilted-xacro \
  ros-kilted-ros-gz \
  ros-kilted-gz-ros2-control \
  ros-kilted-ros2-control ros-kilted-ros2-controllers ros-kilted-ros2controlcli \
  ros-kilted-moveit \
  ros-kilted-joint-state-publisher ros-kilted-joint-state-publisher-gui \
  ros-kilted-robot-state-publisher \
  ros-kilted-cv-bridge ros-kilted-image-transport \
  ros-kilted-depth-image-proc \
  ros-kilted-topic-tools \
  ros-kilted-vision-msgs \
  ros-kilted-tf2-ros ros-kilted-tf2-tools \
  ros-kilted-rviz2 \
  ros-kilted-teleop-twist-keyboard
```

- `topic_tools` — прореживание часов `/clock` (без него симуляция не получает времени);
- `depth_image_proc` — облака точек камер из глубины;
- `vision_msgs` — рамки детектора объектов (`/detections`).

Для навигации тележки (`cart_nav_gz.launch.py`, [NAV.md](NAV.md)):

```bash
sudo apt install -y ros-kilted-navigation2 ros-kilted-nav2-bringup ros-kilted-slam-toolbox \
                    ros-kilted-octomap-server ros-kilted-octomap-rviz-plugins
```

Детектор меток ArUco (`aruco_detector.py`) использует модуль `cv2.aruco` из
`python3-opencv` (в Ubuntu 24.04 он есть, проверено: OpenCV 4.6).

Если что-то забыто, `rosdep` доставит недостающее (запускать после клонирования, раздел 4):

```bash
sudo rosdep init        # один раз на машину
rosdep update
cd ~/ros2k_ws && rosdep install --from-paths src --ignore-src -r -y
```

**Распознавание предметов нейросетью (по желанию).** OpenVINO ставится в отдельное
виртуальное окружение `~/.venvs/openvino`, модели лежат вне git в
`~/.local/share/quasi/models`. Пошагово — [RECOGNITION_OPENVINO.md](RECOGNITION_OPENVINO.md)
и [RUN_SCENARIOS.md](RUN_SCENARIOS.md) §4.7. Для обучения своих моделей нужен ещё один
venv `~/.venvs/yolox` с PyTorch — [MODEL_TRAINING.md](MODEL_TRAINING.md). Для остальных
сценариев это не нужно.

## 4. Клонирование

Корень репозитория — это само рабочее пространство (внутри лежит `src/`):

```bash
git clone https://github.com/TolyaDenisenko/ros2k_ws.git ~/ros2k_ws
cd ~/ros2k_ws
```

Папка `src/object_recognition_msgs` после клонирования пустая: это ссылка на
сабмодуль без `.gitmodules`. Она ни на что не влияет, потому что пакет, которому
она нужна (`d435i_xarm_setup`), исключён из сборки.

## 5. Сборка

Собирать нужно **из `~/ros2k_ws`**, а не из `src/`:

```bash
cd ~/ros2k_ws
source /opt/ros/kilted/setup.bash
colcon build --symlink-install
source install/setup.bash
```

`--symlink-install` нужен, чтобы правки URDF/xacro, launch-файлов, миров, yaml и
python-скриптов подхватывались без пересборки (кроме **новых** файлов: их нужно
установить, т.е. пересобрать пакет: `colcon build --symlink-install --packages-select <пакет>`).

> Папки `src/build`, `src/install`, `src/log` лежат в репозитории специально: в каждой
> только `COLCON_IGNORE`. Если случайно собрали из `src/`, удалите в них всё, кроме
> этого файла ([глава 0](00-введение.md)).

Чтобы окружение подхватывалось в каждом новом терминале:

```bash
echo "source ~/ros2k_ws/install/setup.bash" >> ~/.bashrc
```

### Пакеты, исключённые из сборки

В `src/xarm_ros2` есть пакеты под Gazebo Classic и зависимости, которых нет в
Kilted. В каждом лежит `COLCON_IGNORE`, colcon их пропускает. **Не включайте их обратно.**

| Пакет | Почему исключён |
|---|---|
| `thirdparty/realsense_gazebo_plugin` | плагин для Gazebo Classic (`gazebo_ros`); камера в gz sim сделана нативным SDF-сенсором |
| `xarm_gazebo` | Gazebo Classic; вместо него `xarm_gz_bringup` |
| `xarm_vision/d435i_xarm_setup` | нужен `find_object_2d` / `object_recognition_msgs`, нужен только для реальной камеры |
| `xarm_planner` | метапакет `moveit`, недоступный в Kilted; планирование даёт `xarm_moveit_config` |
| `xarm_moveit_servo` | несовместим с API `moveit_servo` текущей версии |

Также не собирается `docs/reference/quasi_ros_r2_grasp/` (алгоритмы захвата R2 на будущее).

### Проверка сборки

```bash
ros2 pkg list | grep -E "xarm|cart|quasi"
# ожидаем среди прочего: cart_xarm_bringup, quasi_cart_description, quasi_camera_description,
# xarm_description, xarm_controller, xarm_moveit_config, xarm_gz_bringup,
# xarm_grasping, xarm_grasping_msgs
```

Модель робота собирается без ошибок (Gazebo не нужен):

```bash
D=$(ros2 pkg prefix quasi_cart_description)/share/quasi_cart_description/urdf
xacro $D/cart_with_arm.urdf.xacro > /tmp/c.urdf && check_urdf /tmp/c.urdf | head -3
xacro $D/cart_with_arm.urdf.xacro with_arm:=false > /tmp/cart_only.urdf && check_urdf /tmp/cart_only.urdf | head -3
```

Если после `git pull` сборка ведёт себя странно, сделайте чистую сборку:

```bash
cd ~/ros2k_ws && rm -rf build install log && colcon build --symlink-install
```

---

## 6. Первый запуск

Самый быстрый способ убедиться, что всё установлено, — посмотреть модель в RViz без
Gazebo:

```bash
ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py
```

Откроются RViz с тележкой и рукой и окно с ползунками суставов.

Дальше — сценарии с симуляцией. Перед каждым запуском уберите следы прошлых:

```bash
pgrep -a gz            # должно быть пусто
fastdds shm clean      # мёртвые сегменты Fast DDS (из-за них «не находятся» сервисы)
```

| Сценарий | Команда | Подробно |
|---|---|---|
| Рука на столе: захват, pick & place, распознавание | `ros2 launch cart_xarm_bringup arm_table_gz.launch.py` | [RUN_SCENARIOS.md §2](RUN_SCENARIOS.md), [глава 8](08-рука-на-столе.md) |
| Тележка + рука: датчики, камеры, MoveIt, вакуум, езда вручную | `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py` | [RUN_SCENARIOS.md §3.1](RUN_SCENARIOS.md), [глава 2](02-cart_xarm_bringup.md) |
| Тележка без руки (лёгкая симуляция) | `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm:=false` | [глава 2, §3.14](02-cart_xarm_bringup.md) |
| Навигация тележки (по умолчанию без руки) | `ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true` | [RUN_SCENARIOS.md §3.2](RUN_SCENARIOS.md), [NAV.md](NAV.md) |
| Рука одна на столе (пример UFactory, Gazebo + MoveIt) | `ros2 launch xarm_gz_bringup xarm6_gz.launch.py add_vacuum_gripper:=true` | [глава 5](05-xarm_gz_bringup.md) |
| Рука одна, только RViz + MoveIt (фейковое железо) | `ros2 launch xarm_moveit_config xarm6_moveit_fake.launch.py add_vacuum_gripper:=true` | [глава 6](06-xarm_ros2.md) |

Остановка — `Ctrl+C` в терминале с launch и подождать 10–15 с. Не убивайте процессы
`kill -9`: от этого копится мусор в общей памяти. Если параллельно нужна вторая симуляция,
дайте ей свои `ROS_DOMAIN_ID` и `GZ_PARTITION` ([RUN_SCENARIOS.md §1.5](RUN_SCENARIOS.md)).

### Быстрая проверка, что тележка с рукой работает

После `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py` подождите ~15 с и
выполните во втором терминале:

```bash
ros2 control list_controllers
# joint_state_broadcaster ... active
# xarm6_traj_controller   ... active
# arm_hold_controller     ... inactive   (тормоз руки, включается при парковке)

ros2 topic echo --once --field position /joint_states
# ≈ [1.5708, -0.9, -1.3, 0.0, 1.0, 0.0] — рука в начальной позе

ros2 topic hz /clock                        # ≈ 200 Гц — время идёт из Gazebo
ros2 topic hz /gripper_camera/points        # облако камеры на запястье
ros2 run tf2_ros tf2_echo base_link drive_wheel_left_link   # TF колёс есть
ros2 run tf2_ros tf2_echo base_link link_base               # для c2l-v3: ≈ (-0.25, -0.13, 0.849)
ros2 param get /move_group use_sim_time     # Boolean value is: True
```

Дерево TF целиком:

```bash
ros2 run tf2_tools view_frames   # создаст frames_*.pdf
```

### Подвигать руку из терминала

Через контроллер, без MoveIt:

```bash
ros2 topic pub --once /xarm6_traj_controller/joint_trajectory trajectory_msgs/msg/JointTrajectory \
  "{joint_names: [joint1, joint2, joint3, joint4, joint5, joint6],
    points: [{positions: [1.5708, -0.6, -1.0, 0.0, 1.0, 0.0], time_from_start: {sec: 3}}]}"
```

Через MoveIt (запуск по умолчанию, с MoveIt):

```bash
ros2 action send_goal /move_action moveit_msgs/action/MoveGroup "{request: {group_name: xarm6,
  max_velocity_scaling_factor: 0.5, max_acceleration_scaling_factor: 0.5, allowed_planning_time: 5.0,
  goal_constraints: [{joint_constraints: [
    {joint_name: joint1, position: 1.0, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0},
    {joint_name: joint2, position: -0.9, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0},
    {joint_name: joint3, position: -1.3, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0},
    {joint_name: joint4, position: 0.0, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0},
    {joint_name: joint5, position: 1.0, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0},
    {joint_name: joint6, position: 0.0, tolerance_above: 0.01, tolerance_below: 0.01, weight: 1.0}]}]},
  planning_options: {plan_only: false}}"
# ожидаем: Goal finished with status: SUCCEEDED
```

Если рука запаркована (`ros2 topic echo /arm_parking/state` → `true`), сначала снимите
тормоз: `ros2 service call /arm_parking/set std_srvs/srv/SetBool "{data: false}"`.

В логе `move_group` бывают строки
`Could not find the planner configuration 'RRTConnect' on the param server` —
это особенность upstream-конфига xArm, на работу не влияет.

Тележку можно водить с клавиатуры: `ros2 run teleop_twist_keyboard teleop_twist_keyboard`
(публикует `/cmd_vel`). Тележка едет на колёсах (плагин Gazebo `DiffDrive`).

---

## 7. Типичные проблемы установки и первого запуска

| Симптом | Причина и решение |
|---|---|
| `Package 'cart_xarm_bringup' not found` | не выполнен `source ~/ros2k_ws/install/setup.bash` в этом терминале |
| `ros2: invalid choice: 'control'` | не установлен `ros-kilted-ros2controlcli` |
| launch не находит пакет `topic_tools`, у нод нет времени (`/clock` пуст) | не установлен `ros-kilted-topic-tools` — некому публиковать `/clock` |
| `Package 'depth_image_proc' not found`, нет облаков `/…/points` | не установлен `ros-kilted-depth-image-proc` |
| Новый скрипт/мир/модель «не найден» | новые файлы требуют пересборки пакета (`--packages-select <пакет>`) |
| Собрали из `src/` — странные ошибки, пакеты в `src/build` | собирать только из `~/ros2k_ws`; в `src/build`, `src/install`, `src/log` оставить только `COLCON_IGNORE` |
| `Another world of the same name is running` | от прошлого запуска осталась gz sim: найдите её `pgrep -a gz` и закройте `kill <pid>` |
| Сервисы и action «не находятся», Nav2/SLAM не поднимаются | мусор Fast DDS после убитых запусков: остановите ROS и выполните `fastdds shm clean` |
| Gazebo падает с `Scene already exists with name: scene` | Sensors-система подключена дважды: она должна быть только в world-файле, модель собирается с `model_sensors_plugin:=false` (уже так в `cart_with_arm.urdf.xacro`) |
| Пример UFactory (`xarm6_gz.launch.py`): Gazebo сразу закрывается, `Unable to find uri[...]` | нет интернета для моделей Fuel при первом запуске |
| RViz: `No transform from [drive_wheel_*]` | не запущен `joint_state_merger` или нет моста `/cart/wheel_joint_states`; проверьте `ros2 topic echo /cart/wheel_joint_states` |
| Рука в RViz не в той позе, что в Gazebo | `robot_state_publisher` читает `/merger/joint_states` (рука + колёса); проверьте `ros2 topic echo --once /merger/joint_states` — там должны быть углы руки, а не нули |
| MoveIt: `Robot model not loaded` | ошибка разбора URDF MoveIt, смотрите первые строки лога `move_group` |
| `enable_camera_oak:=true` / `enable_camera_orbbec:=true` ломают xacro | нет пакетов `depthai_descriptions` / `orbbec_description`; эти флаги не нужны — передняя камера включается `front_camera:=true` ([глава 1](01-quasi_cart_description.md)) |
| `detector:=true`, а детектор падает при старте | нет venv `~/.venvs/openvino` или файла модели; см. [RECOGNITION_OPENVINO.md](RECOGNITION_OPENVINO.md) |
| Traceback / `exit code -11` при Ctrl+C | ноды на Python и rviz2 так завершаются по сигналу; это не ошибка |

Неполадки самих сценариев (MoveIt не исполняет, Nav2 не поднимается, тележка
опрокидывается и т.п.) — в [RUN_SCENARIOS.md](RUN_SCENARIOS.md) §8 и [NAV.md](NAV.md).

---

## 8. Структура репозитория

```
ros2k_ws/
├── README.md                     # короткий указатель на docs/
├── CLAUDE.md                     # правила проекта для Claude
├── docs/                         # вся документация (оглавление — docs/README.md)
└── src/
    ├── r2_robot/
    │   ├── quasi_cart_description/   # URDF тележки: cart_with_arm.urdf.xacro, cart-arm.xacro, arm_on_table.urdf.xacro
    │   ├── cart_xarm_bringup/        # все launch (тележка, рука на столе, навигация), миры, мосты, RViz, tools/
    │   └── quasi_camera_description/ # камера Orbbec Gemini 335Le (на запястье и передняя)
    ├── xarm_gz_bringup/              # пример UFactory: рука одна на столе (table_gz.world)
    ├── xarm_grasping/                # вакуум, восприятие, сцена MoveIt, pick & place, детектор, grasp_analyzer
    ├── xarm_grasping_msgs/           # свои сообщения и actions (объекты, место, pick & place)
    └── xarm_ros2/                    # upstream UFactory (description, controller, moveit_config, ...),
                                      # с локальными правками под Kilted
```

Подробная карта и что в какой главе — [глава 0](00-введение.md).
