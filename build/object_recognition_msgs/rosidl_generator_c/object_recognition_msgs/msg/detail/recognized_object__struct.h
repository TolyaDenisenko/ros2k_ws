// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:msg/RecognizedObject.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object.h"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__STRUCT_H_

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
// Member 'type'
#include "object_recognition_msgs/msg/detail/object_type__struct.h"
// Member 'point_clouds'
#include "sensor_msgs/msg/detail/point_cloud2__struct.h"
// Member 'bounding_mesh'
#include "shape_msgs/msg/detail/mesh__struct.h"
// Member 'bounding_contours'
#include "geometry_msgs/msg/detail/point__struct.h"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose_with_covariance_stamped__struct.h"

/// Struct defined in msg/RecognizedObject in the package object_recognition_msgs.
/**
  * HEADER ###########################################################
 */
typedef struct object_recognition_msgs__msg__RecognizedObject
{
  /// The header frame corresponds to the pose frame, NOT the point_cloud frame.
  std_msgs__msg__Header header;
  /// OBJECT INFO #########################################################
  /// Contains information about the type and the position of a found object
  /// Some of those fields might not be filled because the used techniques do not fill them or because the user does not
  /// request them
  /// The type of the found object
  object_recognition_msgs__msg__ObjectType type;
  /// confidence: how sure you are it is that object and not another one.
  ///  It is between 0 and 1 and the closer to one it is the better
  float confidence;
  /// OBJECT CLUSTERS #######################################################
  /// Sometimes you can extract the 3d points that belong to the object, in the frames of the original sensors
  /// (it is an array as you might have several sensors)
  sensor_msgs__msg__PointCloud2__Sequence point_clouds;
  /// Sometimes, you can only provide a bounding box/shape, even in 3d
  /// This is in the pose frame
  shape_msgs__msg__Mesh bounding_mesh;
  /// Sometimes, you only have 2d input so you can't really give a pose, you just get a contour, or a box
  /// The last point will be linked to the first one automatically
  geometry_msgs__msg__Point__Sequence bounding_contours;
  /// POSE INFO #########################################################
  /// This is the result that everybody expects : the pose in some frame given with the input. The units are radian/meters
  /// as usual
  geometry_msgs__msg__PoseWithCovarianceStamped pose;
} object_recognition_msgs__msg__RecognizedObject;

// Struct for a sequence of object_recognition_msgs__msg__RecognizedObject.
typedef struct object_recognition_msgs__msg__RecognizedObject__Sequence
{
  object_recognition_msgs__msg__RecognizedObject * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__msg__RecognizedObject__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__STRUCT_H_
