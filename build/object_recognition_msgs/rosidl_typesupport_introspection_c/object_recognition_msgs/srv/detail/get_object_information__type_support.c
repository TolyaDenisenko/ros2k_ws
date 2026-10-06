// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from object_recognition_msgs:srv/GetObjectInformation.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "object_recognition_msgs/srv/detail/get_object_information__rosidl_typesupport_introspection_c.h"
#include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "object_recognition_msgs/srv/detail/get_object_information__functions.h"
#include "object_recognition_msgs/srv/detail/get_object_information__struct.h"


// Include directives for member types
// Member `type`
#include "object_recognition_msgs/msg/object_type.h"
// Member `type`
#include "object_recognition_msgs/msg/detail/object_type__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__srv__GetObjectInformation_Request__init(message_memory);
}

void object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_fini_function(void * message_memory)
{
  object_recognition_msgs__srv__GetObjectInformation_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_member_array[1] = {
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__srv__GetObjectInformation_Request, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_members = {
  "object_recognition_msgs__srv",  // message namespace
  "GetObjectInformation_Request",  // message name
  1,  // number of fields
  sizeof(object_recognition_msgs__srv__GetObjectInformation_Request),
  false,  // has_any_key_member_
  object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_member_array,  // message members
  object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle = {
  0,
  &object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__srv__GetObjectInformation_Request__get_type_hash,
  &object_recognition_msgs__srv__GetObjectInformation_Request__get_type_description,
  &object_recognition_msgs__srv__GetObjectInformation_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Request)() {
  object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, ObjectType)();
  if (!object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__rosidl_typesupport_introspection_c.h"
// already included above
// #include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__functions.h"
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__struct.h"


// Include directives for member types
// Member `information`
#include "object_recognition_msgs/msg/object_information.h"
// Member `information`
#include "object_recognition_msgs/msg/detail/object_information__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__srv__GetObjectInformation_Response__init(message_memory);
}

void object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_fini_function(void * message_memory)
{
  object_recognition_msgs__srv__GetObjectInformation_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_member_array[1] = {
  {
    "information",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__srv__GetObjectInformation_Response, information),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_members = {
  "object_recognition_msgs__srv",  // message namespace
  "GetObjectInformation_Response",  // message name
  1,  // number of fields
  sizeof(object_recognition_msgs__srv__GetObjectInformation_Response),
  false,  // has_any_key_member_
  object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_member_array,  // message members
  object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle = {
  0,
  &object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__srv__GetObjectInformation_Response__get_type_hash,
  &object_recognition_msgs__srv__GetObjectInformation_Response__get_type_description,
  &object_recognition_msgs__srv__GetObjectInformation_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Response)() {
  object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, ObjectInformation)();
  if (!object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__rosidl_typesupport_introspection_c.h"
// already included above
// #include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__functions.h"
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "object_recognition_msgs/srv/get_object_information.h"
// Member `request`
// Member `response`
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  object_recognition_msgs__srv__GetObjectInformation_Event__init(message_memory);
}

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_fini_function(void * message_memory)
{
  object_recognition_msgs__srv__GetObjectInformation_Event__fini(message_memory);
}

size_t object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__size_function__GetObjectInformation_Event__request(
  const void * untyped_member)
{
  const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * member =
    (const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__request(
  const void * untyped_member, size_t index)
{
  const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * member =
    (const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__request(
  void * untyped_member, size_t index)
{
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * member =
    (object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__fetch_function__GetObjectInformation_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const object_recognition_msgs__srv__GetObjectInformation_Request * item =
    ((const object_recognition_msgs__srv__GetObjectInformation_Request *)
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__request(untyped_member, index));
  object_recognition_msgs__srv__GetObjectInformation_Request * value =
    (object_recognition_msgs__srv__GetObjectInformation_Request *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__assign_function__GetObjectInformation_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  object_recognition_msgs__srv__GetObjectInformation_Request * item =
    ((object_recognition_msgs__srv__GetObjectInformation_Request *)
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__request(untyped_member, index));
  const object_recognition_msgs__srv__GetObjectInformation_Request * value =
    (const object_recognition_msgs__srv__GetObjectInformation_Request *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__resize_function__GetObjectInformation_Event__request(
  void * untyped_member, size_t size)
{
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * member =
    (object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *)(untyped_member);
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(member);
  return object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(member, size);
}

size_t object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__size_function__GetObjectInformation_Event__response(
  const void * untyped_member)
{
  const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * member =
    (const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__response(
  const void * untyped_member, size_t index)
{
  const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * member =
    (const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__response(
  void * untyped_member, size_t index)
{
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * member =
    (object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__fetch_function__GetObjectInformation_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const object_recognition_msgs__srv__GetObjectInformation_Response * item =
    ((const object_recognition_msgs__srv__GetObjectInformation_Response *)
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__response(untyped_member, index));
  object_recognition_msgs__srv__GetObjectInformation_Response * value =
    (object_recognition_msgs__srv__GetObjectInformation_Response *)(untyped_value);
  *value = *item;
}

void object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__assign_function__GetObjectInformation_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  object_recognition_msgs__srv__GetObjectInformation_Response * item =
    ((object_recognition_msgs__srv__GetObjectInformation_Response *)
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__response(untyped_member, index));
  const object_recognition_msgs__srv__GetObjectInformation_Response * value =
    (const object_recognition_msgs__srv__GetObjectInformation_Response *)(untyped_value);
  *item = *value;
}

bool object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__resize_function__GetObjectInformation_Event__response(
  void * untyped_member, size_t size)
{
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * member =
    (object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *)(untyped_member);
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(member);
  return object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs__srv__GetObjectInformation_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(object_recognition_msgs__srv__GetObjectInformation_Event, request),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__size_function__GetObjectInformation_Event__request,  // size() function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__request,  // get_const(index) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__request,  // get(index) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__fetch_function__GetObjectInformation_Event__request,  // fetch(index, &value) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__assign_function__GetObjectInformation_Event__request,  // assign(index, value) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__resize_function__GetObjectInformation_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(object_recognition_msgs__srv__GetObjectInformation_Event, response),  // bytes offset in struct
    NULL,  // default value
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__size_function__GetObjectInformation_Event__response,  // size() function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_const_function__GetObjectInformation_Event__response,  // get_const(index) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__get_function__GetObjectInformation_Event__response,  // get(index) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__fetch_function__GetObjectInformation_Event__response,  // fetch(index, &value) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__assign_function__GetObjectInformation_Event__response,  // assign(index, value) function pointer
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__resize_function__GetObjectInformation_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_members = {
  "object_recognition_msgs__srv",  // message namespace
  "GetObjectInformation_Event",  // message name
  3,  // number of fields
  sizeof(object_recognition_msgs__srv__GetObjectInformation_Event),
  false,  // has_any_key_member_
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_member_array,  // message members
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_type_support_handle = {
  0,
  &object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__srv__GetObjectInformation_Event__get_type_hash,
  &object_recognition_msgs__srv__GetObjectInformation_Event__get_type_description,
  &object_recognition_msgs__srv__GetObjectInformation_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Event)() {
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Request)();
  object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Response)();
  if (!object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "object_recognition_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "object_recognition_msgs/srv/detail/get_object_information__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_members = {
  "object_recognition_msgs__srv",  // service namespace
  "GetObjectInformation",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle,
  NULL,  // response message
  // object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle
  NULL  // event_message
  // object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle
};


static rosidl_service_type_support_t object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_type_support_handle = {
  0,
  &object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_members,
  get_service_typesupport_handle_function,
  &object_recognition_msgs__srv__GetObjectInformation_Request__rosidl_typesupport_introspection_c__GetObjectInformation_Request_message_type_support_handle,
  &object_recognition_msgs__srv__GetObjectInformation_Response__rosidl_typesupport_introspection_c__GetObjectInformation_Response_message_type_support_handle,
  &object_recognition_msgs__srv__GetObjectInformation_Event__rosidl_typesupport_introspection_c__GetObjectInformation_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    object_recognition_msgs,
    srv,
    GetObjectInformation
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    object_recognition_msgs,
    srv,
    GetObjectInformation
  ),
  &object_recognition_msgs__srv__GetObjectInformation__get_type_hash,
  &object_recognition_msgs__srv__GetObjectInformation__get_type_description,
  &object_recognition_msgs__srv__GetObjectInformation__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_object_recognition_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation)(void) {
  if (!object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_type_support_handle.typesupport_identifier) {
    object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, srv, GetObjectInformation_Event)()->data;
  }

  return &object_recognition_msgs__srv__detail__get_object_information__rosidl_typesupport_introspection_c__GetObjectInformation_service_type_support_handle;
}
