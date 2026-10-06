// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "object_recognition_msgs/msg/detail/recognized_object_array__functions.h"
#include "object_recognition_msgs/msg/detail/recognized_object_array__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace object_recognition_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void RecognizedObjectArray_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) object_recognition_msgs::msg::RecognizedObjectArray(_init);
}

void RecognizedObjectArray_fini_function(void * message_memory)
{
  auto typed_message = static_cast<object_recognition_msgs::msg::RecognizedObjectArray *>(message_memory);
  typed_message->~RecognizedObjectArray();
}

size_t size_function__RecognizedObjectArray__objects(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<object_recognition_msgs::msg::RecognizedObject> *>(untyped_member);
  return member->size();
}

const void * get_const_function__RecognizedObjectArray__objects(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<object_recognition_msgs::msg::RecognizedObject> *>(untyped_member);
  return &member[index];
}

void * get_function__RecognizedObjectArray__objects(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<object_recognition_msgs::msg::RecognizedObject> *>(untyped_member);
  return &member[index];
}

void fetch_function__RecognizedObjectArray__objects(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const object_recognition_msgs::msg::RecognizedObject *>(
    get_const_function__RecognizedObjectArray__objects(untyped_member, index));
  auto & value = *reinterpret_cast<object_recognition_msgs::msg::RecognizedObject *>(untyped_value);
  value = item;
}

void assign_function__RecognizedObjectArray__objects(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<object_recognition_msgs::msg::RecognizedObject *>(
    get_function__RecognizedObjectArray__objects(untyped_member, index));
  const auto & value = *reinterpret_cast<const object_recognition_msgs::msg::RecognizedObject *>(untyped_value);
  item = value;
}

void resize_function__RecognizedObjectArray__objects(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<object_recognition_msgs::msg::RecognizedObject> *>(untyped_member);
  member->resize(size);
}

size_t size_function__RecognizedObjectArray__cooccurrence(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__RecognizedObjectArray__cooccurrence(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__RecognizedObjectArray__cooccurrence(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__RecognizedObjectArray__cooccurrence(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__RecognizedObjectArray__cooccurrence(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__RecognizedObjectArray__cooccurrence(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__RecognizedObjectArray__cooccurrence(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__RecognizedObjectArray__cooccurrence(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember RecognizedObjectArray_message_member_array[3] = {
  {
    "header",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<std_msgs::msg::Header>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs::msg::RecognizedObjectArray, header),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "objects",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<object_recognition_msgs::msg::RecognizedObject>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs::msg::RecognizedObjectArray, objects),  // bytes offset in struct
    nullptr,  // default value
    size_function__RecognizedObjectArray__objects,  // size() function pointer
    get_const_function__RecognizedObjectArray__objects,  // get_const(index) function pointer
    get_function__RecognizedObjectArray__objects,  // get(index) function pointer
    fetch_function__RecognizedObjectArray__objects,  // fetch(index, &value) function pointer
    assign_function__RecognizedObjectArray__objects,  // assign(index, value) function pointer
    resize_function__RecognizedObjectArray__objects  // resize(index) function pointer
  },
  {
    "cooccurrence",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(object_recognition_msgs::msg::RecognizedObjectArray, cooccurrence),  // bytes offset in struct
    nullptr,  // default value
    size_function__RecognizedObjectArray__cooccurrence,  // size() function pointer
    get_const_function__RecognizedObjectArray__cooccurrence,  // get_const(index) function pointer
    get_function__RecognizedObjectArray__cooccurrence,  // get(index) function pointer
    fetch_function__RecognizedObjectArray__cooccurrence,  // fetch(index, &value) function pointer
    assign_function__RecognizedObjectArray__cooccurrence,  // assign(index, value) function pointer
    resize_function__RecognizedObjectArray__cooccurrence  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers RecognizedObjectArray_message_members = {
  "object_recognition_msgs::msg",  // message namespace
  "RecognizedObjectArray",  // message name
  3,  // number of fields
  sizeof(object_recognition_msgs::msg::RecognizedObjectArray),
  false,  // has_any_key_member_
  RecognizedObjectArray_message_member_array,  // message members
  RecognizedObjectArray_init_function,  // function to initialize message memory (memory has to be allocated)
  RecognizedObjectArray_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t RecognizedObjectArray_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &RecognizedObjectArray_message_members,
  get_message_typesupport_handle_function,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_hash,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_description,
  &object_recognition_msgs__msg__RecognizedObjectArray__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace object_recognition_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<object_recognition_msgs::msg::RecognizedObjectArray>()
{
  return &::object_recognition_msgs::msg::rosidl_typesupport_introspection_cpp::RecognizedObjectArray_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, object_recognition_msgs, msg, RecognizedObjectArray)() {
  return &::object_recognition_msgs::msg::rosidl_typesupport_introspection_cpp::RecognizedObjectArray_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
