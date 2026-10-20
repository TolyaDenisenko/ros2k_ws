# Глава 4. Пакет `xarm_grasping` — анализ захвата, «вакуум» и подвод присоски

> Пакет: [`src/xarm_grasping`](../../src/xarm_grasping). Все ссылки в главе — от корня репозитория,
> номера строк проверены на коммите `cb15afc`.

---

## 1. Зачем этот пакет и с чем он связан

У xArm6 на тележке вместо пальцев стоит **вакуумная присоска**, а на запястье — **камера**
(по умолчанию Orbbec Gemini 335Le). Пакет `xarm_grasping` отвечает на три вопроса:

1. **«Куда ставить присоску?»** — нода `grasp_analyzer` смотрит в облако точек камеры,
   ищет стол и объект и строит список вариантов захвата (лучший публикуется как поза).
2. **«Как включить вакуум в симуляции?»** — нода `vacuum_gripper_sim` даёт ROS-сервис
   «включить/выключить», проверяет, что присоска действительно касается коробки, и командует
   плагином Gazebo, который «приваривает» коробку к руке.
3. **«Как быстро подвести присоску к точке без MoveIt?»** — утилита `arm_goto_tcp.py`
   считает обратную кинематику и отправляет траекторию контроллеру руки.

Плюс чистая библиотека `grasp_generator.py` (без ROS), которую использует `grasp_analyzer`.

### Мини-словарь (только то, что нужно для этой главы)

| Термин | Коротко |
|---|---|
| **Нода (node)** | Отдельная программа-участник ROS-графа. Здесь каждая нода — Python-скрипт с классом, унаследованным от `rclpy.node.Node`. |
| **Топик (topic)** | Именованный канал «издатель → подписчики» для потока сообщений (картинки, облака точек, состояния). |
| **Сервис (service)** | Запрос–ответ: клиент шлёт запрос, сервер возвращает ответ. Пример: `/vacuum_gripper/set`. |
| **Action** | «Долгий сервис» с прогрессом и результатом. `arm_goto_tcp.py` — клиент action `FollowJointTrajectory`. |
| **Параметр** | Настройка ноды (число/строка/список), задаётся при запуске: `--ros-args -p имя:=значение` или в launch/YAML. Нода должна объявить его через `declare_parameter()`. |
| **TF** | Дерево систем координат (фреймов) робота: `link_base`, `link_tcp`, `gripper_camera_color_optical_frame`... Позволяет пересчитать точку из одного фрейма в другой. |
| **QoS** | Настройки доставки. `BEST_EFFORT` — можно терять сообщения (для сенсоров); `TRANSIENT_LOCAL` — новый подписчик получит последнее сообщение («защёлка»). |
| **`sensor_msgs/PointCloud2`** | Облако точек: бинарный массив, у каждой точки поля `x, y, z` (и др.) с известными смещениями. |
| **`geometry_msgs/PoseStamped`** | Поза (позиция + ориентация-кватернион) с заголовком: в каком фрейме и в какой момент. |
| **Маркеры RViz** (`visualization_msgs/MarkerArray`) | Примитивы (сферы, стрелки, цилиндры, текст), которые нода рисует в RViz. |
| **`use_sim_time`** | Брать время из `/clock` симулятора, а не системные часы. В Gazebo обязательно `true`. |

### Связи с другими пакетами

```
quasi_camera_description ──(gz rgbd_camera)──► /gripper_camera/points ──► grasp_analyzer
                                                                          │
                                       /xarm_grasping/best_grasp_pose ◄───┘  (+ маркеры, JSON)

cart-arm.xacro (DetachableJoint link6⇄pick_box) ◄─ /vacuum/attach|detach ◄─ vacuum_gripper_sim ◄─ сервис /vacuum_gripper/set
gz_bridge.yaml (cart_xarm_bringup) ─► /vacuum/state, /gz/robot_pose, /gz/object_pose ─► vacuum_gripper_sim

arm_goto_tcp.py ─► /xarm6_traj_controller/follow_joint_trajectory (gz_ros2_control)
```

- Обе ноды запускаются из главного launch
  [`cart_xarm6_gz.launch.py`](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py):
  `vacuum_sim` — [строки 218–229](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L218-L229),
  `grasp_analyzer` — [строки 249–275](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L249-L275).
- Подробности о камере, мосте и плагине вакуума — в [`docs/CAMERA_VACUUM_SIM.md`](../CAMERA_VACUUM_SIM.md).

---

## 2. Дерево пакета

