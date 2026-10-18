cmake_minimum_required(VERSION 3.8)
project(xarm_grasping)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

# ВАЖНО: install(DIRECTORY ...) падает с ошибкой, если указанный каталог
# пустой или отсутствует. Поэтому каждый каталог ставим отдельно и только
# при наличии (если удалите worlds/ — сборка не сломается).
foreach(dir launch config worlds scripts)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    file(GLOB _contents RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} ${dir}/*)
    if(_contents)
      install(DIRECTORY ${dir} DESTINATION share/${PROJECT_NAME})
    endif()
  endif()
endforeach()

# python-пакет xarm_grasping (grasp_generator.py) — ament добавит его путь
# в PYTHONPATH через package.sh, поэтому "from xarm_grasping..." работает
# после source install/setup.bash. Отдельный site-packages не нужен.
ament_python_install_package(${PROJECT_NAME} PACKAGE_DIR ${PROJECT_NAME})

ament_package()
