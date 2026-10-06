# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target object_recognition_msgs::object_recognition_msgs
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${object_recognition_msgs_TARGETS}.
if(object_recognition_msgs_TARGETS AND NOT TARGET object_recognition_msgs::object_recognition_msgs)
  add_library(object_recognition_msgs::object_recognition_msgs INTERFACE IMPORTED)
  set_target_properties(object_recognition_msgs::object_recognition_msgs PROPERTIES
    INTERFACE_LINK_LIBRARIES "${object_recognition_msgs_TARGETS}")
endif()
