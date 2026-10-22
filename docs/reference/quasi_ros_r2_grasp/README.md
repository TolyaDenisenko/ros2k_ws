# Алгоритмы захвата из quasi_ros_r2 — задел для двухпальцевого схвата

Копия трёх пакетов из репозитория робота R2 (`Ros-r2/quasi_ros_r2`, получен
2026-10-22). Сохранены как **справочный материал** для будущей работы с
двухпальцевым схватом xArm (`add_gripper:=true`); сейчас мы работаем с вакуумной
присоской, эти пакеты **не собираются** (`COLCON_IGNORE` в этой папке).

C++ / PCL, ROS 2. Зависят от libgpd (GPD) и PCL; при переносе проверить сборку
под Kilted.

## Что здесь

| Пакет | Что внутри | Для чего нам |
|---|---|---|
| `quasi_r2_find_grasp_service` | `gpd_grasp_engine.cpp` — обёртка над [GPD](https://github.com/atenpas/gpd) (Grasp Pose Detection): облако объекта → кандидаты захвата двумя пальцами (поза кисти, ширина раскрытия, оценка). Перевод в `moveit_msgs/Grasp` (pre-grasp approach, post-grasp retreat, позы пальцев). Фильтры: по углу к желаемому направлению подхода, по рабочей зоне, по порогу оценки. Маркеры пальцев для RViz. | **Основной кандидат для двухпальцевого схвата.** |
| `quasi_r2_perception` | Сегментация сцены (`object_segmentation.cpp`: RANSAC-плоскости, горизонтальные = опоры, евклидова кластеризация, опорная плоскость для каждого объекта); подбор примитива (`shape_extraction.cpp`: выпуклая оболочка на плоскости → коробка мин. площади с поворотом / цилиндр мин. радиуса, выбор по меньшему объёму); поиск места для установки (`perception_find_place_pose_action.cpp`); RANSAC-распознавание по модели (`objrec_ransac.cpp`, `correspondence_grouping.cpp`). | Сегментация и подбор формы нужны **любому** схвату, в т.ч. присоске (задача K6: анализатор считает всё цилиндром). Поиск места — для шага «положить». |
| `quasi_r2_msgs` | `Object` (кластер + примитив + поза + плоскость), действия `DetectObject`, `FindPlacePose`, сервис `GraspPlanning` (объект → `moveit_msgs/Grasp[]`), `GraspConfig` (формат GPD). | Готовый интерфейс «восприятие → планировщик захвата → MoveIt». |

## Ключевые параметры

`quasi_r2_find_grasp_service/cfg/recognize_pick.yaml` (схват R2):

- геометрия кисти для GPD: `finger_width 0.018`, `hand_outer_diameter 0.12`,
  `hand_depth 0.07`, `hand_height 0.05`, раскрытие `0.005..0.092` м;
- поиск: `num_samples 500`, `num_selected 150`, `num_orientations 8`,
  `thresh_rad 0.785` (допуск по направлению подхода);
- перевод в MoveIt: `eef_offset 0.15` (от основания кисти до последнего
  звена руки), `grasp_offset`, `grasp_frame_id`, `finger_joint_names`.

Для схвата xArm эти числа заменить на геометрию xArm Gripper
(`xarm_description/urdf/gripper/`).

`quasi_r2_perception/param/perception.yaml`: `plane_distance_threshold 0.02`,
`plane_angle_threshold 0.1`, `cluster_tolerance 0.025`, `cluster_min_size 100`.

## Конвейер в R2 (как связано)

```
облако камеры ─► perception: DetectObject
                  └ crop по рабочей зоне → нормали → сегментация (плоскости,
                    кластеры) → [OpenVINO ROI] → примитив (shape_extraction)
                  ─► quasi_r2_msgs/Object (point_cluster с нормалями)
               ─► find_grasp_service: GraspPlanning
                  └ облако в кадр камеры → GPD → фильтры → moveit_msgs/Grasp[]
               ─► MoveIt pick (pre-grasp approach / grasp / retreat)
место ─► perception: FindPlacePose (свободные точки на плоскости)
```
