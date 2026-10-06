// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:msg/ObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_information.h"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"
// Member 'ground_truth_mesh'
#include "shape_msgs/msg/detail/mesh__struct.h"
// Member 'ground_truth_point_cloud'
#include "sensor_msgs/msg/detail/point_cloud2__struct.h"

/// Struct defined in msg/ObjectInformation in the package object_recognition_msgs.
/**
  * VISUALIZATION INFO ######################################################
  * THIS INFO SHOULD BE OBTAINED INDEPENDENTLY FROM THE CORE, LIKE IN AN RVIZ PLUGIN ###################
 */
typedef struct object_recognition_msgs__msg__ObjectInformation
{
  /// The human readable name of the object
  rosidl_runtime_c__String name;
  /// The full mesh of the object: this can be useful for display purposes, augmented reality ... but it can be big
  /// Make sure the type is MESH
  shape_msgs__msg__Mesh ground_truth_mesh;
  /// Sometimes, you only have a cloud in the DB
  /// Make sure the type is POINTS
  sensor_msgs__msg__PointCloud2 ground_truth_point_cloud;
} object_recognition_msgs__msg__ObjectInformation;

// Struct for a sequence of object_recognition_msgs__msg__ObjectInformation.
typedef struct object_recognition_msgs__msg__ObjectInformation__Sequence
{
  object_recognition_msgs__msg__ObjectInformation * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__msg__ObjectInformation__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__STRUCT_H_
