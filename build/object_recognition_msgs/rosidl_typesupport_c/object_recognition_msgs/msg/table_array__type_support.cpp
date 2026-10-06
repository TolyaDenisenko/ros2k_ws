// generated from rosidl_typesupport_c/resource/idl__type_support.cpp.em
// with input from object_recognition_msgs:msg/TableArray.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "object_recognition_msgs/msg/detail/table_array__struct.h"
#include "object_recognition_msgs/msg/detail/table_array__type_support.h"
#include "object_recognition_msgs/msg/detail/table_array__functions.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace object_recognition_msgs
{

namespace msg
{

namespace rosidl_typesupport_c
{

typedef struct _TableArray_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _TableArray_type_support_ids_t;

static const _TableArray_type_support_ids_t _TableArray_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _TableArray_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _TableArray_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _TableArray_type_support_symbol_names_t _TableArray_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, object_recognition_msgs, msg, TableArray)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, object_recognition_msgs, msg, TableArray)),
  }
};

typedef struct _TableArray_type_support_data_t
{
  void * data[2];
} _TableArray_type_support_data_t;

static _TableArray_type_support_data_t _TableArray_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _TableArray_message_typesupport_map = {
  2,
  "object_recognition_msgs",
  &_TableArray_message_typesupport_ids.typesupport_identifier[0],
  &_TableArray_message_typesupport_symbol_names.symbol_name[0],
  &_TableArray_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t TableArray_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_TableArray_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &object_recognition_msgs__msg__TableArray__get_type_hash,
  &object_recognition_msgs__msg__TableArray__get_type_description,
  &object_recognition_msgs__msg__TableArray__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace msg

}  // namespace object_recognition_msgs

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, object_recognition_msgs, msg, TableArray)() {
  return &::object_recognition_msgs::msg::rosidl_typesupport_c::TableArray_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
