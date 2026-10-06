// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from object_recognition_msgs:msg/ObjectType.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "object_recognition_msgs/msg/detail/object_type__rosidl_typesupport_introspection_c.h"
#include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "object_recognition_msgs/msg/detail/object_type__functions.h"
#include "object_recognition_msgs/msg/detail/object_type__struct.h"


// Include directives for member types
// Member `key`
// Member `db`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__msg__ObjectType__init(message_memory);
}

void object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_fini_function(void * message_memory)
{
  object_recognition_msgs__msg__ObjectType__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_member_array[2] = {
  {
    "key",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__ObjectType, key),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "db",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__msg__ObjectType, db),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_members = {
  "object_recognition_msgs__msg",  // message namespace
  "ObjectType",  // message name
  2,  // number of fields
  sizeof(object_recognition_msgs__msg__ObjectType),
  false,  // has_any_key_member_
  object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_member_array,  // message members
  object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_type_support_handle = {
  0,
  &object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__msg__ObjectType__get_type_hash,
  &object_recognition_msgs__msg__ObjectType__get_type_description,
  &object_recognition_msgs__msg__ObjectType__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, ObjectType)() {
  if (!object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__msg__ObjectType__rosidl_typesupport_introspection_c__ObjectType_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
