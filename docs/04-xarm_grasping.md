# Глава 4. Пакет `xarm_grasping` — анализ захвата, «вакуум» и подвод присоски

> Пакет: [`src/xarm_grasping`](../src/xarm_grasping). Номера строк в ссылках сверены с кодом
> 2026-11-13; если код поменяется, ищите по имени функции.

---

## 1. Зачем этот пакет и с чем он связан

У xArm6 вместо пальцев стоит **вакуумная присоска**, а на запястье — **камера**
(по умолчанию Orbbec Gemini 335Le). Пакет `xarm_grasping` — всё, что нужно, чтобы
найти предмет, взять его присоской и поставить в нужное место. В нём два поколения кода:

* **Старое (эта глава подробно):** `grasp_analyzer` (облако точек → варианты захвата
  цилиндра), `vacuum_gripper_sim` («вакуум» в Gazebo), `arm_goto_tcp.py` (подвести
  присоску к точке без MoveIt) и библиотека `grasp_generator.py`.
* **Новое — конвейер pick & place** (сцена «рука на столе»): распознавание, сцена
  MoveIt, взятие по названию. Подробно — главы 9–13 и [PICK_PLACE.md](PICK_PLACE.md);
  здесь только перечень.

### Узлы пакета (`scripts/`)

| Узел | Что делает | Главные интерфейсы | Где запускается | Подробно |
|---|---|---|---|---|
| `grasp_analyzer.py` | облако точек → стол, предмет → варианты захвата **цилиндра** | `/xarm_grasping/best_grasp_pose`, `/grasp_markers`, `/grasp_candidates` | `cart_xarm6_gz` | раздел 4 |
| `vacuum_gripper_sim.py` | «вакуум» в симуляции: проверка касания + DetachableJoint | сервис `/vacuum_gripper/set`, топики `/vacuum_gripper/{cmd,state,held,ready,contact,markers,status}` | `cart_xarm6_gz`, `arm_table_gz` | раздел 6, [глава 9](09-вакуум-кнопка-готовность.md) |
| `vacuum_gripper_xarm.py` | вакуум **реальной руки**: тот же `/vacuum_gripper/*` поверх драйвера xArm, защита насоса | `/xarm/set_vacuum_gripper`, `/xarm/get_vacuum_gripper` → `/vacuum_gripper/{set,cmd,state,held,status}` | вручную (в launch не включён) | [глава 9, 9.3а](09-вакуум-кнопка-готовность.md) |
| `vacuum_rviz_button.py` | кнопка вакуума в RViz (интерактивный маркер) | `/vacuum_button` | `cart_xarm6_gz`, `arm_table_gz` | глава 9 |
| `arm_parking.py` | «тормоза» руки при езде тележки (переключение на `arm_hold_controller`) | `/arm_parking/set`, `/arm_parking/state` | `cart_xarm6_gz` | глава 9, 9.6 |
| `arm_goto_tcp.py` | утилита: IK + траектория, поставить TCP в точку без MoveIt | action `/xarm6_traj_controller/follow_joint_trajectory` | вручную | раздел 7 |
| `scene_markers.py` | истинная сцена Gazebo в RViz (для сравнения) | `/scene_markers` | оба launch | — |
| `object_perception.py` | облако → стол, кластеры, форма и класс предметов; ставит обновление на паузу, пока идёт осмотр (`/survey/active`) | `/objects`, `/objects/markers`, action `/detect_objects` | `arm_table_gz` | [глава 10](10-восприятие-объектов.md) |
| `openvino_detector.py` | нейросеть по картинке (OpenVINO), рамки с названиями | `/detections`, `/detections/image` | `arm_table_gz detector:=true` | [RECOGNITION_OPENVINO.md](RECOGNITION_OPENVINO.md) |
| `scene_manager.py` | предметы и зоны в Planning Scene MoveIt, выбор места постановки | action `/find_place_pose`, `/place_zones/markers` | `arm_table_gz` | главы [11](11-сцена-moveit.md), [12](12-разметка-стола-и-зоны.md) |
| `pick_place_server.py` | «возьми и поставь» через MoveIt; трижды проверяет `/vacuum_gripper/state` | action `/pick_place` | `arm_table_gz` | [глава 13](13-pick-place.md), [PICK_PLACE.md](PICK_PLACE.md) |
| `pick_by_name.py` | команды текстом «возьми куб 2», диалог «этот? да/нет/стоп» | `/pick_command` → `/pick_status`, `/pick_status_json`, `/pick_by_name/{image,marker}` | `arm_table_gz` | [PICK_PLACE.md](PICK_PLACE.md) |
| `survey_objects.py` | осмотр стола с 5 сторон под 45°, голосование нейросети | сервис `/survey_objects` → `/objects_refined`, `/survey/{active,status,votes}` | `arm_table_gz` | [PICK_PLACE.md](PICK_PLACE.md) |

Python-модуль `xarm_grasping/`: `grasp_generator.py` (для `grasp_analyzer`),
`perception.py` (чистый numpy: стол, кластеры, подгонка коробки/цилиндра, классы по
правилам `config/objects.yaml`), `detector.py` (ядро распознавания на OpenVINO, без ROS).
Каталог `training/` — обучение и проверка своей модели распознавания
([MODEL_TRAINING.md](MODEL_TRAINING.md)).

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

cart-arm.xacro (DetachableJoint link6⇄<объект>, по плагину на объект)
      ◄─ /vacuum/<объект>/attach|detach ◄─ vacuum_gripper_sim ◄─ сервис /vacuum_gripper/set, топик /cmd
gz_bridge*.yaml ─► /vacuum/<объект>/state, /gz/robot_pose, /gz/object_pose ─► vacuum_gripper_sim
vacuum_gripper_sim ─► /vacuum_gripper/state (Bool) ─► pick_place_server (проверка «взят»)

