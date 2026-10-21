# Глава 3. Пакет `quasi_camera_description` — камера на запястье руки

В этой главе разбираем, как в модель робота «прикручивается» RGB-D камера
**Orbbec Gemini 335Le**, как она превращается в работающий сенсор в Gazebo, какие
топики и фреймы она даёт и что осталось от старой камеры Intel RealSense **D435i**.

## 1. Зачем этот пакет и с кем он связан

**RGB-D камера** — камера, которая выдаёт одновременно цветную картинку (RGB) и
карту глубины (D, depth): для каждого пикселя — расстояние до объекта в метрах.
Из пары «цвет + глубина» строится **облако точек** (point cloud) — набор 3D-точек,
по которому `grasp_analyzer` ищет объект и точку захвата.

Камера стоит на фланце руки рядом с присоской — схема **eye-in-hand** («глаз в
руке»): камера двигается вместе с инструментом.

Пакет `quasi_camera_description` — это **description-пакет**: в нём нет
программ, только описание геометрии (xacro/URDF), меш и параметры. Термины:

- **URDF** — XML-описание робота: звенья (`<link>`) и соединения (`<joint>`).
- **xacro** — «макросы для URDF»: позволяет писать `${переменные}`, `<xacro:if>`,
  `<xacro:macro>` и собирать итоговый URDF из многих файлов.
- **frame (фрейм, система координат)** — точка + три оси (X, Y, Z). Каждое звено
  URDF — это фрейм. Фиксированный joint (`type="fixed"`) говорит «фрейм B сдвинут
  относительно A на xyz и повёрнут на rpy (roll-pitch-yaw, радианы)».
- **TF** — система ROS 2, которая знает, где все фреймы друг относительно друга
  в каждый момент времени. `robot_state_publisher` читает URDF и публикует TF.
  Благодаря TF можно спросить: «где точка из облака камеры в координатах тележки?».

Кто кого вызывает:

```text
cart_xarm6_gz.launch.py  (аргументы camera_model, gripper_camera_preview)
 └─ xacro cart_with_arm.urdf.xacro use_sim:=true camera_model:=...
     └─ cart-arm.xacro               → вызывает макрос xarm_device
         └─ xarm_device_macro.xacro  → «крючок» add_gripper_camera
             └─ quasi_camera_description/urdf/gripper_camera.xacro   (кронштейн)
                 ├─ urdf/cameras/gemini335le.xacro                  (тело + фреймы)
                 └─ urdf/gz/gz_rgbd_camera.xacro  ← config/gemini335le.yaml (сенсор gz)
Gazebo → gz_bridge_gripper_camera.yaml → /gripper_camera/... → depth_image_proc → /gripper_camera/points
```

## 2. Дерево пакета

```text
src/r2_robot/quasi_camera_description/
├── CMakeLists.txt                      # только install(urdf config meshes) в share/
├── package.xml                         # ament_cmake, exec_depend xacro
├── config/
│   └── gemini335le.yaml                # параметры сенсора в симуляции (sim / sim_preview)
├── meshes/gemini335Le/
│   ├── base_Link.STL                   # 3D-модель корпуса камеры (из OrbbecSDK_ROS2)
│   └── LICENSE.orbbec                  # лицензия меша (Apache-2.0)
└── urdf/
    ├── gripper_camera.xacro            # макрос gripper_camera: кронштейн + выбор модели камеры
    ├── cameras/gemini335le.xacro       # макрос camera_body_gemini335le: тело и оптические фреймы
    └── gz/gz_rgbd_camera.xacro         # макрос gz_rgbd_camera: сенсор rgbd_camera для Gazebo Sim
```

### package.xml и CMakeLists.txt (кратко)

