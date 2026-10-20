# Глава 6. `xarm_ros2` — форк драйвера и описания руки UFactory

> Папка: [`src/xarm_ros2`](../../src/xarm_ros2) — git-подмодуль/форк официального репозитория UFactory `xarm_ros2`.

## 1. Зачем и связи

Это «родной» софт производителя руки xArm. В нём **десяток пакетов**, но наш проект использует только часть:

| Пакет | Что даёт нам | Кто им пользуется |
|---|---|---|
| `xarm_description` | URDF/xacro руки, присоски, D435i; **главный макрос** `xarm_device` | `quasi_cart_description/urdf/cart-arm.xacro`, `xarm_gz_bringup`, MoveIt |
| `xarm_controller` | yaml контроллеров ros2_control (`xarm6_controllers.yaml`) + аппаратные плагины для реальной руки | плагин `gz_ros2_control` в Gazebo |
| `xarm_moveit_config` | SRDF, kinematics, joint_limits, OMPL, launch MoveIt (`_robot_moveit_common.launch.py`) | `cart_xarm6_gz.launch.py`, `xarm6_gz.launch.py` |
| `uf_ros_lib` | Python-утилиты: `MoveItConfigsBuilder`, генерация xacro-команд | launch-файлы `xarm_moveit_config` |
| `xarm_api`, `xarm_sdk`, `xarm_msgs` | драйвер реальной руки по сети | в симуляции не используются (только `xarm_msgs` как зависимость) |

Термины:
- **URDF** — XML-описание робота: звенья (`link`, геометрия/масса) и суставы (`joint`, ось/пределы). **xacro** — макроязык над URDF (параметры, `if/unless`, `include`), его разворачивает команда `xacro`.
- **SRDF** — «семантика» для MoveIt: группы суставов, именованные позы, какие пары звеньев не проверять на столкновение.
- **ros2_control** — фреймворк «железо + контроллеры». Тег `<ros2_control>` в URDF описывает интерфейсы суставов (команда `position`, состояние `position/velocity`) и плагин «железа».
- **MoveIt 2** — планировщик движений; нода `move_group`.

Связи с другими главами: тележка вызывает макрос `xarm_device` из `cart-arm.xacro` (см. главу про `quasi_cart_description`), камера на фланце подключается из `quasi_camera_description` через хук `add_gripper_camera`.

## 2. Дерево папок

```text
src/xarm_ros2/
├── ReadMe.md, ReadMe_cn.md, LICENSE      # оригинальная документация UFactory
├── xarm_description/      ★ URDF/xacro
│   ├── urdf/
│   │   ├── xarm_device.urdf.xacro       # точка входа: xacro:arg → вызов макроса
│   │   ├── xarm_device_macro.xacro      ★ ГЛАВНЫЙ макрос xarm_device (наши хуки)
│   │   ├── dual_xarm_device.urdf.xacro  # две руки (не используем)
│   │   ├── _private_macro.xacro         # служебные begin/end макроса
│   │   ├── xarm6/                       ★ xarm6.urdf.xacro (звенья, пределы), .ros2_control, .gazebo, .transmission, _robot_macro
│   │   ├── xarm5/ xarm7/ xarm7_mirror/ lite6/ uf850/   # другие модели (не используем)
│   │   ├── common/                      # common.link (меши/инерции), common.gazebo (плагины ros2_control), материалы
│   │   ├── vacuum_gripper/              ★ xarm_vacuum_gripper.urdf.xacro
│   │   ├── gripper/                     # двухпалый/BIO/lite захваты
│   │   ├── camera/                      # realsense_d435i.urdf.xacro, realsense.gazebo.xacro, camera.gazebo.xacro
│   │   └── other/                       # произвольная геометрия на фланце
│   ├── config/kinematics/default/       # DH/кинематика по моделям (xarm6_default_kinematics.yaml)
│   ├── config/link_inertial/            # массы/инерции звеньев по моделям
│   ├── launch/                          # _robot_description.launch.py (★), *_rviz_display.launch.py
│   ├── meshes/                          # STL/DAE/OBJ всех моделей (группой)
│   └── rviz/
├── xarm_controller/       ★ config/xarm6_controllers.yaml (+ другие модели); src/hardware/ — плагины для реальной руки
├── xarm_moveit_config/    ★
│   ├── srdf/                            # xarm.srdf.xacro → _xarm6_macro.srdf.xacro
│   ├── config/xarm6/                    # controllers, fake_controllers, joint_limits, kinematics, ompl_planning
│   ├── config/moveit_configs/           # общие: chomp, ompl_defaults, pilz
│   ├── config/<другие модели и захваты>/
│   ├── launch/                          # _robot_moveit_common.launch.py (★), xarm6_moveit_fake/realmove/gazebo, dual_*
│   │   └── lib/robot_moveit_config_lib.py
│   └── rviz/                            # moveit.rviz, planner.rviz, dual_*
├── uf_ros_lib/            ★ uf_ros_lib/moveit_configs_builder.py, uf_robot_utils.py, substitutions/
├── xarm_api/  xarm_sdk/  xarm_msgs/      # реальный робот: ROS-обёртка SDK, C++ SDK, сообщения/сервисы
├── xarm_vision/d435i_xarm_setup/        # COLCON_IGNORE
├── thirdparty/realsense_gazebo_plugin/  # COLCON_IGNORE
├── demo/mbot_demo/                       # пример: рука на мобильной базе (не используем)
├── xarm_gazebo/  xarm_planner/  xarm_moveit_servo/   # COLCON_IGNORE
```

