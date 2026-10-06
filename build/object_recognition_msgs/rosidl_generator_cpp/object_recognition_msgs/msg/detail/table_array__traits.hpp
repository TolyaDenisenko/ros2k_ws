// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/TableArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/table_array.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/table_array__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'tables'
#include "object_recognition_msgs/msg/detail/table__traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const TableArray & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: tables
  {
    if (msg.tables.size() == 0) {
      out << "tables: []";
    } else {
      out << "tables: [";
      size_t pending_items = msg.tables.size();
      for (auto item : msg.tables) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const TableArray & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: tables
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.tables.size() == 0) {
      out << "tables: []\n";
    } else {
      out << "tables:\n";
      for (auto item : msg.tables) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const TableArray & msg, bool use_flow_style = false)
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
  const object_recognition_msgs::msg::TableArray & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::TableArray & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::TableArray>()
{
  return "object_recognition_msgs::msg::TableArray";
}

template<>
inline const char * name<object_recognition_msgs::msg::TableArray>()
{
  return "object_recognition_msgs/msg/TableArray";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::TableArray>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::TableArray>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::TableArray>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__TRAITS_HPP_
