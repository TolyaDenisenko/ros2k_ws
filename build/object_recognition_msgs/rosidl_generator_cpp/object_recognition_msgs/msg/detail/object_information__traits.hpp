// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/ObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_information.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/object_information__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'ground_truth_mesh'
#include "shape_msgs/msg/detail/mesh__traits.hpp"
// Member 'ground_truth_point_cloud'
#include "sensor_msgs/msg/detail/point_cloud2__traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ObjectInformation & msg,
  std::ostream & out)
{
  out << "{";
  // member: name
  {
    out << "name: ";
    rosidl_generator_traits::value_to_yaml(msg.name, out);
    out << ", ";
  }

  // member: ground_truth_mesh
  {
    out << "ground_truth_mesh: ";
    to_flow_style_yaml(msg.ground_truth_mesh, out);
    out << ", ";
  }

  // member: ground_truth_point_cloud
  {
    out << "ground_truth_point_cloud: ";
    to_flow_style_yaml(msg.ground_truth_point_cloud, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ObjectInformation & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: name
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "name: ";
    rosidl_generator_traits::value_to_yaml(msg.name, out);
    out << "\n";
  }

  // member: ground_truth_mesh
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "ground_truth_mesh:\n";
    to_block_style_yaml(msg.ground_truth_mesh, out, indentation + 2);
  }

  // member: ground_truth_point_cloud
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "ground_truth_point_cloud:\n";
    to_block_style_yaml(msg.ground_truth_point_cloud, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ObjectInformation & msg, bool use_flow_style = false)
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
  const object_recognition_msgs::msg::ObjectInformation & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::ObjectInformation & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::ObjectInformation>()
{
  return "object_recognition_msgs::msg::ObjectInformation";
}

template<>
inline const char * name<object_recognition_msgs::msg::ObjectInformation>()
{
  return "object_recognition_msgs/msg/ObjectInformation";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::ObjectInformation>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::ObjectInformation>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::ObjectInformation>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__TRAITS_HPP_
