// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:msg/Table.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/table.h"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.h"
// Member 'convex_hull'
#include "geometry_msgs/msg/detail/point__struct.h"

/// Struct defined in msg/Table in the package object_recognition_msgs.
/**
  * Informs that a planar table has been detected at a given location
 */
typedef struct object_recognition_msgs__msg__Table
{
  std_msgs__msg__Header header;
  /// The pose gives you the transform that take you to the coordinate system
  /// of the table, with the origin somewhere in the table plane and the
  /// z axis normal to the plane
  geometry_msgs__msg__Pose pose;
  /// There is no guarantee that the table does NOT extend further than the
  /// convex hull; this is just as far as we've observed it.
  /// The origin of the table coordinate system is inside the convex hull
  /// Set of points forming the convex hull of the table
  geometry_msgs__msg__Point__Sequence convex_hull;
} object_recognition_msgs__msg__Table;

// Struct for a sequence of object_recognition_msgs__msg__Table.
typedef struct object_recognition_msgs__msg__Table__Sequence
{
  object_recognition_msgs__msg__Table * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__msg__Table__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__STRUCT_H_
