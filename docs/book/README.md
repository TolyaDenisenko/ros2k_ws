# Книга по проекту ros2k_ws

Учебник для начинающих: что лежит в каждой папке проекта, что прописано в
файлах и где что менять.

| Глава | Тема |
|---|---|
| [0. Введение](00-введение.md) | словарь терминов, карта папок, сборка, когда нужна пересборка |
| [1. quasi_cart_description](01-quasi_cart_description.md) | модели тележек, URDF/xacro, крепление руки, вакуум (DetachableJoint) |
| [2. cart_xarm_bringup](02-cart_xarm_bringup.md) | главный launch по шагам, все аргументы, мир, мосты gz↔ROS, RViz |
| [3. quasi_camera_description](03-quasi_camera_description.md) | камера Orbbec на запястье, TF, сенсор gz, облако точек |
| [4. xarm_grasping](04-xarm_grasping.md) | grasp_analyzer, симуляция вакуума, arm_goto_tcp |
| [5. xarm_gz_bringup](05-xarm_gz_bringup.md) | рука одна на столе |
| [6. xarm_ros2](06-xarm_ros2.md) | форк UFactory: описание руки, контроллеры, MoveIt |
| [7. Рецепты](07-рецепты.md) | **шпаргалка: где поменять положение тележки, руки, стола, камеры** |

Знаком **⚠️** отмечены неочевидные места и известные проблемы. Список
актуальных проблем ведётся в [docs/STATUS.md](../STATUS.md).
