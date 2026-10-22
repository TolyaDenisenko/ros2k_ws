cmake_minimum_required(VERSION 3.8)
project(quasi_r2_find_grasp_service)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(quasi_r2_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

# GPD
find_library(gpd_LIBRARIES gpd REQUIRED)
find_path(gpd_INCLUDE_DIRS gpd/grasp_detector.h gpd/util/cloud.h)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBS})

include_directories(
  include
  ${gpd_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} src/find_grasp_server.cpp src/ros_params.cpp src/grasp_detector_node.cpp src/gpd_grasp_engine.cpp)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  quasi_r2_msgs
  PCL
  pcl_ros
  visualization_msgs
  tf2
  tf2_eigen
  tf2_ros
  tf2_geometry_msgs
)


target_link_libraries(${PROJECT_NAME}
  ${gpd_LIBRARIES}
  ${PCL_LIBRARIES}
  ${trajectory_msgs_LIBRARIES}
  ${pcl_conversions_LIBRARIES}
  ${gpd_INCLUDE_LIBRARIES}
)

# Install libs
install(TARGETS
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

# Install header files
install(
  DIRECTORY include/
  DESTINATION include
)

install(
  DIRECTORY launch cfg
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