```
src/xarm_grasping/
├── CMakeLists.txt                  установка launch/config, скриптов в lib/, python-модуля
├── package.xml                     описание пакета и зависимости
├── config/
│   └── grasp_params.yaml           YAML-параметры grasp_analyzer (для его собственного launch)
├── launch/
│   └── grasp_analyzer.launch.py    запуск одного grasp_analyzer с params_file
├── scripts/                        исполняемые ноды (ставятся в lib/xarm_grasping)
│   ├── grasp_analyzer.py           облако точек → стол/объект → варианты захвата, маркеры, best_grasp_pose
│   ├── vacuum_gripper_sim.py       сервис /vacuum_gripper/set: проверка касания + attach/detach в gz
│   └── arm_goto_tcp.py             утилита: IK + траектория, поставить TCP в точку без MoveIt
└── xarm_grasping/                  Python-модуль (импортируется как `xarm_grasping.*`)
    ├── __init__.py                 пустой маркер пакета
    └── grasp_generator.py          чистый генератор захватов цилиндра (TOP/SIDE/RIM), без ROS
```

---

## 3. Сборка: `CMakeLists.txt` и `package.xml`

Пакет имеет тип `ament_cmake`, хотя код целиком на Python. Важные места
[`CMakeLists.txt`](../../src/xarm_grasping/CMakeLists.txt):

