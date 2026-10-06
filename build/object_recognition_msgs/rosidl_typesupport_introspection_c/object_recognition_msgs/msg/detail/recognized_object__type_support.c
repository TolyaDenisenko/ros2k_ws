// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from object_recognition_msgs:msg/RecognizedObject.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "object_recognition_msgs/msg/detail/recognized_object__rosidl_typesupport_introspection_c.h"
#include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "object_recognition_msgs/msg/detail/recognized_object__functions.h"
#include "object_recognition_msgs/msg/detail/recognized_object__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `type`
#include "object_recognition_msgs/msg/object_type.h"
// Member `type`
#include "object_recognition_msgs/msg/detail/object_type__rosidl_typesupport_introspection_c.h"
// Member `point_clouds`
#include "sensor_msgs/msg/point_cloud2.h"
// Member `point_clouds`
#include "sensor_msgs/msg/detail/point_cloud2__rosidl_typesupport_introspection_c.h"
// Member `bounding_mesh`
#include "shape_msgs/msg/mesh.h"
// Member `bounding_mesh`
#include "shape_msgs/msg/detail/mesh__rosidl_typesupport_introspection_c.h"
// Member `bounding_contours`
#include "geometry_msgs/msg/point.h"
// Member `bounding_contours`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"
// Member `pose`
#include "geometry_msgs/msg/pose_with_covariance_stamped.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose_with_covariance_stamped__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__msg__RecognizedObject__init(message_memory);
}

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_fini_function(void * message_memory)
{
  object_recognition_msgs__msg__RecognizedObject__fini(message_memory);
}

size_t object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__size_function__RecognizedObject__point_clouds(
  const void * untyped_member)
{
  const sensor_msgs__msg__PointCloud2__Sequence * member =
    (const sensor_msgs__msg__PointCloud2__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__point_clouds(
  const void * untyped_member, size_t index)
{
  const sensor_msgs__msg__PointCloud2__Sequence * member =
    (const sensor_msgs__msg__PointCloud2__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__point_clouds(
  void * untyped_member, size_t index)
{
  sensor_msgs__msg__PointCloud2__Sequence * member =
    (sensor_msgs__msg__PointCloud2__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__fetch_function__RecognizedObject__point_clouds(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const sensor_msgs__msg__PointCloud2 * item =
    ((const sensor_msgs__msg__PointCloud2 *)
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__point_clouds(untyped_member, index));
  sensor_msgs__msg__PointCloud2 * value =
    (sensor_msgs__msg__PointCloud2 *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__assign_function__RecognizedObject__point_clouds(
  void * untyped_member, size_t index, const void * untyped_value)
{
  sensor_msgs__msg__PointCloud2 * item =
    ((sensor_msgs__msg__PointCloud2 *)
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__point_clouds(untyped_member, index));
  const sensor_msgs__msg__PointCloud2 * value =
    (const sensor_msgs__msg__PointCloud2 *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__resize_function__RecognizedObject__point_clouds(
  void * untyped_member, size_t size)
{
  sensor_msgs__msg__PointCloud2__Sequence * member =
    (sensor_msgs__msg__PointCloud2__Sequence *)(untyped_member);
  sensor_msgs__msg__PointCloud2__Sequence__fini(member);
  return sensor_msgs__msg__PointCloud2__Sequence__init(member, size);
}

size_t object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__size_function__RecognizedObject__bounding_contours(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__bounding_contours(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__bounding_contours(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__fetch_function__RecognizedObject__bounding_contours(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point * item =
    ((const geometry_msgs__msg__Point *)
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__bounding_contours(untyped_member, index));
  geometry_msgs__msg__Point * value =
    (geometry_msgs__msg__Point *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__assign_function__RecognizedObject__bounding_contours(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point * item =
    ((geometry_msgs__msg__Point *)
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__bounding_contours(untyped_member, index));
  const geometry_msgs__msg__Point * value =
    (const geometry_msgs__msg__Point *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__resize_function__RecognizedObject__bounding_contours(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  geometry_msgs__msg__Point__Sequence__fini(member);
  return geometry_msgs__msg__Point__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[7] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "confidence",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, confidence),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "point_clouds",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, point_clouds),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__size_function__RecognizedObject__point_clouds,  // size() function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__point_clouds,  // get_const(index) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__point_clouds,  // get(index) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__fetch_function__RecognizedObject__point_clouds,  // fetch(index, &value) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__assign_function__RecognizedObject__point_clouds,  // assign(index, value) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__resize_function__RecognizedObject__point_clouds  // resize(index) function pointer
  },
  {
    "bounding_mesh",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, bounding_mesh),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "bounding_contours",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, bounding_contours),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__size_function__RecognizedObject__bounding_contours,  // size() function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_const_function__RecognizedObject__bounding_contours,  // get_const(index) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__get_function__RecognizedObject__bounding_contours,  // get(index) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__fetch_function__RecognizedObject__bounding_contours,  // fetch(index, &value) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__assign_function__RecognizedObject__bounding_contours,  // assign(index, value) function pointer
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__resize_function__RecognizedObject__bounding_contours  // resize(index) function pointer
  },
  {
    "pose",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObject, pose),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_members = {
  "object_recognition_msgs__msg",  // message namespace
  "RecognizedObject",  // message name
  7,  // number of fields
  sizeof(object_recognition_msgs__msg__RecognizedObject),
  false,  // has_any_key_member_
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array,  // message members
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_type_support_handle = {
  0,
  &object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__msg__RecognizedObject__get_type_hash,
  &object_recognition_msgs__msg__RecognizedObject__get_type_description,
  &object_recognition_msgs__msg__RecognizedObject__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, RecognizedObject)() {
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, ObjectType)();
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, PointCloud2)();
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, shape_msgs, msg, Mesh)();
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseWithCovarianceStamped)();
  if (!object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__msg__RecognizedObject__rosidl_typesupport_introspection_c__RecognizedObject_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
