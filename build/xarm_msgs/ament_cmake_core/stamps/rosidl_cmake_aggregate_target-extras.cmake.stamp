# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target xarm_msgs::xarm_msgs
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${xarm_msgs_TARGETS}.
if(xarm_msgs_TARGETS AND NOT TARGET xarm_msgs::xarm_msgs)
  add_library(xarm_msgs::xarm_msgs INTERFACE IMPORTED)
  set_target_properties(xarm_msgs::xarm_msgs PROPERTIES
    INTERFACE_LINK_LIBRARIES "${xarm_msgs_TARGETS}")
endif()