- [Строки 13–20](../../src/xarm_grasping/CMakeLists.txt#L13-L20): каталоги `launch`, `config`, `worlds`
  ставятся в `share/xarm_grasping/` **только если они существуют и не пусты** (каталога `worlds/`
  сейчас нет — это нормально).
- [Строки 24–27](../../src/xarm_grasping/CMakeLists.txt#L24-L27): **все** `scripts/*.py` собираются glob'ом
  и ставятся как программы в `lib/xarm_grasping/`:

  ```cmake
  file(GLOB _scripts CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.py)
  if(_scripts)
    install(PROGRAMS ${_scripts} DESTINATION lib/${PROJECT_NAME})
  endif()
  ```

  - `CONFIGURE_DEPENDS` заставляет CMake перепроверять список файлов при каждой сборке, поэтому
    **новый скрипт подхватится сам — но только после `colcon build`**. С `--symlink-install`
    правки *существующих* скриптов видны сразу, а *новый* файл без сборки в `install/` не появится.
  - Чтобы `ros2 run xarm_grasping my_node.py` работал, новый скрипт должен:
    1. начинаться с shebang `#!/usr/bin/env python3` (первая строка);
    2. быть исполняемым: `chmod +x src/xarm_grasping/scripts/my_node.py`
       (при symlink-install в `install/` лежит ссылка на ваш файл — права берутся с него);
    3. после этого: `colcon build --symlink-install --packages-select xarm_grasping`.
- [Строка 32](../../src/xarm_grasping/CMakeLists.txt#L32): `ament_python_install_package` ставит модуль
  `xarm_grasping/`, поэтому в скриптах работает `from xarm_grasping.grasp_generator import ...`
  (после `source install/setup.bash`).

[`package.xml`](../../src/xarm_grasping/package.xml) перечисляет `rclpy`, `sensor_msgs`, `geometry_msgs`,
`visualization_msgs`, `tf2_ros`, `cv_bridge`, `python3-numpy` и др.

> ⚠️ **Обрати внимание:** в `package.xml` не указаны `std_srvs`, `tf2_msgs`, `control_msgs`,
> `trajectory_msgs` и `python3-scipy`, хотя скрипты их импортируют. На машине с полной установкой
> ROS это не мешает, но `rosdep install` их сам не поставит.

---

## 4. `scripts/grasp_analyzer.py` — анализ сцены и варианты захвата

Файл: [`scripts/grasp_analyzer.py`](../../src/xarm_grasping/scripts/grasp_analyzer.py) (345 строк).

### 4.1 Интерфейс

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| подписка | `color_topic` (по умолч. `/camera/color/image_raw`) | `sensor_msgs/Image` | цветная картинка — только для пересылки на дисплей |
| подписка | `points_topic` (по умолч. `/camera/depth/color/points`) | `sensor_msgs/PointCloud2` | облако точек для анализа |
| публикация | `/xarm_grasping/grasp_markers` | `visualization_msgs/MarkerArray` (TRANSIENT_LOCAL) | цилиндр объекта, точки/стрелки/подписи вариантов |
| публикация | `/xarm_grasping/grasp_candidates` | `std_msgs/String` | JSON со всеми вариантами и отладкой |
| публикация | `/xarm_grasping/best_grasp_pose` | `geometry_msgs/PoseStamped` | лучшая поза TCP во фрейме `base_frame` |
| публикация | `/camera/image_display` | `sensor_msgs/Image` | копия цветной картинки (через `cv_bridge`) |
| TF | `base_frame` ← фрейм облака | lookup | перевод центра объекта в `link_base` |

Подписки создаются с QoS `BEST_EFFORT, depth=2` ([строка 100](../../src/xarm_grasping/scripts/grasp_analyzer.py#L100)),
публикаторы — [строки 113–118](../../src/xarm_grasping/scripts/grasp_analyzer.py#L113-L118).

### 4.2 Параметры

Объявлены в [строках 69–80 и 95–96](../../src/xarm_grasping/scripts/grasp_analyzer.py#L69-L96).

| Параметр | Дефолт в коде | В главном launch | Смысл |
|---|---|---|---|
| `color_topic` | `/camera/color/image_raw` | `/gripper_camera/color/image_raw` (Orbbec) | топик картинки |
| `points_topic` | `/camera/depth/color/points` | `/gripper_camera/points` (Orbbec) / `/camera/depth/points` (d435i) | облако точек |
| `camera_frame` | `camera_color_optical_frame` | `gripper_camera_color_optical_frame` (Orbbec) | фрейм камеры (в коде фактически не используется для TF, см. ниже) |
| `base_frame` | `link_base` | `link_base` | фрейм, в котором считаются захваты |
| `table_height` | `1.0` | **не передаётся** | z столешницы в `base_frame`, м |
| `block_radius` | `0.04` | — | радиус цилиндра (априори), м |
| `block_height` | `0.09` | — | высота цилиндра, м |
| `block_x`, `block_y` | `0.35`, `-0.75` | — | запасная (априорная) позиция объекта в `base_frame` |
| `suction_radius` | `0.02` | — | радиус пятна присоски, м |
| `approach_offset` | `0.10` | — | отход pre-grasp вдоль оси подхода, м |
| `analyze_period` | `2.0` | `2.0` | период анализа, с |
| `voxel_size` | `0.01` | — | размер вокселя для прореживания, м |
| `min_points` | `150` | — | минимум точек перед камерой |
| `use_sim_time` | (объявляет сам rclpy) | `true` | время из `/clock` |

Важная деталь из комментария на [строках 67–68](../../src/xarm_grasping/scripts/grasp_analyzer.py#L67-L68):
`use_sim_time` объявлять повторно нельзя — rclpy уже объявил его, будет `ParameterAlreadyDeclaredException`.

### 4.3 Алгоритм (метод `analyze_scene`, раз в `analyze_period` секунд)

1. **Приём облака** ([строки 46–59](../../src/xarm_grasping/scripts/grasp_analyzer.py#L46-L59),
   [139–142](../../src/xarm_grasping/scripts/grasp_analyzer.py#L139-L142)). Функция
   `points_from_msg` вручную достаёт `x, y, z` из бинарного буфера по смещениям полей и
   выбрасывает `NaN/inf`. Колбэк только запоминает последнее облако и его `frame_id`.
2. **Воксельная сетка** ([155–157](../../src/xarm_grasping/scripts/grasp_analyzer.py#L155-L157)) —
   по одной точке на куб `voxel_size`, чтобы считать быстрее.
3. **Только то, что перед камерой**: `z > 0.05` в оптическом фрейме (у оптического фрейма ось Z
   смотрит вперёд из объектива). Если точек меньше `min_points` — пропуск с предупреждением.
4. **Поиск стола** ([168–175](../../src/xarm_grasping/scripts/grasp_analyzer.py#L168-L175)):
   гистограмма z на 60 корзин, самая полная корзина = плоскость стола. Точки в ±2 см от неё —
   стол; точки *ближе* к камере (меньший z) — объект.

   ```python
   zs = front[:, 2]
   hist, edges = np.histogram(zs, bins=60)
   b = int(np.argmax(hist))
   z_plane = 0.5 * (edges[b] + edges[b + 1])
   table_mask = np.abs(front[:, 2] - z_plane) < 0.02
   table_pts = front[table_mask]
   obj_mask = (~table_mask) & (front[:, 2] < z_plane)  # ближе к камере = выше стола
   ```
5. **Центр объекта** ([181–190](../../src/xarm_grasping/scripts/grasp_analyzer.py#L181-L190)):
   если точек объекта ≥ `min_points // 2` — среднее, плюс оценка радиуса (95-й перцентиль
   расстояний в плоскости XY камеры, идёт только в JSON).
6. **Перевод в `link_base` через TF** ([194–214](../../src/xarm_grasping/scripts/grasp_analyzer.py#L194-L214)):
   TF-слушатель создаётся при первом вызове; берётся только `x, y` центра. При **любой** ошибке
   (нет TF, нет объекта) остаётся `block_xy = (block_x, block_y)` — сообщение уровня `debug`, т.е.
   по умолчанию его не видно.
7. **Генерация вариантов** ([217–222](../../src/xarm_grasping/scripts/grasp_analyzer.py#L217-L222)):
   `generate_cylinder_grasps(...)` из `grasp_generator.py` с `block_radius`, `block_height`,
   `table_height` из параметров (а не из облака!).
8. **Публикация** ([224–239](../../src/xarm_grasping/scripts/grasp_analyzer.py#L224-L239)): JSON,
   маркеры, `PoseStamped` первого (лучшего) варианта; RPY → кватернион в `_rpy_to_quat`.
9. **Маркеры** ([287–322](../../src/xarm_grasping/scripts/grasp_analyzer.py#L287-L322)): полупрозрачный
   оранжевый цилиндр объекта (ns `object`), сферы-точки касания (`grasps`), стрелки pre-grasp→grasp
   (`arrows`), подписи `#i TYPE s=score` (`labels`). Цвета: TOP зелёный, SIDE синий, RIM жёлтый,
   лучший — ярко-зелёный. Время жизни маркера 5 с.

> 💡 В `_marker` ([строки 273–284](../../src/xarm_grasping/scripts/grasp_analyzer.py#L273-L284)) все
> числа явно приводятся к `float`, а `lifetime` — через `.to_msg()`. Если передать `int`/numpy-тип
> или `rclpy.duration.Duration`, rosidl делает `assert` и **весь процесс падает** (abort).

> ⚠️ **Обрати внимание (известные ограничения):**
> - Пайплайн **предполагает цилиндр**. Реальный объект в сцене — коробка `pick_box` 0.10×0.05×0.05 м,
>   но варианты строятся для цилиндра `block_radius`/`block_height`.
> - Если TF или сегментация не сработали, нода **молча** берёт априорную позу `block_x/block_y`
>   (`0.35, -0.75` — это от старой сцены `cylinder_block`, а не `pick_box` на x≈0.6).
> - `table_height` **не передаётся** из `cart_xarm6_gz.launch.py`, поэтому действует дефолт `1.0`
>   (в `link_base` верх стола на самом деле ≈ `+0.006`). В `config/grasp_params.yaml` стоит `-0.28` —
>   но этот YAML читает только `grasp_analyzer.launch.py`, не главный launch.
> - Поиск стола предполагает, что камера **смотрит вниз** (стол перпендикулярен оптической оси).
> - Параметр `camera_frame` в TF не участвует — используется `frame_id` из самого облака
>   ([строки 150–152](../../src/xarm_grasping/scripts/grasp_analyzer.py#L150-L152)).

---

## 5. `xarm_grasping/grasp_generator.py` — генератор захватов (без ROS)

Файл: [`xarm_grasping/grasp_generator.py`](../../src/xarm_grasping/xarm_grasping/grasp_generator.py).
Только numpy/scipy, можно запускать без ROS:

```bash
cd ~/ros2k_ws/src/xarm_grasping && python3 -m xarm_grasping.grasp_generator
```

Функции:

| Функция | Строки | Что делает |
|---|---|---|
| `rotation_z(angle)` | [31–35](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L31-L35) | матрица поворота вокруг Z (сейчас не используется) |
| `grasp_pose(position, approach_dir)` | [38–53](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L38-L53) | 4×4 поза TCP, ось Z инструмента смотрит в объект (`-approach`) |
| `matrix_to_xyzrpy(T)` | [56–61](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L56-L61) | 4×4 → `[x,y,z,roll,pitch,yaw]` |
| `generate_cylinder_grasps(...)` | [64–140](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L64-L140) | список вариантов, отсортированный по `score` |
| `pick_best(grasps, ...)` | [143–155](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L143-L155) | выбрать лучший (с простейшей «видимостью»); в ноде не используется |

Сигнатура ([строки 64–66](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L64-L66)):

```python
def generate_cylinder_grasps(center_xy=(0.35, -0.75), radius=0.04, height=0.09,
                             table_z=1.0, suction_radius=0.02,
                             approach_offset=0.10, n_side=4, n_rim=6):
```

Три типа вариантов (цилиндр стоит основанием на `table_z`):

| Тип | Строки | Точка контакта | Ориентация TCP | `score` |
|---|---|---|---|---|
| `TOP` (1 шт.) | [78–98](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L78-L98) | центр крышки, z = `table_z + H` | `rpy = (π, 0, 0)` — присоской вниз | `0.9·min(1, r²/R²) + 0.1` |
| `SIDE` (`n_side`=4) | [100–118](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L100-L118) | на стенке, середина высоты, азимуты 0/90/180/270° | `rpy = (π/2, 0, az)` | `0.55·max(0, 1 − r/R) + 0.15` |
| `RIM` (`n_rim`=6) | [120–137](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L120-L137) | на крышке у края, на расстоянии `R − r` от центра | `rpy = (π, 0, 0)` | `0.5·(R−r)/R + 0.2` |

Каждый вариант — словарь `{type, label, pose, pre_grasp, score, contact_normal}`.
`pre_grasp` — та же точка, отодвинутая на `approach_offset` против направления подхода.
При дефолтах (`r=0.02`, `R=0.04`): TOP=0.325, SIDE=0.425, RIM=0.45 — то есть **первым (лучшим)
оказывается RIM**, а не TOP. Это следствие формул, а не ошибка запуска; если хотите, чтобы
TOP выигрывал, — меняйте веса в этих строках.

> ⚠️ **Обрати внимание:** ориентация в `pose` (из `Rotation.from_euler`) и в `pre_grasp`
> (из `grasp_pose`) строятся разными способами и для SIDE могут отличаться поворотом вокруг оси
> инструмента. Для присоски это обычно не важно, но при отправке в MoveIt проверьте.

---

## 6. `scripts/vacuum_gripper_sim.py` — «логический» вакуум

Файл: [`scripts/vacuum_gripper_sim.py`](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py) (286 строк).

В Gazebo нет физики присоски. Вместо неё в модель робота (`quasi_cart_description/urdf/cart-arm.xacro`)
добавлен плагин `gz::sim::systems::DetachableJoint`: по сообщению на `/vacuum/attach` он создаёт
**жёсткое (fixed) соединение** `link6 ⇄ pick_box`, по `/vacuum/detach` — удаляет его. Эта нода решает,
*когда* attach разрешён, и даёт удобный ROS-сервис.

> ⚠️ **Обрати внимание:** вакуум = **логическая сварка** DetachableJoint, а не силы присасывания.
> Коробка «прилипает» мгновенно и держится при любом ускорении; вес и герметичность не моделируются.

### 6.1 Интерфейс

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| **сервис** | `/vacuum_gripper/set` | `std_srvs/srv/SetBool` | `data: true` — включить (с проверкой касания), `false` — выключить |
| публикация | `/vacuum_gripper/state` | `std_msgs/Bool` (TRANSIENT_LOCAL) | держит ли объект (по подтверждению плагина) |
| публикация | `/vacuum_gripper/status` | `std_msgs/String`, 2 Гц | текст: расстояние TCP→объект, угол, вкл/выкл, держит |
| публикация | `/vacuum/attach`, `/vacuum/detach` | `std_msgs/Empty` | команды плагину (через ros_gz_bridge) |
| подписка | `/vacuum/state` | `std_msgs/String` | ответ плагина `attached`/`detached` |
| подписка | `/gz/robot_pose`, `/gz/object_pose` | `tf2_msgs/TFMessage` | мировые позы моделей из gz (10 Гц) |
| TF | `root_frame` → `tcp_frame` | lookup | поза кончика присоски относительно корня робота |

Код: [строки 120–137](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L120-L137).

### 6.2 Параметры ([строки 93–101](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L93-L101))

| Параметр | Дефолт | Смысл |
|---|---|---|
| `robot_model_name` | `cart_xarm6` | имя модели робота в gz (`child_frame_id` в `/gz/robot_pose`) |
| `object_model_name` | `pick_box` | имя модели объекта в gz |
| `root_frame` | `base_footprint` | корневой фрейм робота (корень SDF) |
| `tcp_frame` | `link_tcp` | кончик присоски |
| `object_size` | `[0.10, 0.05, 0.05]` | размер бокса объекта по его локальным осям, м |
| `contact_distance` | `0.015` | макс. расстояние TCP до поверхности, м |
| `max_angle_deg` | `20.0` | макс. угол между осью инструмента и нормалью грани, ° |
| `require_contact` | `true` | `false` — attach без проверки (отладка) |
| `attach_confirm_timeout` | `1.0` | сколько ждать подтверждения от плагина, с |

Главный launch передаёт `use_sim_time`, `robot_model_name`, `object_model_name`, `object_size`.
`contact_distance`, `max_angle_deg`, `require_contact`, `attach_confirm_timeout` читаются
**при каждом вызове сервиса**, поэтому их можно менять «на лету» через `ros2 param set`.
Остальные читаются один раз в конструкторе.

### 6.3 Алгоритм

1. **Позы из gz** (`on_poses`, [строки 147–154](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L147-L154)):
   запоминает 4×4 матрицы робота и объекта в мире по `child_frame_id`.
2. **Поза TCP в мире** (`tcp_in_world`, [173–183](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L173-L183)):
   `T_world←tcp = T_world←robot · T_root←tcp` (вторая часть — из TF `robot_state_publisher`).
3. **Метрики касания** (`contact_metrics`, [185–207](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L185-L207)):
   точка TCP переводится в систему объекта; расстояние до бокса = длина «вылета» за полуразмеры
   (0, если внутри). Ближайшая грань — ось с наибольшим вылетом; угол — между +Z TCP и
   направлением *внутрь* этой грани.

   ```python
   T_obj_tcp = np.linalg.inv(self.T_world_object) @ T_w_tcp
   p = T_obj_tcp[:3, 3]
   over = np.abs(p) - self.half            # >0 — снаружи по этой оси
   dist = float(np.linalg.norm(np.maximum(over, 0.0)))
   axis = int(np.argmax(over))
   n = np.zeros(3)
   n[axis] = math.copysign(1.0, p[axis])
   tool_z = T_obj_tcp[:3, 2]
   cosang = float(np.clip(np.dot(tool_z, -n), -1.0, 1.0))
   ```
4. **Сервис** (`on_set`, [210–257](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L210-L257)):
   - `false` → `requested=False`, шлёт detach, `success=true`;
   - `true` и уже держит → `success=true, "уже держит"`;
   - иначе (если `require_contact`) проверяет `dist ≤ contact_distance` и `угол ≤ max_angle_deg`;
     при отказе — `success=false` с причиной в `message` (мм и градусы);
   - шлёт attach и ждёт до `attach_confirm_timeout` подтверждения `/vacuum/state`.
5. **Автоотпускание при старте** ([строки 162–167](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L162-L167)):
   плагин DetachableJoint **сам приваривает объект при загрузке**. Если вакуум никто не включал
   (`requested=False`), нода сразу шлёт detach — иначе коробка «поехала» бы с рукой.
6. **Многопоточность** ([строки 129–137, 271–277](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L129-L137)):
   сервис *ждёт* сообщения из подписки, поэтому всё в `ReentrantCallbackGroup` и
   `MultiThreadedExecutor(num_threads=4)`. С обычным `rclpy.spin` сервис бы заблокировал сам себя
   и всегда отвечал «плагин не подтвердил».

---

## 7. `scripts/arm_goto_tcp.py` — подвести присоску к точке без MoveIt

Файл: [`scripts/arm_goto_tcp.py`](../../src/xarm_grasping/scripts/arm_goto_tcp.py) (213 строк).
**Отладочная утилита, не планировщик**: столкновения не проверяются, траектория — прямая
интерполяция в суставах.

### 7.1 Интерфейс

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| подписка | `/robot_description` | `std_msgs/String` (TRANSIENT_LOCAL) | URDF — из него строится цепочка `link_base → link_tcp` |
| подписка | `/joint_states` | `sensor_msgs/JointState` | текущие углы `joint1..joint6` (старт IK) |
| action-клиент | `/xarm6_traj_controller/follow_joint_trajectory` | `control_msgs/action/FollowJointTrajectory` | исполнение траектории |

### 7.2 Аргументы командной строки ([строки 151–158](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L151-L158))

Это не ROS-параметры, а обычные `argparse`-флаги (лишние аргументы, например `--ros-args`, уходят в rclpy).

| Флаг | Дефолт | Смысл |
|---|---|---|
| `--x`, `--y`, `--z` | обязательны | цель TCP в `link_base`, м |
| `--lift` | `0.10` | сначала точка на столько выше цели (0 — сразу в цель) |
| `--time` | `6.0` | время на каждый участок, с |
| `--dry-run` | выкл | только посчитать IK, не двигать |

### 7.3 Алгоритм

1. Ждёт `/robot_description` и `/joint_states` ([строки 162–164](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L162-L164)).
2. `Chain` ([58–89](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L58-L89)) разбирает URDF, идёт от
   `link_tcp` к `link_base` по родителям, запоминает пределы суставов; `fk(q)` — прямая кинематика.
3. `solve_ik` ([92–124](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L92-L124)) — `scipy.optimize.least_squares`
   с двумя целями: позиция (вес 10) и ось инструмента строго вниз (вес 3); yaw свободен.
   Пробует 5 стартов (текущая поза и повороты `joint1` на ±0.5, ±1 рад), затем «сворачивает» углы
   на ±2π к текущим, чтобы рука не крутилась лишний оборот.
4. Проверка ([строка 178](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L178)): ошибка позиции
   > 5 мм или наклон > 10° → `IK не сошлась` и выход с кодом 1.
5. Отправляет одну цель с 1–2 точками (подъём + цель), ждёт результат; `error_code=0` — успех.

Полезные числа из docstring ([строки 21–26](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L21-L26)) для
`cart_table_offline.world` при spawn 0,0: `link_base` в мире = (−0.0075, 0, 0.919); верх стола в
`link_base` z = +0.006; `pick_box` в центре (0.6075, 0, 0.031), его верх z = +0.056.

---

## 8. `launch/grasp_analyzer.launch.py` и `config/grasp_params.yaml`

[`grasp_analyzer.launch.py`](../../src/xarm_grasping/launch/grasp_analyzer.launch.py) запускает **только**
`grasp_analyzer` с двумя аргументами:

| Аргумент | Дефолт | Строка |
|---|---|---|
| `use_sim_time` | `true` | [24](../../src/xarm_grasping/launch/grasp_analyzer.launch.py#L24) |
| `params_file` | `share/xarm_grasping/config/grasp_params.yaml` | [26](../../src/xarm_grasping/launch/grasp_analyzer.launch.py#L26) |

```python
Node(
    package='xarm_grasping',
    executable='grasp_analyzer.py',
    name='grasp_analyzer',
    output='screen',
    parameters=[
        LaunchConfiguration('params_file'),
        {'use_sim_time': LaunchConfiguration('use_sim_time')},
    ],
),
```

`executable='grasp_analyzer.py'` — имя файла вместе с `.py`, потому что CMake ставит скрипт как есть.

[`grasp_params.yaml`](../../src/xarm_grasping/config/grasp_params.yaml): верхний ключ `grasp_analyzer:` должен
совпадать с `name` ноды, имена параметров — с `declare_parameter()`.

> ⚠️ **Обрати внимание:** YAML устарел относительно текущей сцены: топики D435i
> (`/camera/...`), `table_height: -0.28`, `block_x/y` от сцены с цилиндром. С камерой Orbbec
> (по умолчанию) этот launch получит не те топики — переопределяйте их (см. ниже). Главный launch
> `cart_xarm6_gz.launch.py` этот YAML **не использует**.

---

## 9. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| задать реальную высоту стола для grasp_analyzer | [cart_xarm6_gz.launch.py:255–275](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L255-L275) (добавить словарь) / [grasp_analyzer.py:73](../../src/xarm_grasping/scripts/grasp_analyzer.py#L73) | `table_height` | в launch: `{'table_height': 0.006}`; CLI: `ros2 run xarm_grasping grasp_analyzer.py --ros-args -p use_sim_time:=true -p table_height:=0.006` |
| размеры объекта для генератора | [grasp_analyzer.py:74–75](../../src/xarm_grasping/scripts/grasp_analyzer.py#L74-L75) | `block_radius`, `block_height` | `-p block_radius:=0.025 -p block_height:=0.05` |
| запасную позицию объекта | [grasp_analyzer.py:95–96](../../src/xarm_grasping/scripts/grasp_analyzer.py#L95-L96) | `block_x`, `block_y` | `-p block_x:=0.6075 -p block_y:=0.0` |
| другие топики камеры | [grasp_analyzer.py:69–71](../../src/xarm_grasping/scripts/grasp_analyzer.py#L69-L71) | `color_topic`, `points_topic`, `camera_frame` | `-p points_topic:=/gripper_camera/points -p color_topic:=/gripper_camera/color/image_raw` |
| анализ чаще/реже | [grasp_analyzer.py:78](../../src/xarm_grasping/scripts/grasp_analyzer.py#L78) | `analyze_period` | `-p analyze_period:=0.5` (читается один раз при старте) |
| чувствительность сегментации | [grasp_analyzer.py:79–80](../../src/xarm_grasping/scripts/grasp_analyzer.py#L79-L80), допуск стола [172](../../src/xarm_grasping/scripts/grasp_analyzer.py#L172) | `voxel_size`, `min_points`; ±0.02 м — в коде | `-p voxel_size:=0.005 -p min_points:=80` |
| длину подхода pre-grasp | [grasp_analyzer.py:77](../../src/xarm_grasping/scripts/grasp_analyzer.py#L77) | `approach_offset` | `-p approach_offset:=0.15` |
| чтобы TOP был лучшим | [grasp_generator.py:96, 116, 135](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L96) | формулы `score` | поднять вес TOP или снизить RIM |
| число боковых/ободковых вариантов | [grasp_generator.py:66](../../src/xarm_grasping/xarm_grasping/grasp_generator.py#L66) | `n_side`, `n_rim` | передать в вызове на [grasp_analyzer.py:217](../../src/xarm_grasping/scripts/grasp_analyzer.py#L217) |
| смягчить проверку касания вакуума | [vacuum_gripper_sim.py:98–99](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L98-L99) | `contact_distance`, `max_angle_deg` | на лету: `ros2 param set /vacuum_gripper_sim contact_distance 0.03` |
| attach без проверки (отладка) | [vacuum_gripper_sim.py:100](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L100) | `require_contact` | `ros2 param set /vacuum_gripper_sim require_contact false` |
| другой объект/размер | [cart_xarm6_gz.launch.py:224–227](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L224-L227) | `object_model_name`, `object_size` | `'object_size': [0.06, 0.06, 0.12]` (+ `vacuum_target_model` в `cart-arm.xacro`) |
| дольше ждать подтверждения плагина | [vacuum_gripper_sim.py:101](../../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L101) | `attach_confirm_timeout` | `ros2 param set /vacuum_gripper_sim attach_confirm_timeout 3.0` |
| допуски IK в arm_goto_tcp | [arm_goto_tcp.py:178](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L178) | `err > 0.005 or tilt > 10` в коде | правка кода |
| медленнее/быстрее двигать руку | [arm_goto_tcp.py:156](../../src/xarm_grasping/scripts/arm_goto_tcp.py#L156) | `--time` | `--time 10` |
| запустить отдельный анализатор со своим YAML | [grasp_analyzer.launch.py:26](../../src/xarm_grasping/launch/grasp_analyzer.launch.py#L26) | `params_file` | `ros2 launch xarm_grasping grasp_analyzer.launch.py params_file:=/path/my.yaml` |

Как передать параметр **через launch** — добавьте словарь в список `parameters` ноды:

```python
grasp_analyzer = Node(
    package='xarm_grasping', executable='grasp_analyzer.py', name='grasp_analyzer',
    parameters=[{'use_sim_time': True},
                {'table_height': 0.006, 'block_x': 0.6075, 'block_y': 0.0}],
)
```

Более поздний элемент списка перекрывает ранний (поэтому в `grasp_analyzer.launch.py`
`use_sim_time` из аргумента перекрывает значение из YAML).

---

## 10. Частые ошибки

| Симптом | Причина | Что делать |
|---|---|---|
| `No executable found` / `libexec directory ... does not exist` | новый скрипт без сборки, без `chmod +x` или без shebang | `chmod +x`, shebang в 1-й строке, `colcon build --symlink-install --packages-select xarm_grasping` |
| `ModuleNotFoundError: xarm_grasping` | не выполнен `source install/setup.bash` | `source ~/ros2k_ws/install/setup.bash` |
| `Облако точек ещё не получено (ждём камеру из Gazebo)...` | не тот `points_topic`, Gazebo не запущен или симуляция на паузе | `ros2 topic list | grep points`; сверить с параметром |
| Захваты всегда у `(0.350, -0.750)` | TF/сегментация не сработали → априорная поза | `--ros-args --log-level grasp_analyzer:=debug`, проверить TF |
| Маркеры висят над/под столом | `table_height=1.0` по умолчанию | передать `table_height` |
| `ParameterAlreadyDeclaredException: use_sim_time` | добавили `declare_parameter('use_sim_time')` | не объявлять, rclpy делает это сам |
| Процесс падает с assert в rosidl при публикации маркера | в поле сообщения передали `int`/numpy/`rclpy.duration.Duration` | приводить к `float`, `Duration(...).to_msg()` |
| `/vacuum_gripper/set` → `отказ: нет касания — расстояние ... мм` | TCP дальше 15 мм или угол > 20° | подвести `arm_goto_tcp.py`, смотреть `/vacuum_gripper/status` |
| `отказ: нет позы робота из gz` | мост `/gz/robot_pose` не работает | `ros2 topic hz /gz/robot_pose`, проверить `gz_bridge.yaml` |
| `attach отправлен, но плагин не подтвердил` | нет модели `pick_box` или плагин не загружен | `ros2 topic echo /vacuum/state --once`, см. `docs/STATUS.md` |
| Коробка «едет» с рукой сразу после старта | плагин приварил при загрузке, а `vacuum_gripper_sim` не запущен | запустить ноду — она отпустит автоматически |
| `IK не сошлась (точка вне досягаемости?)` | цель вне рабочей зоны или нельзя смотреть строго вниз | `--dry-run`, проверить координаты в `link_base` |
| `нет action-сервера xarm6_traj_controller` | контроллер не заспавнен | `ros2 control list_controllers` |

---

## 11. Проверь себя

Предполагается, что запущен `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py` и во всех
терминалах выполнено `source ~/ros2k_ws/install/setup.bash`.

**Генератор без ROS:**

```bash
cd ~/ros2k_ws/src/xarm_grasping && python3 -m xarm_grasping.grasp_generator | tail -1
# BEST: RIM 0.45
```

**grasp_analyzer:**

```bash
ros2 node info /grasp_analyzer
ros2 param get /grasp_analyzer table_height                 # 1.0 — дефолт!
ros2 topic echo /xarm_grasping/best_grasp_pose --once       # geometry_msgs/msg/PoseStamped, frame_id: link_base
ros2 topic echo /xarm_grasping/grasp_candidates --once --full-length   # JSON
ros2 topic hz /xarm_grasping/best_grasp_pose                # ~0.5 Гц при analyze_period=2.0
```

В RViz: *Add → By topic → `/xarm_grasping/grasp_markers` → MarkerArray*.

Отдельный запуск со своими параметрами (камера Orbbec):

```bash
ros2 run xarm_grasping grasp_analyzer.py --ros-args -r __node:=grasp_test \
  -p use_sim_time:=true -p points_topic:=/gripper_camera/points \
  -p color_topic:=/gripper_camera/color/image_raw -p table_height:=0.006
```

**Вакуум:**

```bash
ros2 topic echo /vacuum_gripper/state --once         # data: false
ros2 topic echo /vacuum_gripper/status --once        # расстояние/угол/держит
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
# далеко от коробки → success: false, message: 'отказ: нет касания — ...'

ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.058 --dry-run   # только IK
ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.058             # присоска на крышку
ros2 topic echo /vacuum_gripper/status --once        # < 15 мм, < 20°
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"       # success: true
ros2 topic echo /vacuum/state --once                 # data: attached

ros2 run xarm_grasping arm_goto_tcp.py --x 0.6075 --y 0.0 --z 0.21 --lift 0     # поднять с коробкой
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: false}"      # отпустить
ros2 topic echo /vacuum/state --once                 # data: detached
```

**Вопросы для самопроверки:**

1. Почему `vacuum_gripper_sim` использует `MultiThreadedExecutor`, а `grasp_analyzer` — обычный `rclpy.spin`?
2. Какое значение `table_height` получает `grasp_analyzer` в главном launch и почему?
3. Что произойдёт, если в `scripts/` добавить `my_tool.py` без `chmod +x` и сразу вызвать `ros2 run`?
4. Почему при дефолтных параметрах лучшим выходит вариант RIM?
5. Какие два условия проверяются перед attach и где их ослабить без перезапуска?
