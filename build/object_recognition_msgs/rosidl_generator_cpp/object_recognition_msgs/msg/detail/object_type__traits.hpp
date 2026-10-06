// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/ObjectType.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_type.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/object_type__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ObjectType & msg,
  std::ostream & out)
{
  out << "{";
  // member: key
  {
    out << "key: ";
    rosidl_generator_traits::value_to_yaml(msg.key, out);
    out << ", ";
  }

  // member: db
  {
    out << "db: ";
    rosidl_generator_traits::value_to_yaml(msg.db, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ObjectType & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: key
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "key: ";
    rosidl_generator_traits::value_to_yaml(msg.key, out);
    out << "\n";
  }

  // member: db
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "db: ";
    rosidl_generator_traits::value_to_yaml(msg.db, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ObjectType & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace object_recognition_msgs

namespace rosidl_generator_traits
{

[[deprecated("use object_recognition_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const object_recognition_msgs::msg::ObjectType & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::ObjectType & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::ObjectType>()
{
  return "object_recognition_msgs::msg::ObjectType";
}

template<>
inline const char * name<object_recognition_msgs::msg::ObjectType>()
{
  return "object_recognition_msgs/msg/ObjectType";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::ObjectType>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::ObjectType>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::ObjectType>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__TRAITS_HPP_