arm_goto_tcp.py ─► /xarm6_traj_controller/follow_joint_trajectory (gz_ros2_control)
```

- В сцене «рука на тележке» ноды запускает
  [`cart_xarm6_gz.launch.py`](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py)
  (только при `manipulation_nodes:=true` и `arm:=true`):
  `vacuum_sim` — [строки 330–343](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L330-L343),
  `grasp_analyzer` — [строки 373–402](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L373-L402).
- В сцене «рука на столе» — `arm_table_gz.launch.py` (вакуум, кнопка, `scene_markers`, весь
  конвейер pick & place; `grasp_analyzer` там не запускается).
- Подробности о камере, мосте и плагине вакуума — в [CAMERA_VACUUM_SIM.md](CAMERA_VACUUM_SIM.md).

---

## 2. Дерево пакета

```
src/xarm_grasping/
├── CMakeLists.txt                  установка launch/config, скриптов в lib/, python-модуля
├── package.xml                     описание пакета и зависимости
├── config/
│   ├── grasp_params.yaml           YAML-параметры grasp_analyzer (для его собственного launch)
│   ├── objects.yaml                правила классов предметов по размерам (object_perception)
│   ├── object_catalog.yaml         названия предметов по-русски/по-английски (pick_by_name)
│   ├── place_zones.yaml            зоны постановки на столе (scene_manager)
│   └── openvino_detector.yaml      параметры нейросети (openvino_detector)
├── launch/
│   └── grasp_analyzer.launch.py    запуск одного grasp_analyzer с params_file
├── scripts/                        исполняемые ноды (ставятся в lib/xarm_grasping), см. таблицу в разделе 1
│   ├── grasp_analyzer.py           облако точек → стол/объект → варианты захвата, маркеры, best_grasp_pose
│   ├── vacuum_gripper_sim.py       «вакуум» в симуляции: проверка касания + attach/detach в gz
│   ├── vacuum_gripper_xarm.py      вакуум реальной руки (переходник к драйверу xArm)
│   ├── vacuum_rviz_button.py       кнопка вакуума в RViz
│   ├── arm_parking.py              парковка руки при езде тележки
│   ├── arm_goto_tcp.py             утилита: IK + траектория, поставить TCP в точку без MoveIt
│   ├── scene_markers.py            истинная сцена Gazebo в RViz
│   ├── object_perception.py        /objects, /detect_objects
│   ├── openvino_detector.py        /detections (нейросеть)
│   ├── scene_manager.py            Planning Scene, /find_place_pose
│   ├── pick_place_server.py        /pick_place
│   ├── pick_by_name.py             /pick_command («возьми куб 2»)
│   └── survey_objects.py           /survey_objects (осмотр с разных сторон)
├── training/                       обучение детектора: train_ssdlite, export_openvino, evaluate/analyze_detector
└── xarm_grasping/                  Python-модуль (импортируется как `xarm_grasping.*`)
    ├── __init__.py                 пустой маркер пакета
    ├── grasp_generator.py          чистый генератор захватов цилиндра (TOP/SIDE/RIM), без ROS
    ├── perception.py               чистый numpy: стол, кластеры, форма, класс
    └── detector.py                 распознавание на OpenVINO без ROS