★ — то, что реально используется проектом и может понадобиться менять.

## 3. Разбор содержательных файлов

### 3.1. `xarm_description/urdf/xarm_device.urdf.xacro` — точка входа

[xarm_device.urdf.xacro](../../src/xarm_ros2/xarm_description/urdf/xarm_device.urdf.xacro): объявляет `xacro:arg` ([строки 3–48](../../src/xarm_ros2/xarm_description/urdf/xarm_device.urdf.xacro#L3-L48)) и вызывает макрос ([строки 52–71](../../src/xarm_ros2/xarm_description/urdf/xarm_device.urdf.xacro#L52-L71)) с `load_gazebo_plugin="true"`. Дефолт `dof=7` — для xArm6 обязательно `dof:=6`.

> ⚠️ Обрати внимание: этот файл **не** пробрасывает `add_gripper_camera` / `gripper_camera_*` в макрос. Камеру `quasi_camera_description` можно получить только через свой xacro, вызывающий макрос напрямую (так делает `cart-arm.xacro`). В `xarm_gz_bringup` и в модели MoveIt камеры нет.

### 3.2. `xarm_description/urdf/xarm_device_macro.xacro` — главный макрос

[xarm_device_macro.xacro](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro) (352 строки). Параметры макроса ([строки 3–50](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L3-L50)), важные для нас:

```xml
<xacro:macro name="xarm_device" params="
  prefix:=''  dof:=7  robot_type:='xarm'
  add_vacuum_gripper:=false
  ros2_control_plugin:='uf_robot_hardware/UFRobotSystemHardware'
  attach_to:='world'  attach_xyz:='0 0 0'  attach_rpy:='0 0 0'
  create_attach_link:=true
  load_gazebo_plugin:=false
  add_realsense_d435i:=false
  add_gripper_camera:=false
  gripper_camera_model:='gemini335le'
  gripper_camera_nominal:=true  gripper_camera_gz:=false  gripper_camera_preview:=false
  ...
```

Порядок сборки внутри макроса:

| Строки | Что делает | Зачем знать |
|---|---|---|
| [63–69](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L63-L69) | `mesh_path` = `file://$(find xarm_description)/meshes` для gz-плагинов, иначе `package://` | gz не резолвит `package://` |
| [78–86](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L78-L86) | создаёт `link world` (если `attach_to=='world'`) и fixed-сустав `world_joint`: `attach_to → link_base` с `attach_xyz/rpy` | на тележке `attach_to` = звено тележки |
| [165–181](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L165-L181) | для `dof==6`, `robot_type` не lite/uf850 → `xarm6_robot` (звенья + ros2_control + gazebo + transmission) | |
| [220–244](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L220-L244) | **хук `add_realsense_d435i`**: D435i (меш камеры = `link_eef`), gz-сенсоры `realsense_gz`; иначе пустой `link_eef` | старая камера |
| [250–257](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L250-L257) | **хук `add_gripper_camera`** (наша вставка): камера из `quasi_camera_description` на `link_eef` | Gemini 335Le и др. |
| [258–262](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L258-L262) | `joint_eef`: `link6 → link_eef`, нули | точка крепления всего инструмента |
| [291–302](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L291-L302) | **хук `add_vacuum_gripper`** (если нет `add_gripper`/`add_bio_gripper`) → `xarm_vacuum_gripper_urdf` на `link_eef` | присоска |
| [318–343](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L318-L343) | `load_gazebo_plugin`: вставляет `gz_ros2_control-system` с yaml `xarm_controller/config/${robot_type}${dof}_controllers.yaml` (или `ros2_control_params`) | откуда gz берёт контроллеры |
| [345–348](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L345-L348) | `use_gazebo_camera` — старая Classic-камера | не включать |

Наш хук камеры:

```xml
<xacro:if value="${add_gripper_camera}">
  <xacro:include filename="$(find quasi_camera_description)/urdf/gripper_camera.xacro" />
  <xacro:gripper_camera camera_model="${gripper_camera_model}" parent="${prefix}link_eef"
                        name="gripper_camera"
                        use_nominal_extrinsics="${gripper_camera_nominal}"
                        add_gz_sensor="${gripper_camera_gz}"
                        preview="${gripper_camera_preview}" />
</xacro:if>
```

Приоритеты инструментов: `add_gripper` > `add_bio_gripper` > `add_vacuum_gripper` > `add_other_geometry` — включённый «старший» отключает младших. `add_gripper_camera` и `add_realsense_d435i` по смыслу взаимоисключающие (комментарий [строки 246–249](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L246-L249)), но макрос это не проверяет.

### 3.3. `xarm6/xarm6.urdf.xacro` — звенья и пределы суставов

[xarm6.urdf.xacro:3–11](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.urdf.xacro#L3-L11):

```xml
joint1_lower_limit:=${-2.0*pi}  joint1_upper_limit:=${2.0*pi}
joint2_lower_limit:=${-2.059}   joint2_upper_limit:=${2.0944}
joint3_lower_limit:=${-3.927}   joint3_upper_limit:=${0.19198}
joint4_lower_limit:=${-2.0*pi}  joint4_upper_limit:=${2.0*pi}
joint5_lower_limit:=${-1.69297} joint5_upper_limit:=${pi}
joint6_lower_limit:=${-2.0*pi}  joint6_upper_limit:=${2.0*pi}
```

Каждый сустав — `revolute` с `effort="50.0" velocity="3.14"` и `damping/friction = 1.0` (пример joint1, [строки 94–106](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.urdf.xacro#L94-L106)). Положения суставов (DH) берутся из `config/kinematics/default/xarm6_default_kinematics.yaml`, массы — из `config/link_inertial/`.

> ⚠️ Обрати внимание: те же пределы **продублированы** в [xarm6.ros2_control.xacro:15–20](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L15-L20). Меняешь пределы — меняй в обоих файлах, а скорости/ускорения для MoveIt — в `joint_limits.yaml` (3.7).

### 3.4. `xarm6/xarm6.ros2_control.xacro` — интерфейсы и начальная поза

[xarm6.ros2_control.xacro](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro) (149 строк). Наша доработка — начальная поза в симуляции ([строки 3–9](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L3-L9)):

```xml
<!-- 6 чисел через пробел (рад), joint1..joint6; пусто — все нули ...
     Реальное железо (UFRobotSystemHardware) этот параметр игнорирует. -->
<xacro:arg name="xarm_initial_positions" default=""/>
<xacro:property name="xarm_init_pos" value="$(arg xarm_initial_positions)"/>
```

Для каждого сустава: `command_interface position` (min/max = пределы) и `velocity` (±3.14), `state_interface position` с `initial_value` из `xarm_init_pos` ([строка 57](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L57) для joint1) и `velocity`. Плагин «железа» — `${ros2_control_plugin}`; параметры `robot_ip`, `hw_ns` и т.п. вставляются только для реального `UFRobotSystemHardware` ([строки 24–36](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L24-L36)).

### 3.5. Gazebo-xacro

- [xarm6/xarm6.gazebo.xacro](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.gazebo.xacro) — `selfCollide=true` для всех звеньев.
- [common/common.gazebo.xacro:4–14](../../src/xarm_ros2/xarm_description/urdf/common/common.gazebo.xacro#L4-L14) — макрос `gz_ros2_control_plugin`: `<plugin filename="gz_ros2_control-system" ...><parameters>yaml</parameters>`. Рядом — Ignition, Classic и ROS1 варианты (не используются).
- [camera/realsense.gazebo.xacro](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro) — макрос `realsense_gz` ([строка 258](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro#L258)), топики `camera/color/image_raw`, `camera/depth` и т.д.; аргумент `model_sensors_plugin` (по умолчанию `true`, [строка 10](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro#L10)) добавляет систему Sensors в модель — **если Sensors уже есть в мире, передай `false`**, иначе `Scene already exists` + segfault ([комментарий 4–9](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro#L4-L9)).

### 3.6. `vacuum_gripper/xarm_vacuum_gripper.urdf.xacro` — присоска

[xarm_vacuum_gripper.urdf.xacro](../../src/xarm_ros2/xarm_description/urdf/vacuum_gripper/xarm_vacuum_gripper.urdf.xacro): `vacuum_gripper_fix` (`link_eef → xarm_vacuum_gripper_link`), масса 0.656 кг, меш, и TCP ([строки 33–39](../../src/xarm_ros2/xarm_description/urdf/vacuum_gripper/xarm_vacuum_gripper.urdf.xacro#L33-L39)):

```xml
<link name="${prefix}link_tcp" />
<joint name="${prefix}joint_tcp" type="fixed">
  <origin xyz="0 0 0.126" rpy="0 0 0" />
```

`link_tcp` — точка, которую MoveIt ведёт к цели (кончик присоски, 126 мм от фланца). Это чисто геометрия; «присасывание» в симуляции делает `DetachableJoint` из `cart-arm.xacro` (см. `docs/CAMERA_VACUUM_SIM.md`).

### 3.7. `_robot_description.launch.py`

[xarm_description/launch/_robot_description.launch.py](../../src/xarm_ros2/xarm_description/launch/_robot_description.launch.py): собирает `LaunchConfiguration` ([строки 21–61](../../src/xarm_ros2/xarm_description/launch/_robot_description.launch.py#L21-L61)), прогоняет xacro и запускает `robot_state_publisher` ([строки 109–119](../../src/xarm_ros2/xarm_description/launch/_robot_description.launch.py#L109-L119)). Наша вставка — `xarm_initial_positions` ([строка 61](../../src/xarm_ros2/xarm_description/launch/_robot_description.launch.py#L61), [102](../../src/xarm_ros2/xarm_description/launch/_robot_description.launch.py#L102)). Используется `xarm_gz_bringup`; тележка строит URDF своим `Command(xacro ...)`.

### 3.8. `xarm_controller/config/xarm6_controllers.yaml`

[xarm6_controllers.yaml](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml) — его читает менеджер контроллеров внутри gz:

```yaml
controller_manager:
  ros__parameters:
    update_rate: 150  # Hz
    joint_state_broadcaster:
      type: joint_state_broadcaster/JointStateBroadcaster
    xarm6_traj_controller:
      type: joint_trajectory_controller/JointTrajectoryController
```

| Параметр | Строка | Значение | Смысл |
|---|---|---|---|
| `update_rate` | [3](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L3) | 150 Гц | частота цикла управления |
| `command_interfaces` | [13–15](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L13-L15) | position, velocity | |
| `constraints.goal_time` | [27](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L27) | 0.5 с | допуск по времени достижения цели |
| `jointN: {trajectory: 1.0, goal: 0.01}` | [29–34](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L29-L34) | рад | допуск отклонения на траектории / в конце |
| `state_publish_rate` | [36](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L36) | 25 Гц | |
| `action_monitor_rate` | [37](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L37) | 10 Гц | |
| `open_loop_control` | [38](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L38) | true | команда без обратной связи по ошибке |

> ⚠️ Обрати внимание: `interface_name: position` ([строка 35](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L35)) — устаревший ключ JTC, современные версии используют `command_interfaces`.

Остальное в пакете: `launch/` (ros2_control для реальной руки/fake, `*_control_rviz_display`), `src/hardware/` — плагины `UFRobotSystemHardware` (реальная рука) и `UFRobotFakeSystemHardware` (fake для `xarm6_moveit_fake`). Файл `bak_uf_robot_fake_system_hardware.cpp` — бэкап.

### 3.9. `xarm_moveit_config`

**SRDF** — [srdf/_xarm6_macro.srdf.xacro](../../src/xarm_ros2/xarm_moveit_config/srdf/_xarm6_macro.srdf.xacro) (213 строк):
- группа `xarm6`: `world_joint`, joint1..6, `joint_eef` и, при присоске, `vacuum_gripper_fix` + `joint_tcp` ([строки 14–40](../../src/xarm_ros2/xarm_moveit_config/srdf/_xarm6_macro.srdf.xacro#L14-L40)) — поэтому конец цепи = `link_tcp`;
- позы `home` (все нули) и `hold-up` (joint5 = −1.5708) ([42–57](../../src/xarm_ros2/xarm_moveit_config/srdf/_xarm6_macro.srdf.xacro#L42-L57));
- присоска — `passive_joint` ([строка 181](../../src/xarm_ros2/xarm_moveit_config/srdf/_xarm6_macro.srdf.xacro#L181)); ниже — `disable_collisions`.

**config/xarm6/**:

| Файл | Ключевое |
|---|---|
| [kinematics.yaml](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/kinematics.yaml) | KDL, `search_resolution 0.005`, `timeout 0.005` с, `attempts 3` (TRAC-IK закомментирован) |
| [joint_limits.yaml](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/joint_limits.yaml) | все суставы `max_velocity 2.14`, `max_acceleration 10.0` |
| [ompl_planning.yaml](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/ompl_planning.yaml) | `default_planner_config: RRTConnect` + список планировщиков |
| [controllers.yaml](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/controllers.yaml), [fake_controllers.yaml](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/fake_controllers.yaml) | одинаковые: `xarm6_traj_controller`, `FollowJointTrajectory`, `action_ns: follow_joint_trajectory` |

Общие `config/moveit_configs/` (chomp, ompl_defaults, pilz) использует `MoveItConfigsBuilder`. Прочие папки (`xarm5`, `lite6`, `*_gripper`…) — для других моделей.

**`launch/_robot_moveit_common.launch.py`** — используется **обоими** нашими сценариями (cart и `xarm_gz_bringup`). [Файл](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py):

- `use_sim_time` — `LaunchConfiguration('use_sim_time', default=False)` ([строка 63](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L63)) и передаётся нодам явно: `move_group` ([250](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L250)), `rviz2` ([276](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L276)), static TF ([299](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L299)). Явный параметр ноды **перекрывает** глобальный `SetParameter`, поэтому наши launch передают `use_sim_time:=true` напрямую.
- `publish_attach_tf` ([строка 66](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L66), наша вставка) — отключает static TF `world → link_base` ([287–301](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L287-L301)); на тележке `false`, т.к. `link_base` уже ребёнок `arm_mount_link`.
- `rviz_config` ([266–268](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L266-L268), наша вставка) — свой конфиг RViz.
- Модель для MoveIt: `get_xarm_robot_description_parameters(...)` из [launch/lib/robot_moveit_config_lib.py:55–75](../../src/xarm_ros2/xarm_moveit_config/launch/lib/robot_moveit_config_lib.py#L55-L75) — xacro `xarm_device.urdf.xacro` + `xarm.srdf.xacro` + `joint_limits.yaml` + `kinematics.yaml` ([строки 71–123](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L71-L123)).
- Пайплайн OMPL с адаптерами ([168–194](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L168-L194)), исполнение траекторий: `allowed_execution_duration_scaling 1.2`, `allowed_goal_duration_margin 0.5`, `allowed_start_tolerance 0.01`, `execution_duration_monitoring False` ([203–209](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L203-L209)).
- Закрыл RViz → весь launch завершается ([304–307](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L304-L307)).

Остальные launch: `xarm6_moveit_fake.launch.py` (MoveIt + fake-железо, без Gazebo; через `_robot_moveit_fake.launch.py` и `MoveItConfigsBuilder`), `*_realmove` (реальная рука), `*_gazebo` (Classic, не работает), `dual_*`, `demo/`.

### 3.10. `uf_ros_lib/moveit_configs_builder.py` — своя модель руки у MoveIt

[moveit_configs_builder.py](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py) (1703 строки) — класс `MoveItConfigsBuilder` ([строка 210](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py#L210)) собирает все параметры MoveIt. Главное:

```python
self.__urdf_package = Path(get_package_share_directory('xarm_description'))
self.__urdf_file_path = Path('urdf/xarm_device.urdf.xacro')
self.__srdf_file_path = Path('srdf/xarm.srdf.xacro')
```

([строки 338–340](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py#L338-L340)). Аргументы xacro — словарь [строки ~300–327](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py#L300-L327): `add_vacuum_gripper`, `add_realsense_d435i`, `attach_*` и т.д. — **нет** `add_gripper_camera`. kinematics/joint_limits грузятся из `config/<robot>/` ([426](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py#L426), [460](../../src/xarm_ros2/uf_ros_lib/uf_ros_lib/moveit_configs_builder.py#L460)).

**Вывод для студента:** MoveIt **не** берёт `/robot_description` из Gazebo/тележки. Он сам строит модель **только руки** (`world → link_base → ... → link_tcp`) — без тележки и без камеры. Поэтому:
- изменил URDF руки (новый инструмент, длина TCP, пределы) в своём xacro — продублируй это в `xarm_device_macro.xacro`/аргументах, которые видит MoveIt;
- камера и тележка не участвуют в проверке столкновений MoveIt;
- флаги (`add_vacuum_gripper` и т.п.) передавай MoveIt-у явно, как в `cart_xarm6_gz.launch.py`.

> ⚠️ Обрати внимание: наш путь запуска (`_robot_moveit_common.launch.py`) использует **не** `MoveItConfigsBuilder`, а `robot_moveit_config_lib.get_xarm_robot_description_parameters`; `MoveItConfigsBuilder` работает в `_robot_moveit_fake/realmove/gazebo` и `demo/`. Логика одинаковая (тот же `xarm_device.urdf.xacro`), но при доработках проверь оба места.

Также в `uf_ros_lib`: `uf_robot_utils.py` (`get_xacro_command`, `load_yaml`), `parameter_descriptions.py`, `substitutions/` (подстановки для joint_limits и т.п.).

### 3.11. Пакеты для реального робота (кратко)

- `xarm_sdk` — C++ SDK UFactory (TCP-связь с контроллером руки).
- `xarm_api` — ROS-нода-драйвер поверх SDK (`launch/xarm6_driver.launch.py`, `config/xarm_params.yaml`).
- `xarm_msgs` — сообщения и сервисы (`MoveJoint.srv`, `MoveCartesian.srv`, `GripperMove.srv`, …).

В симуляции не нужны, но собираются (от них зависят другие пакеты форка).

### 3.12. Прочее

- `xarm_vision/d435i_xarm_setup` — калибровка реальной D435i + AR-метки (отключён).
- `thirdparty/realsense_gazebo_plugin` — плагин D435i для Gazebo Classic (отключён).
- `demo/mbot_demo` — пример руки на мобильной базе от UFactory (URDF, SRDF, миры, launch); нами не используется.

### 3.13. Отключённые пакеты (`COLCON_IGNORE`)

| Пакет | Почему отключён (из `COLCON_IGNORE`) | Включать? |
|---|---|---|
| `xarm_gazebo` | Gazebo Classic: `<depend>gazebo_ros</depend>`, которого нет в Kilted / gz-sim; замена — `xarm_gz_bringup` | нет |
| `xarm_planner` | требует метапакет `moveit`, неполный в Kilted; планирование даёт `xarm_moveit_config` | нет |
| `xarm_moveit_servo` | API-дрейф `moveit_servo` между релизами MoveIt 2; серво не нужно | нет |
| `thirdparty/realsense_gazebo_plugin` | плагин Gazebo Classic (`gazebo_ros`); файл `COLCON_IGNORE` пустой | нет |
| `xarm_vision/d435i_xarm_setup` | зависит от `find_object_2d` и `object_recognition_msgs` — нет в Kilted; только для реальной D435i | нет |

## 4. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Включить камеру на фланце | [xarm_device_macro.xacro:23](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L23) (вызов из `cart-arm.xacro`) | `add_gripper_camera` | `add_gripper_camera="true"` |
| Сменить модель камеры | [xarm_device_macro.xacro:24](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L24) | `gripper_camera_model` | `'d435i'` |
| Длину TCP присоски | [xarm_vacuum_gripper.urdf.xacro:36](../../src/xarm_ros2/xarm_description/urdf/vacuum_gripper/xarm_vacuum_gripper.urdf.xacro#L36) | `origin xyz` | `0 0 0.14` |
| Сузить пределы сустава | [xarm6.urdf.xacro:5](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.urdf.xacro#L5) **и** [xarm6.ros2_control.xacro:16](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L16) | `joint2_*_limit` | `${-1.5}` |
| Стартовую позу в симе | [xarm6.ros2_control.xacro:8](../../src/xarm_ros2/xarm_description/urdf/xarm6/xarm6.ros2_control.xacro#L8) (обычно через launch-аргумент) | `xarm_initial_positions` | `"0 -0.6 -0.7 0 1.3 0"` |
| Частоту контроллеров | [xarm6_controllers.yaml:3](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L3) | `update_rate` | `250` |
| Допуски траектории | [xarm6_controllers.yaml:27-34](../../src/xarm_ros2/xarm_controller/config/xarm6_controllers.yaml#L27-L34) | `goal_time`, `trajectory`, `goal` | `goal: 0.02` |
| Скорость движений MoveIt | [joint_limits.yaml:7](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/joint_limits.yaml#L7) (и т.д.) | `max_velocity` | `1.0` |
| Другой IK-решатель / таймаут | [kinematics.yaml:2,5](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/kinematics.yaml#L2-L5) | `kinematics_solver`, `timeout` | `0.05` |
| Другой планировщик | [ompl_planning.yaml:2](../../src/xarm_ros2/xarm_moveit_config/config/xarm6/ompl_planning.yaml#L2) | `default_planner_config` | `RRTstar` |
| Именованная поза | [_xarm6_macro.srdf.xacro:42](../../src/xarm_ros2/xarm_moveit_config/srdf/_xarm6_macro.srdf.xacro#L42) | `group_state` | новая `ready` |
| Допуск старта траектории | [_robot_moveit_common.launch.py:207](../../src/xarm_ros2/xarm_moveit_config/launch/_robot_moveit_common.launch.py#L207) | `allowed_start_tolerance` | `0.05` |

## 5. Частые ошибки

| Симптом | Причина | Что делать |
|---|---|---|
| В RViz/MoveIt нет присоски/камеры | MoveIt строит свою модель; флаг не передан | передать флаг в `_robot_moveit_common`; камеру MoveIt не видит вообще |
| MoveIt не исполняет, «state is stale» | `use_sim_time` default False в `_robot_moveit_common` | передать `use_sim_time:=true` явно |
| Меши в gz не видны | `package://` | `ros2_control_plugin` должен быть gz-шным → `file://` |
| `Scene already exists`, segfault | Sensors в мире и в модели (D435i) | `model_sensors_plugin:=false` |
| Две ветви TF у `link_base` | static TF `world→link_base` + тележка | `publish_attach_tf:=false` |
| xacro даёт 7 суставов | дефолт `dof=7` | `dof:=6` |
| Сборка падает на `gazebo_ros`/`moveit` | убрали `COLCON_IGNORE` | вернуть файл |
| Правка пределов не влияет на MoveIt | скорости MoveIt — из `joint_limits.yaml` | править yaml |

## 6. Проверь себя

```bash
cd ~/ros2k_ws && source /opt/ros/kilted/setup.bash && source install/setup.bash
D=src/xarm_ros2/xarm_description/urdf/xarm_device.urdf.xacro

# 1) Модель «как у MoveIt»: только рука + присоска
xacro $D dof:=6 add_vacuum_gripper:=true > /tmp/arm.urdf && check_urdf /tmp/arm.urdf | head -20
grep -c "gripper_camera" /tmp/arm.urdf     # 0 — камеры у MoveIt нет

# 2) TCP присоски
grep -A2 'name="joint_tcp"' /tmp/arm.urdf

# 3) Начальная поза попадает в ros2_control
xacro $D dof:=6 ros2_control_plugin:=gz_ros2_control/GazeboSimSystem \
  xarm_initial_positions:="0 -0.6 -0.7 0 1.3 0" | grep initial_value

# 4) Какие пакеты форка отключены
find src/xarm_ros2 -name COLCON_IGNORE

# 5) MoveIt без Gazebo
ros2 launch xarm_moveit_config xarm6_moveit_fake.launch.py add_vacuum_gripper:=true
```

При запущенной симуляции:

```bash
ros2 param get /move_group use_sim_time
ros2 control list_controllers
ros2 action list | grep follow_joint_trajectory
```

Вопросы:
1. Почему камера Gemini есть в Gazebo, но MoveIt о ней «не знает»?
2. В каких трёх местах живут пределы/ограничения сустава joint2?
3. Почему `xarm_gazebo` нельзя просто включить в Kilted?
4. Где задано, что MoveIt шлёт траектории именно в `xarm6_traj_controller`?
