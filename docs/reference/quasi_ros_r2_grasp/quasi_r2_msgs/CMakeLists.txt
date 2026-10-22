cmake_minimum_required(VERSION 3.8)
project(quasi_r2_msgs)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(quasi_msgs REQUIRED)

rosidl_generate_interfaces(
    ${PROJECT_NAME}
        action/DetectObject.action
        action/FindPlacePose.action
        srv/GraspPlanning.srv
        srv/Calibration.srv
        srv/ServoPosition.srv
        srv/ServoSaveZero.srv
        msg/Object.msg
        msg/GraspConfig.msg
        msg/GraspConfigList.msg
        msg/DisplayConstants.msg
        msg/DisplayMessage.msg
        msg/DisplayIcon.msg
        msg/JointCommand.msg
        msg/DisplayAnimation.msg
        msg/DisplayTextIcons.msg
    DEPENDENCIES
        builtin_interfaces
        vision_msgs
        moveit_msgs
        sensor_msgs
        shape_msgs
        geometry_msgs
        std_msgs
        quasi_msgs
)

ament_export_dependencies(rosidl_default_runtime geometry_msgs shape_msgs sensor_msgs moveit_msgs vision_msgs quasi_msgs)

ament_package()
