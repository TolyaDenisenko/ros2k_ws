cmake_minimum_required(VERSION 3.8)
project(quasi_cart_description)

find_package(ament_cmake REQUIRED)

# install(DIRECTORY ...) завершится ошибкой, если какой-то из каталогов
# отсутствует — поэтому перечисляем только реально существующие.
install(DIRECTORY urdf
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
