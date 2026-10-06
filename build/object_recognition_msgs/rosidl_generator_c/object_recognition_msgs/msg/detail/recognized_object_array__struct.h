// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object_array.h"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__STRUCT_H_

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
// Member 'objects'
#include "object_recognition_msgs/msg/detail/recognized_object__struct.h"
// Member 'cooccurrence'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/RecognizedObjectArray in the package object_recognition_msgs.
/**
  * HEADER ###########################################################
 */
typedef struct object_recognition_msgs__msg__RecognizedObjectArray
{
  std_msgs__msg__Header header;
  /// This message type describes a potential scene configuration: a set of objects that can explain the scene
  object_recognition_msgs__msg__RecognizedObject__Sequence objects;
  /// SEARCH ###########################################################
  /// The co-occurrence matrix between the recognized objects
  rosidl_runtime_c__float__Sequence cooccurrence;
} object_recognition_msgs__msg__RecognizedObjectArray;

// Struct for a sequence of object_recognition_msgs__msg__RecognizedObjectArray.
typedef struct object_recognition_msgs__msg__RecognizedObjectArray__Sequence
{
  object_recognition_msgs__msg__RecognizedObjectArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__msg__RecognizedObjectArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__STRUCT_H_