```

Сообщения и action (`DetectedObject`, `PickPlace`, `DetectObjects`, `FindPlacePose`) —
в отдельном пакете `src/xarm_grasping_msgs`.

---

## 3. Сборка: `CMakeLists.txt` и `package.xml`

Пакет имеет тип `ament_cmake`, хотя код целиком на Python. Важные места
[`CMakeLists.txt`](../src/xarm_grasping/CMakeLists.txt):

- [Строки 13–20](../src/xarm_grasping/CMakeLists.txt#L13-L20): каталоги `launch`, `config`, `worlds`
  ставятся в `share/xarm_grasping/` **только если они существуют и не пусты** (каталога `worlds/`
  сейчас нет — это нормально).
- [Строки 24–27](../src/xarm_grasping/CMakeLists.txt#L24-L27): **все** `scripts/*.py` собираются glob'ом
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
- [Строка 32](../src/xarm_grasping/CMakeLists.txt#L32): `ament_python_install_package` ставит модуль
  `xarm_grasping/`, поэтому в скриптах работает `from xarm_grasping.grasp_generator import ...`
  (после `source install/setup.bash`).

[`package.xml`](../src/xarm_grasping/package.xml) перечисляет `rclpy`, `sensor_msgs`, `geometry_msgs`,
`visualization_msgs`, `tf2_ros`, `cv_bridge`, `python3-numpy` и др. Там же
([строки 21–37](../src/xarm_grasping/package.xml#L21-L37)) указаны зависимости, которые
импортируют скрипты вакуума, парковки, pick & place и `arm_goto_tcp.py`: `interactive_markers`,
`std_srvs`, `controller_manager_msgs`, `tf2_msgs`, `xarm_grasping_msgs`, `moveit_msgs`,
`control_msgs`, `trajectory_msgs`, `python3-scipy`, `vision_msgs` и др. Поэтому `rosdep install`
поставит всё нужное сам. Исключения: OpenVINO для `openvino_detector.py` ставится в отдельный
venv `~/.venvs/openvino` ([RECOGNITION_OPENVINO.md](RECOGNITION_OPENVINO.md)), а `xarm_msgs`
(нужен `vacuum_gripper_xarm.py`) в `package.xml` не указан — он приходит из `xarm_ros2` в этом же
рабочем пространстве.

---

## 4. `scripts/grasp_analyzer.py` — анализ сцены и варианты захвата

Файл: [`scripts/grasp_analyzer.py`](../src/xarm_grasping/scripts/grasp_analyzer.py) (445 строк).
Работает только в сцене «рука на тележке» (`cart_xarm6_gz`, при `manipulation_nodes:=true`).
В сцене «рука на столе» вместо него — `object_perception` (глава 10).

### 4.1 Интерфейс

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| подписка | `color_topic` (по умолч. `/camera/color/image_raw`) | `sensor_msgs/Image` | цветная картинка — только для пересылки на дисплей |
| подписка | `points_topic` (по умолч. `/camera/depth/color/points`) | `sensor_msgs/PointCloud2` | облако точек для анализа |
| публикация | `/xarm_grasping/grasp_markers` | `visualization_msgs/MarkerArray` (TRANSIENT_LOCAL) | цилиндр объекта, точки/стрелки/подписи вариантов |
| публикация | `/xarm_grasping/grasp_candidates` | `std_msgs/String` | JSON со всеми вариантами и отладкой (или причиной отказа `rejected`) |
| публикация | `/xarm_grasping/best_grasp_pose` | `geometry_msgs/PoseStamped` | лучшая поза TCP во фрейме `base_frame` |
| публикация | `/camera/image_display` | `sensor_msgs/Image` | копия цветной картинки (через `cv_bridge`) |
| TF | `base_frame` ← фрейм облака | lookup | перевод **всего облака** в `link_base` |

QoS ([строки 120–134](../src/xarm_grasping/scripts/grasp_analyzer.py#L120-L134)): картинка —
`BEST_EFFORT, depth=2`; облако — **`RELIABLE, depth=1`**. Облако 1280×800 весит ~32 МБ, и при
`BEST_EFFORT` потеря одного UDP-кусочка выбрасывала всё сообщение — до ноды не доходило ни
одного облака. Публикаторы — [строки 137–142](../src/xarm_grasping/scripts/grasp_analyzer.py#L137-L142).

### 4.2 Параметры

Объявлены в [строках 69–92 и 115–116](../src/xarm_grasping/scripts/grasp_analyzer.py#L69-L116).

| Параметр | Дефолт в коде | В главном launch | Смысл |
|---|---|---|---|
| `color_topic` | `/camera/color/image_raw` | `/gripper_camera/color/image_raw` (Orbbec) | топик картинки |
| `points_topic` | `/camera/depth/color/points` | `/gripper_camera/points` (Orbbec) / `/camera/depth/points` (d435i) | облако точек |
| `camera_frame` | `camera_color_optical_frame` | `gripper_camera_color_optical_frame` (Orbbec) | пишется только в JSON отказа; для TF берётся `frame_id` облака |
| `base_frame` | `link_base` | `link_base` | фрейм, в котором считаются захваты |
| `table_height` | `1.0` | `0.015` | ожидаемая высота столешницы в `base_frame`, м (около неё ищется стол) |
| `block_radius` | `0.04` | — | радиус цилиндра (априори), м |
| `block_height` | `0.09` | — | высота цилиндра, м |
| `block_x`, `block_y` | `0.35`, `-0.75` | `1.14`, `0.135` | запасная поза объекта в `base_frame` (только при `use_prior_pose`) |
| `suction_radius` | `0.02` | — | радиус пятна присоски, м |
| `approach_offset` | `0.10` | — | отход pre-grasp вдоль оси подхода, м |
| `analyze_period` | `2.0` | `2.0` | период анализа, с |
| `voxel_size` | `0.01` | — | размер вокселя (и ячейки сетки кластеров), м |
| `min_points` | `150` | — | минимум точек в облаке и в окне стола |
| `max_range` | `1.2` | — | точки дальше этого от камеры отбрасываются (пол, стены), м |
| `max_object_radius` | `0.15` | — | «объект» шире — отказ (это не предмет на столе), м |
| `table_tolerance` | `0.01` | — | минимальный порог «над столом», м |
| `table_search` | `0.05` | — | окно поиска стола: `table_height ± table_search`, м |
| `object_max_height` | `0.30` | — | выше стола на столько — уже не объект, м |
| `min_object_points` | `20` | — | минимум вокселей в кластере |
| `max_cloud_age` | `3.0` | — | облако старше (с) не используется |
| `use_prior_pose` | `false` | — | `true` — если объект не найден, по-старому брать `block_x/y` |
| `use_sim_time` | (объявляет сам rclpy) | `true` | время из `/clock` |

Важная деталь из комментария на [строках 67–68](../src/xarm_grasping/scripts/grasp_analyzer.py#L67-L68):
`use_sim_time` объявлять повторно нельзя — rclpy уже объявил его, будет `ParameterAlreadyDeclaredException`.

### 4.3 Алгоритм (метод `analyze_scene`, раз в `analyze_period` секунд)

Код: [строки 186–339](../src/xarm_grasping/scripts/grasp_analyzer.py#L186-L339).

1. **Приём облака** ([строки 46–59](../src/xarm_grasping/scripts/grasp_analyzer.py#L46-L59),
   [165–169](../src/xarm_grasping/scripts/grasp_analyzer.py#L165-L169)). Функция
   `points_from_msg` вручную достаёт `x, y, z` из бинарного буфера по смещениям полей и
   выбрасывает `NaN/inf`. Колбэк только запоминает последнее облако, его `frame_id` и время.
2. **Защита от мусора**: облако старше `max_cloud_age` — отказ; точки дальше `max_range` от
   камеры отбрасываются.
3. **Воксельная сетка** — по одной точке на куб `voxel_size`. Меньше `min_points` — отказ
   («камера не смотрит на стол»).
4. **Перевод облака в `link_base` через TF** (`base_frame` ← `frame_id` облака). Нет TF — отказ.
   Теперь всё дальше считается в координатах руки, поэтому **наклон камеры не мешает**
   (раньше стол искался поперёк оптической оси, и при наклоне его край попадал в «объект»).
5. **Стол** — плоскость `z = a·x + b·y + c` по точкам в окне `table_height ± table_search`,
   4 итерации с отбросом выбросов. Порог «над столом» = `max(table_tolerance, 3σ)` шума.
6. **Кластеры предметов** на сетке XY: ячейка — «предмет», если **медиана** высоты её точек
   выше порога (отдельные шумные точки столешницы так не проходят). Целью выбирается кластер,
   ближайший к точке, куда смотрит центр кадра.
7. **Проверки**: объект не найден — отказ (или `block_x/y` при `use_prior_pose:=true`);
   радиус кластера больше `max_object_radius` — отказ.
8. **Генерация вариантов**: `generate_cylinder_grasps(...)` из `grasp_generator.py` с
   `block_radius`, `block_height`, `table_height` из параметров (размеры — **не из облака**).
9. **Публикация**: JSON, маркеры, `PoseStamped` первого (лучшего) варианта; RPY → кватернион в `_rpy_to_quat`.

**Отказ** (`_reject`, [строки 171–183](../src/xarm_grasping/scripts/grasp_analyzer.py#L171-L183)):
предупреждение в лог (не чаще раза в 10 с), JSON с полем `rejected` и пустым списком захватов,
маркеры стираются, `best_grasp_pose` **не публикуется**.

**Маркеры** (`publish_markers`, [строки 386–424](../src/xarm_grasping/scripts/grasp_analyzer.py#L386-L424)):
полупрозрачный оранжевый цилиндр объекта (ns `object`), сферы-точки касания (`grasps`), стрелки
pre-grasp→grasp (`arrows`), подписи `#i TYPE s=score` (`labels`). Цвета: TOP зелёный, SIDE синий,
RIM жёлтый, лучший — ярко-зелёный.

> 💡 В `_marker` ([строки 364–385](../src/xarm_grasping/scripts/grasp_analyzer.py#L364-L385)) все
> числа явно приводятся к `float`, а `lifetime` — через `.to_msg()`. Если передать `int`/numpy-тип
> или `rclpy.duration.Duration`, rosidl делает `assert` и **весь процесс падает** (abort).

> ⚠️ **Обрати внимание (известные ограничения):**
> - Пайплайн **предполагает цилиндр**. Реальный объект в сцене тележки — коробка `pick_box`
>   0.10×0.05×0.05 м, но варианты строятся для цилиндра `block_radius`/`block_height`.
> - `table_height=0.015` и `block_x/y=1.14, 0.135` в главном launch
>   ([строки 373–402](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L373-L402))
>   посчитаны для c2l-v3 со спавном (−0.3, 0) по измеренной позе `link_base` в мире ≈
>   (−0.54, −0.135, 0.910): 0.925 − 0.910 = 0.015, 0.60 − (−0.54) = 1.14, 0 − (−0.135) = 0.135.
>   Сейчас в launch `spawn_x` по умолчанию **−0.5**, а не −0.3 — по высоте стола это не влияет
>   (`spawn_z` тот же), а `block_x` тогда на 0.2 м меньше настоящего (используется только при
>   `use_prior_pose:=true`). Другая тележка или спавн — пересчитать вручную.
> - `pick_box` от основания руки c2l-v3 ≈ 1.14 м, а вылет xArm6 ≈ 0.7 м — **взять коробку на
>   тележке рука сейчас не может**, раскладка сцены ещё не исправлена.

---

## 5. `xarm_grasping/grasp_generator.py` — генератор захватов (без ROS)

Файл: [`xarm_grasping/grasp_generator.py`](../src/xarm_grasping/xarm_grasping/grasp_generator.py).
Только numpy/scipy, можно запускать без ROS:

```bash
cd ~/ros2k_ws/src/xarm_grasping && python3 -m xarm_grasping.grasp_generator
```

Функции:

| Функция | Строки | Что делает |
|---|---|---|
| `rotation_z(angle)` | [31–35](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L31-L35) | матрица поворота вокруг Z (сейчас не используется) |
| `grasp_pose(position, approach_dir)` | [38–53](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L38-L53) | 4×4 поза TCP, ось Z инструмента смотрит в объект (`-approach`) |
| `matrix_to_xyzrpy(T)` | [56–61](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L56-L61) | 4×4 → `[x,y,z,roll,pitch,yaw]` |
| `generate_cylinder_grasps(...)` | [64–140](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L64-L140) | список вариантов, отсортированный по `score` |
| `pick_best(grasps, ...)` | [143–155](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L143-L155) | выбрать лучший (с простейшей «видимостью»); в ноде не используется |

Сигнатура ([строки 64–66](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L64-L66)):

```python
def generate_cylinder_grasps(center_xy=(0.35, -0.75), radius=0.04, height=0.09,
                             table_z=1.0, suction_radius=0.02,
                             approach_offset=0.10, n_side=4, n_rim=6):
```

Три типа вариантов (цилиндр стоит основанием на `table_z`):

| Тип | Строки | Точка контакта | Ориентация TCP | `score` |
|---|---|---|---|---|
| `TOP` (1 шт.) | [78–98](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L78-L98) | центр крышки, z = `table_z + H` | `rpy = (π, 0, 0)` — присоской вниз | `0.9·min(1, r²/R²) + 0.1` |
| `SIDE` (`n_side`=4) | [100–118](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L100-L118) | на стенке, середина высоты, азимуты 0/90/180/270° | `rpy = (π/2, 0, az)` | `0.55·max(0, 1 − r/R) + 0.15` |
| `RIM` (`n_rim`=6) | [120–137](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L120-L137) | на крышке у края, на расстоянии `R − r` от центра | `rpy = (π, 0, 0)` | `0.5·(R−r)/R + 0.2` |

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

Файл: [`scripts/vacuum_gripper_sim.py`](../src/xarm_grasping/scripts/vacuum_gripper_sim.py) (460 строк).
Здесь — краткая справка; подробный разбор с кодом, кнопкой RViz и светофором готовности —
[глава 9](09-вакуум-кнопка-готовность.md).

В Gazebo нет физики присоски. Вместо неё в модель робота (`quasi_cart_description/urdf/cart-arm.xacro`)
добавлено **по одному** плагину `gz::sim::systems::DetachableJoint` на каждый объект из
xacro-аргумента `vacuum_targets`: по сообщению на `/vacuum/<объект>/attach` плагин создаёт
**жёсткое (fixed) соединение** `link6 ⇄ <объект>`, по `/vacuum/<объект>/detach` — удаляет его. Эта нода
решает, *когда* и *какой* объект можно приварить, и даёт удобный ROS-интерфейс.

> ⚠️ **Обрати внимание:** вакуум = **логическая сварка** DetachableJoint, а не силы присасывания.
> Предмет «прилипает» мгновенно и держится при любом ускорении; вес и герметичность не моделируются.

Для **реальной руки** есть узел `vacuum_gripper_xarm.py` с тем же интерфейсом (`set`, `cmd`,
`state`, `held`, `status`; без `ready`/`contact`/`markers`) — см. [главу 9, 9.3а](09-вакуум-кнопка-готовность.md).

### 6.1 Интерфейс ([строки 157–185](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L157-L185))

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| **сервис** | `/vacuum_gripper/set` | `std_srvs/srv/SetBool` | `data: true` — включить (с проверкой касания), `false` — выключить |
| подписка | `/vacuum_gripper/cmd` | `std_msgs/Bool` | то же без ответа (скрипты, `pick_place_server`) |
| публикация | `/vacuum_gripper/state` | `std_msgs/Bool` (TRANSIENT_LOCAL) | держит ли объект (по подтверждению плагина) — **единый сигнал «предмет взят»** |
| публикация | `/vacuum_gripper/held` | `std_msgs/String` (TRANSIENT_LOCAL) | имя удерживаемого объекта или `''` |
| публикация | `/vacuum_gripper/ready`, `/vacuum_gripper/contact` | `Bool`, `String` (JSON), 10 Гц | можно ли брать прямо сейчас; расстояние, угол, уровень |
| публикация | `/vacuum_gripper/markers` | `MarkerArray` | подсветка цели в RViz |
| публикация | `/vacuum_gripper/status` | `std_msgs/String`, 2 Гц | текст: ближайший объект, расстояние, угол, вкл/выкл, держит |
| публикация | `/vacuum/<объект>/attach`, `/detach` | `std_msgs/Empty` | команды плагину (через ros_gz_bridge) |
| подписка | `/vacuum/<объект>/state` | `std_msgs/String` | ответ плагина `attached`/`detached` |
| подписка | `/gz/robot_pose`, `/gz/object_pose` | `tf2_msgs/TFMessage` | мировые позы моделей из gz (10 Гц) |
| TF | `root_frame` → `tcp_frame` | lookup | поза кончика присоски относительно корня робота |

### 6.2 Параметры ([строки 119–129](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L119-L129))

| Параметр | Дефолт | Смысл |
|---|---|---|
| `robot_model_name` | `cart_xarm6` | имя модели робота в gz (`child_frame_id` в `/gz/robot_pose`); на столе — `xarm6_table` |
| `root_frame` | `base_footprint` | корневой фрейм робота (корень SDF); на столе — `world` |
| `tcp_frame` | `link_tcp` | кончик присоски |
| `object_names` | `['pick_box']` | имена моделей, которые можно брать |
| `object_sizes` | `[0.10, 0.05, 0.05]` | по 3 числа X Y Z на объект подряд, м (цилиндр — 2r × 2r × h) |
| `object_shapes` | `['']` | `box`/`cylinder` — только для подсветки в RViz |
| `near_distance` | `0.05` | с какого расстояния уровень `near` («жёлтый»), м |
| `contact_distance` | `0.015` | макс. расстояние TCP до поверхности, м |
| `max_angle_deg` | `20.0` | макс. угол между осью инструмента и нормалью грани, ° |
| `require_contact` | `true` | `false` — attach без проверки (отладка) |
| `attach_confirm_timeout` | `1.0` | сколько ждать подтверждения от плагина, с |

Главный launch тележки передаёт `use_sim_time`, `robot_model_name`, `object_names: ['pick_box']`,
`object_sizes`; `arm_table_gz` — списки всех 7 предметов из `config/arm_table_objects.yaml`.
`contact_distance`, `max_angle_deg`, `near_distance`, `require_contact`, `attach_confirm_timeout`
читаются **при каждом использовании**, поэтому их можно менять «на лету» через `ros2 param set`.
Списки объектов читаются один раз в конструкторе.

### 6.3 Алгоритм

1. **Позы из gz** (`on_poses`, [строка 192](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L192)):
   запоминает 4×4 матрицы робота и объектов в мире по `child_frame_id`.
2. **Поза TCP в мире** (`tcp_in_world`, [строка 222](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L222)):
   `T_world←tcp = T_world←robot · T_root←tcp` (вторая часть — из TF `robot_state_publisher`).
3. **Метрики касания** (`contact_metrics`, [строка 234](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L234)):
   точка TCP переводится в систему объекта; расстояние до бокса = длина «вылета» за полуразмеры
   (0, если внутри). Ближайшая грань — ось с наибольшим вылетом; угол — между +Z TCP и
   направлением *внутрь* этой грани. `nearest` ([строка 263](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L263))
   выбирает ближайший из всех объектов.
4. **Включение** (`vacuum_on`, [строки 278–310](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L278-L310)):
   - уже держит → `success=true, "уже держит ..."`;
   - иначе (если `require_contact`) проверяет `dist ≤ contact_distance` и `угол ≤ max_angle_deg`;
     при отказе — `success=false` с причиной в `message` (мм и градусы);
   - шлёт attach и ждёт до `attach_confirm_timeout` подтверждения `/vacuum/<объект>/state`.
   Выключение (`vacuum_off`) шлёт detach всем объектам.
5. **Стартовое отпускание** (`on_status_timer`, [строка 335](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L335)):
   плагин DetachableJoint **сам приваривает объект при загрузке**, а его сообщение об этом часто
   теряется. Поэтому после появления позы робота нода 6 раз подряд (таймер 10 Гц, то есть около 0.6 с) шлёт
   detach всем объектам, которые держать не велели.
6. **Лог без падений** (`_log`, [строка 318](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L318)):
   успех пишется `info`, отказ — `warning`, **из разных строк кода**. Раньше уровень менялся в одной
   строке, и rclpy после первого отказа ронял сервис (исправлено 2026-11-09).
7. **Многопоточность** ([строки 168–185, 437–457](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L437-L457)):
   сервис *ждёт* сообщения из подписки, поэтому всё в `ReentrantCallbackGroup` и
   `MultiThreadedExecutor(num_threads=4)`. С обычным `rclpy.spin` сервис бы заблокировал сам себя
   и всегда отвечал «плагин не подтвердил». Исключения в колбэках пишутся в лог, нода продолжает работу.

---

## 7. `scripts/arm_goto_tcp.py` — подвести присоску к точке без MoveIt

Файл: [`scripts/arm_goto_tcp.py`](../src/xarm_grasping/scripts/arm_goto_tcp.py) (213 строк).
**Отладочная утилита, не планировщик**: столкновения не проверяются, траектория — прямая
интерполяция в суставах.

### 7.1 Интерфейс

| Направление | Имя | Тип | Назначение |
|---|---|---|---|
| подписка | `/robot_description` | `std_msgs/String` (TRANSIENT_LOCAL) | URDF — из него строится цепочка `link_base → link_tcp` |
| подписка | `/joint_states` | `sensor_msgs/JointState` | текущие углы `joint1..joint6` (старт IK) |
| action-клиент | `/xarm6_traj_controller/follow_joint_trajectory` | `control_msgs/action/FollowJointTrajectory` | исполнение траектории |

### 7.2 Аргументы командной строки ([строки 151–158](../src/xarm_grasping/scripts/arm_goto_tcp.py#L151-L158))

Это не ROS-параметры, а обычные `argparse`-флаги (лишние аргументы, например `--ros-args`, уходят в rclpy).

| Флаг | Дефолт | Смысл |
|---|---|---|
| `--x`, `--y`, `--z` | обязательны | цель TCP в `link_base`, м |
| `--lift` | `0.10` | сначала точка на столько выше цели (0 — сразу в цель) |
| `--time` | `6.0` | время на каждый участок, с |
| `--dry-run` | выкл | только посчитать IK, не двигать |

### 7.3 Алгоритм

1. Ждёт `/robot_description` и `/joint_states` ([строки 162–164](../src/xarm_grasping/scripts/arm_goto_tcp.py#L162-L164)).
2. `Chain` ([58–89](../src/xarm_grasping/scripts/arm_goto_tcp.py#L58-L89)) разбирает URDF, идёт от
   `link_tcp` к `link_base` по родителям, запоминает пределы суставов; `fk(q)` — прямая кинематика.
3. `solve_ik` ([92–124](../src/xarm_grasping/scripts/arm_goto_tcp.py#L92-L124)) — `scipy.optimize.least_squares`
   с двумя целями: позиция (вес 10) и ось инструмента строго вниз (вес 3); yaw свободен.
   Пробует 5 стартов (текущая поза и повороты `joint1` на ±0.5, ±1 рад), затем «сворачивает» углы
   на ±2π к текущим, чтобы рука не крутилась лишний оборот.
4. Проверка ([строка 178](../src/xarm_grasping/scripts/arm_goto_tcp.py#L178)): ошибка позиции
   > 5 мм или наклон > 10° → `IK не сошлась` и выход с кодом 1.
5. Отправляет одну цель с 1–2 точками (подъём + цель), ждёт результат; `error_code=0` — успех.

Полезные числа из docstring ([строки 21–26](../src/xarm_grasping/scripts/arm_goto_tcp.py#L21-L26)) для
`cart_table_offline.world` при spawn 0,0: `link_base` в мире = (−0.0075, 0, 0.919); верх стола в
`link_base` z = +0.006; `pick_box` в центре (0.6075, 0, 0.031), его верх z = +0.056.
Эти числа посчитаны для руки в центре крыши (как у `c2`). Для дефолтной тележки
`c2l-v3` `link_base` стоит у края, и цифры другие — см. параметры `grasp_analyzer` в
главном launch (`table_height=0.015`, `pick_box` в `link_base` ≈ (1.14, 0.135) — дальше, чем
достаёт рука). Для опытов с `arm_goto_tcp.py` удобнее сцена «рука на столе»: там `world` =
`link_base`, верх стола z = 0, например куб `cube` — центр (0.50, 0.00), верх z = 0.06
(все предметы — в [CAMERA_VACUUM_SIM.md](CAMERA_VACUUM_SIM.md), раздел 4).

---

## 8. `launch/grasp_analyzer.launch.py` и `config/grasp_params.yaml`

[`grasp_analyzer.launch.py`](../src/xarm_grasping/launch/grasp_analyzer.launch.py) запускает **только**
`grasp_analyzer` с двумя аргументами:

| Аргумент | Дефолт | Строка |
|---|---|---|
| `use_sim_time` | `true` | [24](../src/xarm_grasping/launch/grasp_analyzer.launch.py#L24) |
| `params_file` | `share/xarm_grasping/config/grasp_params.yaml` | [26](../src/xarm_grasping/launch/grasp_analyzer.launch.py#L26) |

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

[`grasp_params.yaml`](../src/xarm_grasping/config/grasp_params.yaml): верхний ключ `grasp_analyzer:` должен
совпадать с `name` ноды, имена параметров — с `declare_parameter()`.

YAML написан под сцену «тележка `c2l-v3`, спавн (−0.3, 0)» (сейчас в launch `spawn_x` по умолчанию −0.5, см. 4.3), камера
Orbbec Gemini 335Le, мир `cart_table_offline.world`): топики `/gripper_camera/color/image_raw`
и `/gripper_camera/points`, фрейм `gripper_camera_color_optical_frame`, `table_height: 0.015`,
`block_x: 1.14`, `block_y: 0.135` — те же числа, что в главном launch.

> ⚠️ **Обрати внимание:** главный launch `cart_xarm6_gz.launch.py` этот YAML **не читает** —
> он задаёт параметры сам. YAML нужен для ручного запуска `grasp_analyzer.launch.py` рядом с
> работающей симуляцией. Меняешь числа — меняй в обоих местах.

---

## 9. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| задать высоту стола для grasp_analyzer | [cart_xarm6_gz.launch.py:400](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L400) / [grasp_analyzer.py:73](../src/xarm_grasping/scripts/grasp_analyzer.py#L73) | `table_height` | в launch: `{'table_height': 0.015}`; CLI: `ros2 run xarm_grasping grasp_analyzer.py --ros-args -p use_sim_time:=true -p table_height:=0.015` |
| размеры объекта для генератора | [grasp_analyzer.py:74–75](../src/xarm_grasping/scripts/grasp_analyzer.py#L74-L75) | `block_radius`, `block_height` | `-p block_radius:=0.025 -p block_height:=0.05` |
| запасную позицию объекта | [cart_xarm6_gz.launch.py:401–402](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L401-L402) / [grasp_analyzer.py:115–116](../src/xarm_grasping/scripts/grasp_analyzer.py#L115-L116) | `block_x`, `block_y`, `use_prior_pose` | `-p use_prior_pose:=true -p block_x:=1.14 -p block_y:=0.135` |
| отсечь мусор дальше/ближе | [grasp_analyzer.py:85–91](../src/xarm_grasping/scripts/grasp_analyzer.py#L85-L91) | `max_range`, `max_object_radius`, `object_max_height`, `table_search` | `-p max_range:=1.5` |
| другие топики камеры | [grasp_analyzer.py:69–71](../src/xarm_grasping/scripts/grasp_analyzer.py#L69-L71) | `color_topic`, `points_topic`, `camera_frame` | `-p points_topic:=/gripper_camera/points -p color_topic:=/gripper_camera/color/image_raw` |
| анализ чаще/реже | [grasp_analyzer.py:78](../src/xarm_grasping/scripts/grasp_analyzer.py#L78) | `analyze_period` | `-p analyze_period:=0.5` (читается один раз при старте) |
| чувствительность сегментации | [grasp_analyzer.py:79–90](../src/xarm_grasping/scripts/grasp_analyzer.py#L79-L90) | `voxel_size`, `min_points`, `table_tolerance`, `min_object_points` | `-p voxel_size:=0.005 -p min_points:=80` |
| длину подхода pre-grasp | [grasp_analyzer.py:77](../src/xarm_grasping/scripts/grasp_analyzer.py#L77) | `approach_offset` | `-p approach_offset:=0.15` |
| чтобы TOP был лучшим | [grasp_generator.py:96, 116, 135](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L96) | формулы `score` | поднять вес TOP или снизить RIM |
| число боковых/ободковых вариантов | [grasp_generator.py:66](../src/xarm_grasping/xarm_grasping/grasp_generator.py#L66) | `n_side`, `n_rim` | передать в вызове на [grasp_analyzer.py:316](../src/xarm_grasping/scripts/grasp_analyzer.py#L316) |
| смягчить проверку касания вакуума | [vacuum_gripper_sim.py:126–127](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L126-L127) | `contact_distance`, `max_angle_deg` | на лету: `ros2 param set /vacuum_gripper_sim contact_distance 0.03` |
| attach без проверки (отладка) | [vacuum_gripper_sim.py:128](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L128) | `require_contact` | `ros2 param set /vacuum_gripper_sim require_contact false` |
| другой объект/размер | [cart_xarm6_gz.launch.py:337–340](../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L337-L340); для стола — `config/arm_table_objects.yaml` | `object_names`, `object_sizes` | `'object_sizes': [0.06, 0.06, 0.12]` (+ имя в `vacuum_targets` и мост `/vacuum/<имя>/*`) |
| дольше ждать подтверждения плагина | [vacuum_gripper_sim.py:129](../src/xarm_grasping/scripts/vacuum_gripper_sim.py#L129) | `attach_confirm_timeout` | `ros2 param set /vacuum_gripper_sim attach_confirm_timeout 3.0` |
| допуски IK в arm_goto_tcp | [arm_goto_tcp.py:178](../src/xarm_grasping/scripts/arm_goto_tcp.py#L178) | `err > 0.005 or tilt > 10` в коде | правка кода |
| медленнее/быстрее двигать руку | [arm_goto_tcp.py:156](../src/xarm_grasping/scripts/arm_goto_tcp.py#L156) | `--time` | `--time 10` |
| запустить отдельный анализатор со своим YAML | [grasp_analyzer.launch.py:26](../src/xarm_grasping/launch/grasp_analyzer.launch.py#L26) | `params_file` | `ros2 launch xarm_grasping grasp_analyzer.launch.py params_file:=/path/my.yaml` |

Как передать параметр **через launch** — добавьте словарь в список `parameters` ноды:

```python
grasp_analyzer = Node(
    package='xarm_grasping', executable='grasp_analyzer.py', name='grasp_analyzer',
    parameters=[{'use_sim_time': True},
                {'table_height': 0.015, 'block_x': 1.14, 'block_y': 0.135}],
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
| `Захват не построен: ...` в логе, маркеры пропали | объект не найден или отвергнут (нет TF, стол не около `table_height`, «объект» шире `max_object_radius`, облако устарело) | причина — в тексте и в поле `rejected` JSON `/xarm_grasping/grasp_candidates`; камеру навести на стол |
| Захваты всегда у `(1.140, 0.135)` | включён `use_prior_pose:=true`, а объект не найден | см. строку выше, проверить TF |
| Маркеры висят над/под столом | `table_height` не соответствует сцене (другая тележка/спавн или ручной запуск с дефолтом 1.0) | пересчитать и передать `table_height` |
| `ParameterAlreadyDeclaredException: use_sim_time` | добавили `declare_parameter('use_sim_time')` | не объявлять, rclpy делает это сам |
| Процесс падает с assert в rosidl при публикации маркера | в поле сообщения передали `int`/numpy/`rclpy.duration.Duration` | приводить к `float`, `Duration(...).to_msg()` |
| `/vacuum_gripper/set` → `отказ: нет касания — расстояние ... мм` | TCP дальше 15 мм или угол > 20° | подвести `arm_goto_tcp.py`, смотреть `/vacuum_gripper/status` |
| `отказ: нет позы робота из gz` | мост `/gz/robot_pose` не работает | `ros2 topic hz /gz/robot_pose`, проверить `gz_bridge.yaml` |
| `attach отправлен, но плагин не подтвердил` | нет модели в мире, она `static`, или её имени нет в `vacuum_targets` | `ros2 topic echo /vacuum/<имя>/state --once`, см. [STATUS.md](STATUS.md) |
| Коробка «едет» с рукой сразу после старта | плагин приварил при загрузке, а `vacuum_gripper_sim` не запущен | запустить ноду — она отпустит автоматически; без ноды — `vacuum_targets:=none` |
| Сервис вакуума ответил отказом один раз, дальше молчит | старая версия ноды (уровень лога менялся в одной строке) | исправлено 2026-11-09: обновить код, пересобрать |
| `IK не сошлась (точка вне досягаемости?)` | цель вне рабочей зоны или нельзя смотреть строго вниз | `--dry-run`, проверить координаты в `link_base` |
| `нет action-сервера xarm6_traj_controller` | контроллер не заспавнен | `ros2 control list_controllers` |

---

## 11. Проверь себя

Для `grasp_analyzer` предполагается, что запущен `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py`,
для вакуума — `ros2 launch cart_xarm_bringup arm_table_gz.launch.py` (на тележке c2l-v3 рука до
коробки не достаёт). Во всех терминалах выполнено `source ~/ros2k_ws/install/setup.bash`.

**Генератор без ROS:**

```bash
cd ~/ros2k_ws/src/xarm_grasping && python3 -m xarm_grasping.grasp_generator | tail -1
# BEST: RIM 0.45
```

**grasp_analyzer:**

```bash
ros2 node info /grasp_analyzer
ros2 param get /grasp_analyzer table_height                 # 0.015 — из главного launch
ros2 topic echo /xarm_grasping/best_grasp_pose --once       # geometry_msgs/msg/PoseStamped, frame_id: link_base
ros2 topic echo /xarm_grasping/grasp_candidates --once --full-length   # JSON
ros2 topic hz /xarm_grasping/best_grasp_pose                # ~0.5 Гц при analyze_period=2.0
```

В RViz: *Add → By topic → `/xarm_grasping/grasp_markers` → MarkerArray*.

Отдельный запуск со своими параметрами (камера Orbbec):

```bash
ros2 run xarm_grasping grasp_analyzer.py --ros-args -r __node:=grasp_test \
  -p use_sim_time:=true -p points_topic:=/gripper_camera/points \
  -p color_topic:=/gripper_camera/color/image_raw -p table_height:=0.015
```

**Вакуум** (сцена «рука на столе», куб `cube` в (0.50, 0.00), верх z = 0.06):

```bash
ros2 topic echo /vacuum_gripper/state --once         # data: false
ros2 topic echo /vacuum_gripper/status --once        # ближайший объект, расстояние/угол/держит
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
# далеко от предметов → success: false, message: 'отказ: нет касания — до ... мм ...'

ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.0 --z 0.061 --dry-run   # только IK
ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.0 --z 0.061             # присоска на верх куба
ros2 topic echo /vacuum_gripper/status --once        # < 15 мм, < 20°
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"     # success: true
ros2 topic echo /vacuum_gripper/state --once         # data: true — «предмет взят»
ros2 topic echo /vacuum_gripper/held --once          # data: cube

ros2 run xarm_grasping arm_goto_tcp.py --x 0.50 --y 0.0 --z 0.25 --lift 0     # поднять с кубом
ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: false}"    # отпустить
ros2 topic echo /vacuum_gripper/held --once          # data: ''
```

**Вопросы для самопроверки:**

1. Почему `vacuum_gripper_sim` использует `MultiThreadedExecutor`, а `grasp_analyzer` — обычный `rclpy.spin`?
2. Какое значение `table_height` получает `grasp_analyzer` в главном launch и почему?
3. Что произойдёт, если в `scripts/` добавить `my_tool.py` без `chmod +x` и сразу вызвать `ros2 run`?
4. Почему при дефолтных параметрах лучшим выходит вариант RIM?
5. Какие два условия проверяются перед attach и где их ослабить без перезапуска?
6. Какой топик одинаково говорит «предмет взят» и в симуляции, и на реальной руке? Какой узел
   публикует его на реальной руке?
