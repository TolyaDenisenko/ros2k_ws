# Что лежит на столе и куда это ставить: восприятие → сцена MoveIt → pick&place

Сценарий «рука на столе» (`ros2 launch cart_xarm_bringup arm_table_gz.launch.py`).
Три ноды пакета `xarm_grasping` и интерфейсы пакета `xarm_grasping_msgs`.

```
/gripper_camera/points ─► object_perception ── /objects (DetectedObjectArray) ──┐
                           облако → стол → кластеры → форма → класс             │
                           action /detect_objects                               ▼
                                                                         scene_manager
config/objects.yaml (правила классов) ──────────────────────────────►    Planning Scene MoveIt:
config/place_zones.yaml (зоны) ─────────────────────────────────────►    объекты, стол, камера;
/vacuum_gripper/held ───────────────────────────────────────────────►    захват = прикрепить к link_tcp
                                                                          action /find_place_pose
                                                                          /place_zones/markers
                                                                                │
action /pick_place (object_id, zone) ─► pick_place_server ─── MoveIt (/move_action,
                                         detect → место → над объектом → вниз →   /compute_cartesian_path,
                                         вакуум → вверх → над зоной → вниз →      /execute_trajectory)
                                         отпустить → вверх → detect               + /vacuum_gripper/set
```

## 1. Восприятие — `object_perception.py`

Облако камеры гриппера переводится в `link_base` и обрабатывается функциями
`xarm_grasping/perception.py` (без ROS, можно тестировать отдельно):

1. **Стол** — плоскость около `table_height` (для стола 0.0), σ шума глубины.
2. **Кластеры** — ячейки 1×1 см, у которых медиана высоты выше max(1 см, 3σ);
   одиночные ячейки — шум.
3. **Форма** (по мотивам `shape_extraction.cpp` R2): выпуклая оболочка верхней
   части объекта → прямоугольник минимальной площади (коробка, с поворотом) и
   описанная окружность (цилиндр); побеждает меньший объём. Высота — медиана
   верхней грани.
4. **Класс** — правила `config/objects.yaml`: форма + размеры с допуском `tol`.
   Не подошло ни одно — класс = форма (`box` / `cylinder`), маркер красный.

Результат — `xarm_grasping_msgs/DetectedObjectArray` в `/objects` (раз в секунду и
по запросу) и маркеры `/objects/markers` (форма + подпись `id (уверенность)`,
размеры в см).

| Поле `DetectedObject` | Что |
|---|---|
| `id` | `<класс>_<n>`, сохраняется между распознаваниями (тот же класс в 5 см) |
| `label`, `confidence` | класс и уверенность 0..1 |
| `shape`, `dimensions` | `SHAPE_BOX` [L, W, H] / `SHAPE_CYLINDER` [H, R] |
| `pose` | центр; для коробки X вдоль длинной стороны |
| `grasp_pose` | центр верхней грани, присоска вниз |

Объект в присоске (пока вакуум держит) в `/objects` не попадает.

```bash
ros2 topic echo /objects
ros2 action send_goal /detect_objects xarm_grasping_msgs/action/DetectObjects "{update_scene: true}"
```

## 2. Сцена MoveIt — `scene_manager.py`

* объекты из `/objects` → `CollisionObject` в Planning Scene — MoveIt объезжает их;
* стол → `table` (верх на 3 мм ниже найденной плоскости — база руки стоит на нём);
* камера гриппера с кронштейном → прикреплённое тело на `link_eef` (её нет в
  модели MoveIt);
* вакуум схватил (`/vacuum_gripper/held`) → ближайший к присоске объект
  прикрепляется к `link_tcp`; отпустил → откреплён и остаётся там, где поставлен.

Это работает и при ручном захвате (кнопка в RViz), не только из `/pick_place`.

## 3. Куда ставить — зоны

`config/place_zones.yaml`: прямоугольники на столе (в `link_base`) и что в них
ставится (`accepts`: классы или категории из `objects.yaml`). Видны в RViz
(дисплей PlaceZones).

action `/find_place_pose` (`object_id`, `zone`): сетка точек в зоне (шаг
`search_step`), объект целиком в зоне, до других объектов ≥ `clearance`; из
свободных — ближайшая к центру зоны. Зона не задана — первая, которая принимает
класс объекта.

```bash
ros2 action send_goal /find_place_pose xarm_grasping_msgs/action/FindPlacePose "{object_id: pick_box_0}"
```

## 4. Взять и поставить — `pick_place_server.py`

```bash
ros2 action send_goal /pick_place xarm_grasping_msgs/action/PickPlace \
    "{object_id: pick_box_0}" --feedback          # зона по правилам
ros2 action send_goal /pick_place xarm_grasping_msgs/action/PickPlace \
    "{object_id: cylinder_block_0, zone: zone_cylinders}" --feedback
```

Шаги (feedback `stage`): `detect` → место в зоне (ищется ДО захвата) →
`plan_pregrasp` (MoveIt OMPL над объектом) → `approach` (прямо вниз до 5 мм над
верхом) → `grip` (ждём `/vacuum_gripper/ready`, вакуум) → `lift` → `transfer`
(MoveIt над местом) → `lower` (низ объекта в 6 мм над столом) → `release` →
`retreat` → `detect`. Ошибка любого шага — `success: false` и причина; цель можно
отменить.

Требует MoveIt (`use_moveit:=true`, по умолчанию). Без окна RViz:
`use_rviz:=false` (аргумент добавлен в `xarm_moveit_config/_robot_moveit_common`).

## Сцена со многими объектами

Список объектов стола — `cart_xarm_bringup/config/arm_table_objects.yaml` (имя =
класс, форма, размер, место). После правки:

```bash
python3 src/r2_robot/cart_xarm_bringup/tools/gen_arm_table_scene.py   # мир + мост
```

Новый класс — добавить правило в `xarm_grasping/config/objects.yaml` (иначе
распознается как `box` / `cylinder`). Объекты отправляются по id из `/objects`
(`<класс>_<n>`); id держится, пока объект на месте, и переезжает вместе с ним,
если его унесла присоска.

## Ограничения

* Объекты видны только с камеры гриппера: распознавание — только из позы
  наблюдения (она же стартовая); из других поз сцена не обновляется.
* Соседние объекты ближе ~5 см иногда сливаются в один кластер (вероятно,
  «летающие» точки на перепадах глубины).
* Классы — по форме и размеру; объекты одной формы и размера не различаются
  (нужен цвет/нейросеть — следующий шаг).
* Захват — центр верхней грани, присоска вертикально; наклонённые/лежащие на боку
  объекты не обрабатываются.
* Тележка (`cart_xarm6_gz.launch.py`) этими нодами пока не пользуется.
