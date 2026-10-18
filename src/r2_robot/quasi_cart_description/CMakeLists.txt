cmake_minimum_required(VERSION 3.8)
project(quasi_cart_description)

find_package(ament_cmake REQUIRED)

# install(DIRECTORY ...) завершится ошибкой, если какой-то из каталогов
# отсутствует — поэтому перечисляем только реально существующие.
# ВАЖНО: meshes ОБЯЗАТЕЛЬНО должны устанавливаться в share/ — xacro разворачивает
# $(find quasi_cart_description) в .../share/quasi_cart_description, и Gazebo
# (gz-sim) резолвит package://quasi_cart_description/meshes/*.STL именно через
# AMENT_RESOURCE_INDEX. Если meshes не установлены — модель тележки в Gazebo
# не отрисовывается вовсе, а рука отображается "наполовину" (звенья с примитивами
# видны, звенья с mesh — нет).
install(DIRECTORY urdf meshes config launch rviz worlds
  DESTINATION share/${PROJECT_NAME}
  OPTIONAL
)

ament_package()
