cmake_minimum_required(VERSION 3.8)
project(quasi_r2_perception)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(Boost 1.78 REQUIRED COMPONENTS system filesystem date_time iostreams json url)
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(bondcpp REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(quasi_r2_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(object_msgs REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(quasi_utils REQUIRED)
find_package(geometric_shapes REQUIRED)
find_package(image_geometry REQUIRED)
find_package(message_filters REQUIRED)
find_package(nlohmann_json REQUIRED)

if(PCL_VERSION VERSION_LESS "1.12.0")
  find_library(QUASI_QHULL_LIBRARY NAMES qhull)
else()
  find_library(QUASI_QHULL_LIBRARY NAMES qhull_r)
endif()

add_executable(object_recognition_test
        src/object_recognition_test_node.cpp
        src/correspondence_grouping.cpp
        src/objrec_ransac.cpp
        src/object_segmentation.cpp
        )
ament_target_dependencies(object_recognition_test
        rclcpp
        rclcpp_action
#        quasi_r2_msgs
#        quasi_utils
        sensor_msgs
        tf2_ros
        tf2_geometry_msgs
        tf2_eigen
        PCL
        pcl_ros
        pcl_conversions
        object_msgs
        geometric_shapes
        )
target_include_directories(object_recognition_test PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(object_recognition_test ${PCL_LIBRARIES})


add_executable(perception
        src/perception_node.cpp
        src/perception_detect_object_action.cpp
        src/perception_find_place_pose_action.cpp
        src/perception_node_qhull.cpp
        src/object_segmentation.cpp
        src/shape_extraction.cpp
        )
ament_target_dependencies(perception
        rclcpp
        rclcpp_action
        rclcpp_lifecycle
        bondcpp
        quasi_utils
        vision_msgs
        quasi_r2_msgs
        sensor_msgs
        object_msgs
        tf2_ros
        tf2_geometry_msgs
        tf2_eigen
        PCL
        pcl_ros
        pcl_conversions
        geometric_shapes
        image_geometry
        message_filters
        nlohmann_json
        )
target_include_directories(perception PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(perception ${PCL_LIBRARIES}  ${QUASI_QHULL_LIBRARY})



install(TARGETS object_recognition_test perception
        RUNTIME DESTINATION lib/${PROJECT_NAME}
        LIBRARY DESTINATION lib
        )
install(DIRECTORY param launch
        DESTINATION share/${PROJECT_NAME}
        )

#install(
#  DIRECTORY include/
#  DESTINATION include
#)
#install(
#  TARGETS quasi_r2_perception
#  EXPORT export_${PROJECT_NAME}
#  ARCHIVE DESTINATION lib
#  LIBRARY DESTINATION lib
#  RUNTIME DESTINATION bin
#)

#ament_export_include_directories(
#  include
#)
#ament_export_libraries(
#  quasi_r2_perception
#)
#ament_export_targets(
#  export_${PROJECT_NAME}
#)

ament_python_install_package(${PROJECT_NAME})

ament_package()
