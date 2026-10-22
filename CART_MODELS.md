# Модели тележек и крепление руки xArm6

Как выбрать тележку при запуске, где задаётся точка крепления руки и что
менять, чтобы её сдвинуть или добавить новую тележку.

Основной README: [README.md](README.md) (установка, сборка, все сценарии запуска).

---

## 1. Доступные модели

Тележка выбирается launch-аргументом `robot_model`. Значение — имя файла
`src/r2_robot/quasi_cart_description/urdf/<robot_model>.urdf.xacro`.

| `robot_model` | Корпус (меш `base_link`) | Габарит корпуса в плане (X × Y) | Крепление руки (`base_link`) | Статус |
|---|---|---|---|---|
| `c2` | `base_link_c2-v3.stl` | ≈ 0.83 × 0.47 м | `-0.0075 0 0.849` — центр крыши | работает |
| `c2l-v3` *(по умолчанию)* | `base_link_c2l-v3.stl` | ≈ 1.03 × 0.62 м | `-0.25 -0.1295 0.849` | работает |

Габарит по X включает ручку. Высота у всех моделей одинаковая: крыша на
z = 0.849 м, ручка до ≈ 0.99 м.

Чем c2l отличается от c2 (по URDF):

| Параметр | c2 | c2l-v3 |
|---|---|---|
| Корпус | — | на ~100 мм длиннее с каждой стороны по X, на ~75 мм шире с каждой стороны по Y |
| Ведущие колёса, ±Y | 0.177 | 0.252 (колея ≈ 0.50 м против 0.35 м) |
| Поворотные колёса от оси ведущих, X | 0.5465 | 0.7465 |
| Масса корпуса | 22.9 кг | 35.5 кг |
| Доп. напольные ToF 27–30 | нет | есть (`enable_extra_floor_tofs`) |

> Меш `base_link_c2l-v3.stl` взят из репозитория тележки `quasi_ros_c1`
> (`quasi_cart_description/meshes`), оттуда же меши ToF 27–36. Раньше меша не
> было, и вместо него использовалась `c2l-v3-scaled` (меш c2, растянутый до
> габаритов c2l). Эта модель удалена.

---

## 2. Запуск

Перед запуском (после любых правок в `urdf/` или `launch/`):

```bash
cd ~/ros2k_ws
colcon build --packages-select quasi_cart_description cart_xarm_bringup
source install/setup.bash
```

> Файлы xacro читаются из `install/`, а не из `src/`. Без пересборки
> изменения не применятся.

### Gazebo + MoveIt (основной запуск)

```bash
# тележка c2l — по умолчанию
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py

# тележка c2
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py robot_model:=c2

# c2l без MoveIt, обычный RViz
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py robot_model:=c2l-v3 use_moveit:=false
```

### Только RViz (без Gazebo) — проверка модели и крепления

```bash
ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py robot_model:=c2l-v3
```

Так быстрее всего проверить, где стоит рука: Gazebo не нужен, суставы руки
двигаются ползунками.

### Launch-аргументы, связанные с тележкой

Действуют в обоих файлах (`cart_xarm6_gz.launch.py`, `cart_xarm6_rviz.launch.py`):

| Аргумент | По умолчанию | Назначение |
|---|---|---|
| `robot_model` | `c2l-v3` | тип тележки (таблица в разделе 1) |
| `arm_offset_xyz` | `0 0 0` | **доп.** смещение базы руки от точки крепления тележки, м |
| `arm_offset_rpy` | `0 0 0` | доп. поворот базы руки (roll pitch yaw), рад |

Только в `cart_xarm6_gz.launch.py`: `spawn_x`, `spawn_y`, `spawn_z`, `spawn_yaw` —
поза спавна тележки в мире. Стол стоит на x ≈ +1.7. У c2l рука сдвинута на
0.25 м назад, а перед тележки на 0.1 м длиннее, поэтому для работы со столом
может понадобиться другой `spawn_x`.

---

## 3. Система координат и точка крепления

Точка крепления задаётся в кадре `base_link` (REP-103):

```
                 вид сверху на тележку

          +Y (влево, если стоять у ручки лицом вперёд)
           ^
           |      * рука c2l: (-0.25, -0.1295)
   ручка   |                                       перед
   ведущие |             o base_link (0, 0)        поворотные
   колёса  |                                       колёса
           +------------------------------------------> +X (вперёд)
```

