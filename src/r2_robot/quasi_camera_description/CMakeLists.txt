cmake_minimum_required(VERSION 3.8)
project(quasi_camera_description)

find_package(ament_cmake REQUIRED)

# Меши, xacro-макросы и yaml-параметры камер ставим в share/: xacro резолвит
# $(find quasi_camera_description), а gz-sim — package://quasi_camera_description/...
install(DIRECTORY urdf config meshes
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
