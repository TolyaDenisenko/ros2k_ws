# Объект на столе, найденный по облаку точек (object_perception).
# Интерфейс по мотивам quasi_r2_msgs/Object (docs/reference/quasi_ros_r2_grasp).

# Устойчивый в пределах сцены идентификатор: <label>_<n> (pick_box_0, cylinder_1)
string id
# Класс по правилам config/objects.yaml; если ни одно правило не подошло —
# имя формы (box | cylinder)
string label
# Уверенность классификации 0..1 (по отклонению размеров от правила)
float32 confidence

uint8 SHAPE_BOX = 1
uint8 SHAPE_CYLINDER = 3
# Константы совпадают с shape_msgs/SolidPrimitive
uint8 shape
# BOX: [X, Y, Z] — размеры по осям pose; CYLINDER: [height, radius]
# (порядок как в shape_msgs/SolidPrimitive)
float64[] dimensions

# Центр объекта; ось Z — вверх (нормаль стола), для коробки X — вдоль длинной стороны
geometry_msgs/Pose pose
# Поза присоски для захвата: центр верхней грани, ось инструмента (Z TCP) вниз
geometry_msgs/Pose grasp_pose

# Точек в кластере (вокселей 1 см) — насколько объект хорошо виден
uint32 num_points

# Класс по картинке (нейросеть OpenVINO, нода openvino_detector): рамка детектора, в которую попало
# не меньше половины точек кластера. Пусто и 0, если детектор выключен или рамка не нашлась.
# Геометрия (pose, shape, dimensions) всегда из облака; это дополнительное «что это».
string detector_label
float32 detector_score
