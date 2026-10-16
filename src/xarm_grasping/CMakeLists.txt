cmake_minimum_required(VERSION 3.8)
project(xarm_grasping)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch config worlds scripts
  DESTINATION share/${PROJECT_NAME}
)

# python-модуль (библиотека генерации захватов)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
install(FILES xarm_grasping/grasp_generator.py
  DESTINATION lib/python3/${Python3_VERSION_MAJOR}/site-packages/${PROJECT_NAME}
)

ament_python_install_package(${PROJECT_NAME} PACKAGE_DIR ${PROJECT_NAME})

ament_package()
