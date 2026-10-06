// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/Table.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/table.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/table__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__traits.hpp"
// Member 'convex_hull'
#include "geometry_msgs/msg/detail/point__traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Table & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: pose
  {
    out << "pose: ";
    to_flow_style_yaml(msg.pose, out);
    out << ", ";
  }

  // member: convex_hull
  {
    if (msg.convex_hull.size() == 0) {
      out << "convex_hull: []";
    } else {
      out << "convex_hull: [";
      size_t pending_items = msg.convex_hull.size();
      for (auto item : msg.convex_hull) {
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
  const Table & msg,
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

  // member: pose
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pose:\n";
    to_block_style_yaml(msg.pose, out, indentation + 2);
  }

  // member: convex_hull
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.convex_hull.size() == 0) {
      out << "convex_hull: []\n";
    } else {
      out << "convex_hull:\n";
      for (auto item : msg.convex_hull) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Table & msg, bool use_flow_style = false)
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
  const object_recognition_msgs::msg::Table & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::Table & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::Table>()
{
  return "object_recognition_msgs::msg::Table";
}

template<>
inline const char * name<object_recognition_msgs::msg::Table>()
{
  return "object_recognition_msgs/msg/Table";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::Table>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::Table>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::Table>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__TRAITS_HPP_
