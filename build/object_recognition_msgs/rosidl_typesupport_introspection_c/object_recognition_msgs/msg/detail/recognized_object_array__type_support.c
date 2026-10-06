// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "object_recognition_msgs/msg/detail/recognized_object_array__rosidl_typesupport_introspection_c.h"
#include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "object_recognition_msgs/msg/detail/recognized_object_array__functions.h"
#include "object_recognition_msgs/msg/detail/recognized_object_array__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `objects`
#include "object_recognition_msgs/msg/recognized_object.h"
// Member `objects`
#include "object_recognition_msgs/msg/detail/recognized_object__rosidl_typesupport_introspection_c.h"
// Member `cooccurrence`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__msg__RecognizedObjectArray__init(message_memory);
}

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_fini_function(void * message_memory)
{
  object_recognition_msgs__msg__RecognizedObjectArray__fini(message_memory);
}

size_t object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__size_function__RecognizedObjectArray__objects(
  const void * untyped_member)
{
  const object_recognition_msgs__msg__RecognizedObject__Sequence * member =
    (const object_recognition_msgs__msg__RecognizedObject__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__objects(
  const void * untyped_member, size_t index)
{
  const object_recognition_msgs__msg__RecognizedObject__Sequence * member =
    (const object_recognition_msgs__msg__RecognizedObject__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__objects(
  void * untyped_member, size_t index)
{
  object_recognition_msgs__msg__RecognizedObject__Sequence * member =
    (object_recognition_msgs__msg__RecognizedObject__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__fetch_function__RecognizedObjectArray__objects(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const object_recognition_msgs__msg__RecognizedObject * item =
    ((const object_recognition_msgs__msg__RecognizedObject *)
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__objects(untyped_member, index));
  object_recognition_msgs__msg__RecognizedObject * value =
    (object_recognition_msgs__msg__RecognizedObject *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__assign_function__RecognizedObjectArray__objects(
  void * untyped_member, size_t index, const void * untyped_value)
{
  object_recognition_msgs__msg__RecognizedObject * item =
    ((object_recognition_msgs__msg__RecognizedObject *)
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__objects(untyped_member, index));
  const object_recognition_msgs__msg__RecognizedObject * value =
    (const object_recognition_msgs__msg__RecognizedObject *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__resize_function__RecognizedObjectArray__objects(
  void * untyped_member, size_t size)
{
  object_recognition_msgs__msg__RecognizedObject__Sequence * member =
    (object_recognition_msgs__msg__RecognizedObject__Sequence *)(untyped_member);
  object_recognition_msgs__msg__RecognizedObject__Sequence__fini(member);
  return object_recognition_msgs__msg__RecognizedObject__Sequence__init(member, size);
}

size_t object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__size_function__RecognizedObjectArray__cooccurrence(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__cooccurrence(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__cooccurrence(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__fetch_function__RecognizedObjectArray__cooccurrence(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__cooccurrence(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__assign_function__RecognizedObjectArray__cooccurrence(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__cooccurrence(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__resize_function__RecognizedObjectArray__cooccurrence(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObjectArray, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "objects",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObjectArray, objects),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__size_function__RecognizedObjectArray__objects,  // size() function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__objects,  // get_const(index) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__objects,  // get(index) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__fetch_function__RecognizedObjectArray__objects,  // fetch(index, &value) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__assign_function__RecognizedObjectArray__objects,  // assign(index, value) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__resize_function__RecognizedObjectArray__objects  // resize(index) function pointer
  },
  {
    "cooccurrence",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__RecognizedObjectArray, cooccurrence),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__size_function__RecognizedObjectArray__cooccurrence,  // size() function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_const_function__RecognizedObjectArray__cooccurrence,  // get_const(index) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__get_function__RecognizedObjectArray__cooccurrence,  // get(index) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__fetch_function__RecognizedObjectArray__cooccurrence,  // fetch(index, &value) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__assign_function__RecognizedObjectArray__cooccurrence,  // assign(index, value) function pointer
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__resize_function__RecognizedObjectArray__cooccurrence  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_members = {
  "object_recognition_msgs__msg",  // message namespace
  "RecognizedObjectArray",  // message name
  3,  // number of fields
  sizeof(object_recognition_msgs__msg__RecognizedObjectArray),
  false,  // has_any_key_member_
  object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_member_array,  // message members
  object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_type_support_handle = {
  0,
  &object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_hash,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_description,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, RecognizedObjectArray)() {
  object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, RecognizedObject)();
  if (!object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__msg__RecognizedObjectArray__rosidl_typesupport_introspection_c__RecognizedObjectArray_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
