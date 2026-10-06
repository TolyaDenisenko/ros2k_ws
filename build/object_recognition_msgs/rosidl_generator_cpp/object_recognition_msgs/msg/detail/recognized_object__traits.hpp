// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/RecognizedObject.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/recognized_object__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'type'
#include "object_recognition_msgs/msg/detail/object_type__traits.hpp"
// Member 'point_clouds'
#include "sensor_msgs/msg/detail/point_cloud2__traits.hpp"
// Member 'bounding_mesh'
#include "shape_msgs/msg/detail/mesh__traits.hpp"
// Member 'bounding_contours'
#include "geometry_msgs/msg/detail/point__traits.hpp"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose_with_covariance_stamped__traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RecognizedObject & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: type
  {
    out << "type: ";
    to_flow_style_yaml(msg.type, out);
    out << ", ";
  }

  // member: confidence
  {
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << ", ";
  }

  // member: point_clouds
  {
    if (msg.point_clouds.size() == 0) {
      out << "point_clouds: []";
    } else {
      out << "point_clouds: [";
      size_t pending_items = msg.point_clouds.size();
      for (auto item : msg.point_clouds) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: bounding_mesh
  {
    out << "bounding_mesh: ";
    to_flow_style_yaml(msg.bounding_mesh, out);
    out << ", ";
  }

  // member: bounding_contours
  {
    if (msg.bounding_contours.size() == 0) {
      out << "bounding_contours: []";
    } else {
      out << "bounding_contours: [";
      size_t pending_items = msg.bounding_contours.size();
      for (auto item : msg.bounding_contours) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: pose
  {
    out << "pose: ";
    to_flow_style_yaml(msg.pose, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RecognizedObject & msg,
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

  // member: type
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "type:\n";
    to_block_style_yaml(msg.type, out, indentation + 2);
  }

  // member: confidence
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << "\n";
  }

  // member: point_clouds
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.point_clouds.size() == 0) {
      out << "point_clouds: []\n";
    } else {
      out << "point_clouds:\n";
      for (auto item : msg.point_clouds) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: bounding_mesh
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "bounding_mesh:\n";
    to_block_style_yaml(msg.bounding_mesh, out, indentation + 2);
  }

  // member: bounding_contours
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.bounding_contours.size() == 0) {
      out << "bounding_contours: []\n";
    } else {
      out << "bounding_contours:\n";
      for (auto item : msg.bounding_contours) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: pose
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pose:\n";
    to_block_style_yaml(msg.pose, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RecognizedObject & msg, bool use_flow_style = false)
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
  const object_recognition_msgs::msg::RecognizedObject & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::RecognizedObject & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::RecognizedObject>()
{
  return "object_recognition_msgs::msg::RecognizedObject";
}

template<>
inline const char * name<object_recognition_msgs::msg::RecognizedObject>()
{
  return "object_recognition_msgs/msg/RecognizedObject";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::RecognizedObject>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::RecognizedObject>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::RecognizedObject>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__TRAITS_HPP_