- **Начало `base_link`** — центр корпуса в плане.
- **+X** — вперёд, к поворотным колёсам; **−X** — назад, к ручке и ведущим колёсам.
- **+Y** — влево, если стоять у ручки лицом вперёд; **−Y** — вправо.
- **z = 0.849** — верхняя поверхность крыши.

Цепочка TF: `base_link → arm_mount_link → link_base` (база руки).

- `arm_mount_joint` (`base_link → arm_mount_link`) — **точка крепления тележки**
  (`arm_mount_xyz`, задаётся в файле модели);
- `world_joint` руки (`arm_mount_link → link_base`) — **доп. смещение**
  (`arm_offset_xyz` / `arm_offset_rpy`, launch-аргумент).

Итоговая позиция руки = `arm_mount_xyz` + `arm_offset_xyz`.

### Откуда взято крепление c2l

Из CAD-сборки `C2R.000.000.000`: от центра тележки до центра базы руки
ΔX = 250 мм (к ручке), ΔZ(CAD) = 129.5 мм (вбок, к левому краю), ΔY(CAD) = 855.3 мм
(высота). Оси CAD и ROS различаются: высота в CAD — это Y, вбок — Z.

Допущения, которые стоит проверить:

1. **Сторона смещения вбок.** Взято −Y: правый край, если стоять у ручки
   лицом вперёд. Если рука должна стоять у другого края, замените на `0.1295`.
2. **Высота.** Взят верх крыши по мешу, 0.849. В CAD 855.3 мм отсчитаны от
   точки внизу тележки. Если эта точка совпадает с началом `base_link`,
   поставьте `0.855`.

---

## 4. Как изменить смещение руки

### Вариант A — временно, при запуске (без правки файлов)

`arm_offset_xyz` **добавляется** к точке крепления модели:

```bash
# сдвинуть руку ещё на 5 см вперёд и на 2 см вправо
ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py \
    robot_model:=c2l-v3 arm_offset_xyz:="0.05 -0.02 0"

# повернуть базу руки на 90° вокруг вертикали
ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py \
    robot_model:=c2l-v3 arm_offset_rpy:="0 0 1.5708"
```

Удобно подбирать значение в RViz, а потом перенести его в файл модели (вариант B).

### Вариант B — постоянно, в файле модели

| Тележка | Файл | Что менять |
|---|---|---|
| `c2l-v3` | `src/r2_robot/quasi_cart_description/urdf/c2l-v3.urdf.xacro` | `<xacro:property name="arm_mount_xyz" default="-0.25 -0.1295 0.849" />` (в начале файла) |
| `c2` | `src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro` | `<xacro:property name="arm_mount_xyz" default="-0.0075 0.0 0.849"/>` — это значение по умолчанию для всех моделей, у которых нет своего |

> Почему так работает: в `c2l-v3.urdf.xacro` и `cart-arm.xacro` свойство
> объявлено с `default=`. Такое значение применяется, только если свойство ещё
> не задано. Действует первое объявление в порядке подключения:
> `c2l-v3` → `cart-arm.xacro`.

После правки пересоберите `quasi_cart_description` (раздел 2).

---

## 5. Передняя камера, лидар, ToF

Передняя камера — Orbbec Gemini 335Le (как на реальной тележке), файл
`quasi_cart_description/urdf/cart-front-camera.xacro`, тело — общий макрос
`quasi_camera_description`. Включена по умолчанию, отключается
`front_camera:=false`. В симуляции 640×400 @15 Гц.

Поза — свойства `front_camera_xyz` / `front_camera_rpy` в файле модели
(нижний винт камеры в `base_link`; камера вверх ногами, `roll = -π`,
наклон вниз 0.041 рад):

| Модель | `front_camera_xyz` | Источник |
|---|---|---|
| `c2` | `0.309 0 0.840` (default в `cart-front-camera.xacro`) | `quasi_ros_c1`, c2-v3 |
| `c2l-v3` | `0.409 0 0.840` | **оценка**: поза c2 + 0.1 м вперёд; уточнить по CAD |

Топики в ROS (мосты `gz_bridge_cart_sensors.yaml`, `gz_bridge_front_camera.yaml`;
имена — как в `quasi_ros_c1/nav2.yaml`):