- [package.xml](../../src/r2_robot/quasi_camera_description/package.xml#L15-L16) —
  сборка `ament_cmake`, зависимость только от `xacro`.
- [CMakeLists.txt](../../src/r2_robot/quasi_camera_description/CMakeLists.txt#L8-L10) —
  копирует `urdf`, `config`, `meshes` в `install/quasi_camera_description/share/...`.
  Именно оттуда их находит `$(find quasi_camera_description)`.
  **Новый файл** в этих папках появится в `install/` только после
  `colcon build --packages-select quasi_camera_description` (симлинки делаются на
  существующие файлы).

### Меши

`meshes/gemini335Le/base_Link.STL` (~3 МБ) взят из форка OrbbecSDK_ROS2 вместе с
лицензией. Используется только как `<visual>` (картинка); для коллизий — простой бокс.

## 3. Разбор файлов

### 3.1. `urdf/gripper_camera.xacro` — кронштейн и выбор камеры

Это «входная точка» пакета. Сигнатура макроса
([gripper_camera.xacro#L19-L22](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L19-L22)):

```xml
<xacro:macro name="gripper_camera"
    params="camera_model parent:=link_eef name:=gripper_camera
            mount_xyz:='0.075 0 0.030' mount_rpy:='${pi} ${-pi/2} 0'
            use_nominal_extrinsics:=true add_gz_sensor:=false preview:=false">
```

| Параметр | Дефолт | Смысл |
|---|---|---|
| `camera_model` | — (обязателен) | `gemini335le`; другие модели пока не реализованы (TODO d555, [L54](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L54)) |
| `parent` | `link_eef` | к какому звену крепим (фланец/конец руки) |
| `name` | `gripper_camera` | префикс всех фреймов и топиков |
| `mount_xyz` | `0.075 0 0.030` | где винт камеры относительно `link_eef`, м: 75 мм вбок от оси, 30 мм от фланца |
| `mount_rpy` | `π -π/2 0` | поворот камеры: её ось X (направление взгляда) смотрит вдоль Z инструмента |
| `use_nominal_extrinsics` | `true` | рисовать ли в URDF оптические фреймы (в симуляции да, на железе их даёт драйвер) |
| `add_gz_sensor` | `false` | добавлять ли сенсор Gazebo |
| `preview` | `false` | лёгкий режим 640×400 |

Фрейм `link_eef`: ось Z направлена «из фланца», вдоль присоски (TCP на z=0.126).
Комментарий в шапке ([L8-L16](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L8-L16))
объясняет: камера смотрит туда же, куда присоска, но смещена на ~89 мм от оси; сама
присоска в кадр не попадает. Значения — **стартовые**, их надо уточнить по CAD и калибровке.

Кронштейн ([L24-L45](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L24-L45))
— пока «бокс-заглушка» 37.5×40×60 мм, массой 80 г:

```xml
<xacro:property name="_m" value="${[float(v) for v in mount_xyz.split()]}"/>
<joint name="${name}_bracket_joint" type="fixed">
  <origin xyz="${_m[0] - 0.01875} ${_m[1]} ${_m[2]}" rpy="0 0 0"/>
  <parent link="${parent}"/>
  <child link="${name}_bracket_link"/>
</joint>
```

Позиция кронштейна **вычисляется из `mount_xyz`** ([L27-L29](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L27-L29)).
Строка `mount_xyz` разбивается на три числа (`_m`), и центр бокса ставится на
18.75 мм (половина его толщины 37.5 мм) ближе к оси инструмента, чем винт камеры:
`x = mount_x − 0.01875`, `y` и `z` — как у камеры. При дефолте `0.075 0 0.030`
получается `0.05625 0 0.030` — рядом с присоской (она доходит до x=0.0375), без
пересечения. Значит, если сдвинуть камеру через `mount_xyz`, кронштейн поедет вместе с ней.

Дальше выбор модели и сенсора ([L47-L59](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L47-L59)):

```xml
<xacro:if value="${camera_model == 'gemini335le'}">
  <xacro:include filename="$(find quasi_camera_description)/urdf/cameras/gemini335le.xacro"/>
  <xacro:camera_body_gemini335le name="${name}" parent="${parent}"
                                 use_nominal_extrinsics="${use_nominal_extrinsics}">
    <origin xyz="${mount_xyz}" rpy="${mount_rpy}"/>
  </xacro:camera_body_gemini335le>
</xacro:if>
...
<xacro:if value="${add_gz_sensor}">
  <xacro:include filename="$(find quasi_camera_description)/urdf/gz/gz_rgbd_camera.xacro"/>
  <xacro:gz_rgbd_camera name="${name}" model="${camera_model}" preview="${preview}"/>
</xacro:if>
```

Обрати внимание: `<origin .../>` передаётся в макрос **как блок** (`*origin` в
параметрах вызываемого макроса) — так поза крепления попадает внутрь.

> ⚠️ Обрати внимание: при неизвестном `camera_model` (например `d555`) тело камеры
> не создаётся, но `gz_rgbd_camera` всё равно попытается прочитать
> `config/<model>.yaml` и повесить сенсор на несуществующий фрейм — xacro упадёт.

#### Почему `mount_rpy = π -π/2 0`

У «обычного» фрейма камеры (REP-103) X — вперёд (куда смотрит). Нужно, чтобы X камеры
совпал с Z инструмента. Комментарий проверяет: `R·[1,0,0] = [0,0,1]`. Хотите
наклонить камеру к оси присоски — меняйте pitch/второй угол на несколько градусов
(например `${pi} ${-pi/2+0.2} 0`) и проверяйте в RViz по оси TF.

### 3.2. `urdf/cameras/gemini335le.xacro` — тело камеры и фреймы

Макрос `camera_body_gemini335le(name, parent, *origin, use_nominal_extrinsics:=true)`
([L27-L28](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L27-L28)).
Он свой, а не из `orbbec_description`, потому что оригинальный URDF Orbbec плоский,
без префикса, — две камеры с ним не поставишь ([L5-L8](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L5-L8)).

Цепочка фреймов при `name=gripper_camera`:

```text
link_eef
 └─ gripper_camera_mount_link          (нижний винт 1/4"; поза = mount_xyz/mount_rpy)
     └─ gripper_camera_link            (+0.034005 +0.0475 +0.014048 — паспорт Orbbec)
         ├─ gripper_camera_depth_frame ─ gripper_camera_depth_optical_frame
         ├─ gripper_camera_color_frame ─ gripper_camera_color_optical_frame   (Y −23.75 мм)
         ├─ gripper_camera_left_ir_frame                                       (совпадает с link)
         └─ gripper_camera_right_ir_frame                                      (Y −95 мм, база стерео)
```

- `gripper_camera_link` ([L41-L69](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L41-L69)):
  масса 0.220 кг (паспорт), visual — меш через `file://$(find ...)` (gz не понимает
  `package://` для наших пакетов), collision — бокс 50×124×28.6 мм.
- Всё ниже `_link` создаётся только если `use_nominal_extrinsics=true`
  ([L71](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L71)).
  На железе эти фреймы публикует драйвер Orbbec; **двух издателей одного фрейма в TF быть не должно**.

#### Оптический фрейм: почему он повёрнут

Есть два соглашения об осях:

| Фрейм | X | Y | Z |
|---|---|---|---|
| обычный (`*_frame`, `*_link`, REP-103) | вперёд | влево | вверх |
| оптический (`*_optical_frame`) | вправо по картинке | вниз по картинке | **вперёд, в глубину** |

Все алгоритмы обработки изображений (модель камеры-обскуры, `camera_info`,
`depth_image_proc`) считают, что Z — это глубина, а X/Y совпадают с осями
пикселей (u вправо, v вниз). Поэтому к каждому сенсорному фрейму добавлен
оптический с поворотом `rpy = -π/2 0 -π/2`
([L78-L81](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L78-L81)):

```xml
<joint name="${name}_depth_optical_joint" type="fixed">
  <origin xyz="0 0 0" rpy="${-pi/2} 0 ${-pi/2}"/>
  <parent link="${name}_depth_frame"/><child link="${name}_depth_optical_frame"/>
</joint>
```

Сообщения камеры (картинки и облако) несут `frame_id = gripper_camera_color_optical_frame`.

### 3.3. `urdf/gz/gz_rgbd_camera.xacro` — сенсор Gazebo

**Gazebo Sim (gz)** — симулятор физики и сенсоров. Сенсор описывается тегом
`<sensor>` внутри `<gazebo reference="звено">`; при конвертации URDF→SDF он
прикрепляется к этому звену. Тип `rgbd_camera` выдаёт цвет и глубину **одного
разрешения и FOV, совмещённые попиксельно** — аналог `depth_registration` у
настоящего драйвера ([L5-L9](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L5-L9)).

Параметры читаются из yaml ([L25-L27](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L25-L27)):

```xml
<xacro:property name="cfg_all"
  value="${xacro.load_yaml('$(find quasi_camera_description)/config/' + model + '.yaml')}"/>
<xacro:property name="cfg" value="${cfg_all['sim_preview'] if preview else cfg_all['sim']}"/>
```

Сам сенсор ([L29-L60](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L29-L60)):

```xml
<gazebo reference="${name}_color_frame">
  <sensor name="${name}_rgbd" type="rgbd_camera">
    <frame_id>${name}_color_optical_frame</frame_id>
    <topic>${name}/gz</topic>
    <update_rate>${cfg['rate']}</update_rate>
    ...
    <camera name="${name}">
      <optical_frame_id>${name}_color_optical_frame</optical_frame_id>
      <horizontal_fov>${cfg['hfov']}</horizontal_fov>
```

Ключевые моменты:

- Сенсор висит на `gripper_camera_color_frame` (обычный фрейм: gz-камера смотрит
  по X), а в сообщения пишет `frame_id = ..._color_optical_frame`. Без `<frame_id>`
  в сообщениях было бы служебное имя «склеенного» звена (gz сливает звенья с
  fixed-суставами), и TF до облака не нашёлся бы ([L20-L21](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L20-L21)).
- `<topic>gripper_camera/gz</topic>` → gz-топики `/gripper_camera/gz/image`,
  `/gripper_camera/gz/depth_image`, `/gripper_camera/gz/camera_info` (и
  `/gripper_camera/gz/points`, который мы не мостим).
- `near/far` задаются и для цвета (`<clip>`), и для глубины (`<depth_camera><clip>`).
- Шум: гауссов, `stddev = color_noise_std` ([L54-L58](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L54-L58)).
- Сенсор работает, только если в **мире** объявлена система `Sensors`. В модели
  её объявлять нельзя — будет `Scene already exists` и segfault ([L17-L18](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L17-L18)).

> ⚠️ Обрати внимание: `depth_noise_std` из yaml **нигде не используется** — в
> xacro подставлен только `color_noise_std`. Глубина в симуляции фактически без
> отдельно заданного шума.

### 3.4. `config/gemini335le.yaml` — числа сенсора

[gemini335le.yaml#L5-L24](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L5-L24):

| Ключ | `sim` (дефолт) | `sim_preview` | Смысл |
|---|---|---|---|
| `hfov` | 1.5708 (90°) | 1.5708 | горизонтальный угол обзора, рад (FOV глубины; у цвета 94°) |
| `width × height` | 1280 × 800 | 640 × 400 | разрешение, px |
| `rate` | 15.0 | 15.0 | частота кадров, Гц |
| `near` | 0.25 | 0.25 | мёртвая зона, м: ближе — глубины нет |
| `far` | 6.0 | 6.0 | максимальная дальность, м |
| `depth_noise_std` | 0.002 | 0.002 | σ глубины, м (⚠️ не применяется, см. выше) |
| `color_noise_std` | 0.007 | 0.007 | σ шума цвета |

Секция `hardware` ([L27-L28](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L27-L28))
— заметка для железа (прошивка 1.8.10), xacro её не читает.

Вертикальный FOV не задаётся: gz считает его из hfov и соотношения сторон
(1280/800 → ±~32°, как и сказано в комментарии кронштейна).

### 3.5. Крючок в `xarm_device_macro.xacro`

Макрос `xarm_device` из форка xarm_ros2 получил новые параметры
([xarm_device_macro.xacro#L21-L27](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L21-L27)):
`add_realsense_d435i:=false`, `add_d435i_links:=true`, `add_gripper_camera:=false`,
`gripper_camera_model:='gemini335le'`, `gripper_camera_nominal:=true`,
`gripper_camera_gz:=false`, `gripper_camera_preview:=false`.

Сам крючок ([L242-L257](../../src/xarm_ros2/xarm_description/urdf/xarm_device_macro.xacro#L242-L257)):

```xml
<xacro:unless value="${add_realsense_d435i}">
  <link name="${prefix}link_eef"/>
</xacro:unless>
<xacro:if value="${add_gripper_camera}">
  <xacro:include filename="$(find quasi_camera_description)/urdf/gripper_camera.xacro" />
  <xacro:gripper_camera camera_model="${gripper_camera_model}" parent="${prefix}link_eef"
                        name="gripper_camera"
                        use_nominal_extrinsics="${gripper_camera_nominal}"
                        add_gz_sensor="${gripper_camera_gz}"
                        preview="${gripper_camera_preview}" />
</xacro:if>
```

Камеры взаимоисключающие: у D435i звено `link_eef` создаётся внутри
`realsense_d435i.urdf.xacro` (меш камеры — это и есть `link_eef`), а без неё —
пустым звеном (строки 243–244).

А значения туда передаёт [cart-arm.xacro#L84-L90](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L84-L90):

```xml
add_realsense_d435i="${'$(arg camera_model)' == 'd435i'}"
add_d435i_links="true"
add_gripper_camera="${'$(arg camera_model)' != 'd435i'}"
gripper_camera_model="$(arg camera_model)"
gripper_camera_nominal="$(arg use_sim)"
gripper_camera_gz="$(arg use_sim)"
gripper_camera_preview="$(arg gripper_camera_preview)"
```

Аргументы `camera_model` (дефолт `gemini335le`) и `gripper_camera_preview`
(дефолт `false`) объявлены там же ([L33-L34](../../src/r2_robot/quasi_cart_description/urdf/cart-arm.xacro#L33-L34)).
При `use_sim:=false` (просто RViz, железо) камера рисуется без оптических фреймов и без gz-сенсора.

> ⚠️ Обрати внимание: MoveIt собирает **свою** модель руки через
> `uf_ros_lib/moveit_configs_builder.py` (см. CLAUDE.md) — по умолчанию без
> камеры. Кронштейн и корпус камеры MoveIt как препятствие не видит, пока это не
> продублировать там.

### 3.6. Что осталось от D435i

- `xarm_description/urdf/camera/realsense_d435i.urdf.xacro` — тело D435i, фреймы `camera_*`.
- [realsense.gazebo.xacro](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro)
  (382 строки) — три варианта сенсоров: `realsense_gazebo` (Gazebo Classic, L12),
  `realsense_ignition` (L135) и `realsense_gz` ([L258](../../src/xarm_ros2/xarm_description/urdf/camera/realsense.gazebo.xacro#L258)).
  В нашей связке (`gz_ros2_control/GazeboSimSystem`) используется `realsense_gz`:
  четыре **отдельных** сенсора (depth, color, left/right IR) по 1280 px,
  `horizontal_fov 1.57`, 30 Гц, топики `camera/depth`, `camera/color/image_raw`,
  `camera/infra1|2/image_raw` (L286–L369).
- Мост [gz_bridge_d435i.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge_d435i.yaml)
  отдаёт `/camera/color/image_raw`, `/camera/color/camera_info`,
  `/camera/depth/depth_info`, `/camera/depth/points` (облако — из gz напрямую).

Включается только `camera_model:=d435i`; это «старая» ветка для сравнения.
Отличия от Orbbec: другие имена топиков/фреймов (`/camera/...`,
`camera_color_optical_frame`), камера сидит прямо в `link_eef`, облако берётся из gz.

## 4. Путь данных: картинка → мост → облако

```text
gz сенсор gripper_camera_rgbd  (15 Гц, 1280×800)
  /gripper_camera/gz/image         ─┐
  /gripper_camera/gz/depth_image   ─┼─ ros_gz_bridge (gz_bridge_gripper_camera.yaml)
  /gripper_camera/gz/camera_info   ─┘
        ↓
  /gripper_camera/color/image_raw          sensor_msgs/Image (RGB)
  /gripper_camera/aligned_depth/image_raw  sensor_msgs/Image (32FC1, метры)
  /gripper_camera/color/camera_info        sensor_msgs/CameraInfo
        ↓  depth_image_proc::PointCloudXyzrgbNode (контейнер gripper_camera_proc)
  /gripper_camera/points                   sensor_msgs/PointCloud2, frame gripper_camera_color_optical_frame
        ↓
  grasp_analyzer (cloud_topic=/gripper_camera/points)
```

- **ros_gz_bridge** — нода, переводящая сообщения gz в сообщения ROS 2; список
  пар задан в [gz_bridge_gripper_camera.yaml](../../src/r2_robot/cart_xarm_bringup/config/gz_bridge_gripper_camera.yaml#L7-L24).
- **depth_image_proc** — стандартный пакет ROS: по `camera_info` (фокус, центр)
  и глубине пикселя вычисляет 3D-точку, раскрашивает её цветом. Подключён в
  [cart_xarm6_gz.launch.py#L194-L215](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L194-L215).
  Облако из gz не используется специально: так в симуляции и на железе
  конвейер одинаковый, а система координат gz-облака зависит от версии gz-sensors.
- Выбор топиков для `grasp_analyzer` по `camera_model` — [launch#L264-L272](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L264-L272).
- Аргументы launch: `camera_model` ([L364](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L364)),
  `gripper_camera_preview` ([L367](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L367)),
  условия `is_orbbec`/`is_d435i` ([L74-L75](../../src/r2_robot/cart_xarm_bringup/launch/cart_xarm6_gz.launch.py#L74-L75)).

## 5. Что здесь можно менять

| Хочу… | Файл:строка | Параметр | Пример |
|---|---|---|---|
| Сдвинуть камеру | [gripper_camera.xacro:21](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L21) | `mount_xyz` | `'0.080 0 0.040'` |
| Повернуть/наклонить камеру | [gripper_camera.xacro:21](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L21) | `mount_rpy` | `'${pi} ${-pi/2+0.17} 0'` (~10°) |
| Сместить кронштейн относительно камеры | [gripper_camera.xacro:29](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L29) | `origin xyz` (`_m[0] - 0.01875`) | обычно не нужно: кронштейн сам следует за `mount_xyz` |
| Изменить размер кронштейна | [gripper_camera.xacro:39,43](../../src/r2_robot/quasi_camera_description/urdf/gripper_camera.xacro#L39) | `box size` | `0.04 0.05 0.06` |
| Поменять разрешение | [gemini335le.yaml:7-8](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L7) | `width`, `height` | `848`, `530` |
| Поменять FOV | [gemini335le.yaml:6](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L6) | `hfov` (рад) | `1.64` (94°) |
| Поменять частоту | [gemini335le.yaml:9](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L9) | `rate` | `30.0` |
| Ближнюю/дальнюю границу | [gemini335le.yaml:10-11](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L10) | `near`, `far` | `0.2`, `3.0` |
| Шум цвета | [gemini335le.yaml:13](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L13) | `color_noise_std` | `0.0` (без шума) |
| Лёгкий режим | [gemini335le.yaml:16-24](../../src/r2_robot/quasi_camera_description/config/gemini335le.yaml#L16) + launch | `sim_preview`, `gripper_camera_preview:=true` | 640×400 |
| Старую камеру | launch | `camera_model:=d435i` | `ros2 launch ... camera_model:=d435i` |
| Переименовать gz-топик | [gz_rgbd_camera.xacro:32](../../src/r2_robot/quasi_camera_description/urdf/gz/gz_rgbd_camera.xacro#L32) + мост | `<topic>` | синхронно правь `gz_bridge_gripper_camera.yaml` |
| Массу камеры | [gemini335le.xacro:51](../../src/r2_robot/quasi_camera_description/urdf/cameras/gemini335le.xacro#L51) | `mass` | `0.220` |

Правки xacro/yaml при `--symlink-install` подхватываются после перезапуска launch без пересборки.

## 6. Частые ошибки

1. **Сенсор не публикует ничего.** В мире нет системы `gz-sim-sensors-system`, или
   наоборот она добавлена в модель → `Scene already exists`, segfault.
2. **В RViz облако не отображается, ошибка TF.** Fixed Frame в RViz не связан с
   `gripper_camera_color_optical_frame`, или сенсору убрали `<frame_id>`.
3. **Облако «смотрит» не туда / повёрнуто на 90°.** Перепутали обычный и оптический
   фрейм (точки в optical-фрейме интерпретируют как в REP-103).
4. **Нет точек вблизи.** Объект ближе `near = 0.25` м — это мёртвая зона, как у реальной камеры.
5. **Повернул камеру через `mount_rpy`, а кронштейн не повернулся** — кронштейн следует только за `mount_xyz` (сдвиг), поворот на него не влияет; при сильном наклоне проверь, что бокс не торчит сквозь камеру (см. 3.1).
6. **Добавил `camera_model:=d555`** — xacro падает: модели и yaml ещё нет.
7. **Новый yaml/меш «не находится»** — нужна пересборка пакета (`colcon build --packages-select quasi_camera_description`).
8. **Тормозит GPU/мост** — используй `gripper_camera_preview:=true`.

## 7. Проверь себя

**Упражнение 1. Найди все фреймы камеры в URDF.**

```bash
cd ~/ros2k_ws && source install/setup.bash
xacro src/r2_robot/quasi_cart_description/urdf/cart_with_arm.urdf.xacro use_sim:=true > /tmp/c.urdf
check_urdf /tmp/c.urdf | grep gripper_camera
grep -c 'gripper_camera_rgbd' /tmp/c.urdf      # 1 — сенсор есть
```
Повтори с `use_sim:=false`: оптических фреймов и сенсора быть не должно. Почему?

**Упражнение 2. Частота и разрешение.** Запусти симуляцию (`cart_xarm6_gz.launch.py`) и выполни:

```bash
ros2 topic hz /gripper_camera/color/image_raw          # ≈15 Гц
ros2 topic echo --once /gripper_camera/color/camera_info --field width   # 1280
ros2 topic hz /gripper_camera/points
```
Перезапусти с `gripper_camera_preview:=true` — width должен стать 640.

**Упражнение 3. Оптический фрейм.**

```bash
ros2 run tf2_ros tf2_echo gripper_camera_link gripper_camera_color_optical_frame
```
Объясни, почему трансляция Y = −0.02375, а поворот соответствует rpy (−90°, 0, −90°).
Затем `tf2_echo link_eef gripper_camera_color_optical_frame`: куда смотрит ось Z оптического фрейма в координатах `link_eef`?

**Упражнение 4. Наклони камеру.** Поменяй `mount_rpy` на `'${pi} ${-pi/2+0.17} 0'`,
перезапусти `cart_xarm6_rviz.launch.py` и посмотри в RViz на оси TF камеры. Не забудь
вернуть значение и подумать, нужно ли двигать кронштейн.