| Топик | Тип | frame |
|---|---|---|
| `/scan`, `/scan/points` | LaserScan, PointCloud2 | `lidar_link` |
| `/range1_pcl` … `/range8_pcl` | PointCloud2 (ToF 18×18 лучей) | `tofN_link` |
| `/front_camera/color/image_raw`, `.../camera_info`, `/front_camera/aligned_depth/image_raw` | Image, CameraInfo | `front_camera_color_optical_frame` |
| `/front_camera/points` | PointCloud2 (depth_image_proc) | `front_camera_color_optical_frame` |

---

## 6. Как проверить, что запущена именно c2l

Если тележка кажется маленькой, сначала убедитесь, что загружена нужная модель.

```bash
# 1) меш корпуса в загруженном описании (для c2l: base_link_c2l-v3.stl)
ros2 param get /robot_state_publisher robot_description | grep -o 'base_link_c2[^"]*' | sort -u

# 2) положение базы руки относительно корпуса (для c2l: -0.25 -0.1295 0.849)
ros2 run tf2_ros tf2_echo base_link link_base

# 3) колея ведущих колёс (для c2l: y = ±0.252, для c2: ±0.177)
ros2 run tf2_ros tf2_echo drive_link drive_wheel_left_link
```

Типичные причины:

| Симптом | Причина | Что сделать |
|---|---|---|
| меш `base_link_c2-v3.stl`, рука в центре крыши | запущено `robot_model:=c2` | `robot_model:=c2l-v3` или без аргумента |
| Правки в xacro не видны | не пересобран пакет или не выполнен `source` | раздел 2 |

Для ориентира: у c2l корпус в плане ≈ 0.92 × 0.62 м без ручки (у c2 ≈ 0.72 × 0.47),
а высота у обеих одинаковая, ≈ 1 м. Рядом с xArm6 тележка может казаться
небольшой. Сравнивать удобнее по колее: ≈ 0.50 м у c2l против 0.35 м у c2.

---

## 7. Как добавить новую тележку

1. Создайте `src/r2_robot/quasi_cart_description/urdf/<имя>.urdf.xacro` по
   образцу существующих (корпус `base_link`, `drive_joint`, ToF, лидар, IMU,
   колёса, свойства `camera_offset_*`).
2. В начале файла задайте точку крепления руки:
   ```xml
   <xacro:property name="arm_mount_xyz" default="X Y Z" />
   ```
   Без этой строки рука встанет в центр крыши c2 (`-0.0075 0 0.849`).
3. Задайте позу передней камеры `front_camera_xyz` / `front_camera_rpy`
   (иначе — поза c2).
4. Пересоберите пакет и запустите с `robot_model:=<имя>`.
5. Желательно добавить имя в `description` аргумента `robot_model` в обоих
   launch-файлах (`src/r2_robot/cart_xarm_bringup/launch/`).

> Имена, начинающиеся с `c2w` и `c2h`, `cart-base.urdf.xacro` и
> `cart.urdf.xacro` обрабатывают особо (часть ToF отключается). Не давайте
> такие префиксы обычным тележкам.

---

## 8. Файлы

| Файл | Роль |
|---|---|
| `quasi_cart_description/urdf/cart_with_arm.urdf.xacro` | сборка: подключает `<robot_model>.urdf.xacro`, привод, датчики, руку |
| `quasi_cart_description/urdf/cart-arm.xacro` | рука xArm6 + присоска + D435i; `arm_mount_joint`, значение `arm_mount_xyz` по умолчанию |
| `quasi_cart_description/urdf/c2.urdf.xacro` | тележка c2 |
| `quasi_cart_description/urdf/c2l-v3.urdf.xacro` | тележка c2l: геометрия, `arm_mount_xyz`, `front_camera_*` |
| `quasi_cart_description/urdf/cart-front-camera.xacro` | передняя камера Orbbec 335Le (`front_camera:=true`) |
| `quasi_cart_description/meshes/base_link_c2-v3.stl`, `base_link_c2l-v3.stl` | меши корпусов c2, c2l |
| `cart_xarm_bringup/config/gz_bridge_cart_sensors.yaml`, `gz_bridge_front_camera.yaml` | мосты лидара, ToF, передней камеры |
| `cart_xarm_bringup/launch/cart_xarm6_gz.launch.py` | Gazebo + RViz/MoveIt; аргументы `robot_model`, `arm_offset_*`, `spawn_*` |
| `cart_xarm_bringup/launch/cart_xarm6_rviz.launch.py` | только RViz; аргументы `robot_model`, `arm_offset_*` |

Все пути относительно `src/r2_robot/`.
