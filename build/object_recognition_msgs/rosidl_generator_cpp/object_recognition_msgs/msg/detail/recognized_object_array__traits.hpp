// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object_array.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/msg/detail/recognized_object_array__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'objects'
#include "object_recognition_msgs/msg/detail/recognized_object__traits.hpp"

namespace object_recognition_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RecognizedObjectArray & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: objects
  {
    if (msg.objects.size() == 0) {
      out << "objects: []";
    } else {
      out << "objects: [";
      size_t pending_items = msg.objects.size();
      for (auto item : msg.objects) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: cooccurrence
  {
    if (msg.cooccurrence.size() == 0) {
      out << "cooccurrence: []";
    } else {
      out << "cooccurrence: [";
      size_t pending_items = msg.cooccurrence.size();
      for (auto item : msg.cooccurrence) {
        rosidl_generator_traits::value_to_yaml(item, out);
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
  const RecognizedObjectArray & msg,
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

  // member: objects
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.objects.size() == 0) {
      out << "objects: []\n";
    } else {
      out << "objects:\n";
      for (auto item : msg.objects) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: cooccurrence
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.cooccurrence.size() == 0) {
      out << "cooccurrence: []\n";
    } else {
      out << "cooccurrence:\n";
      for (auto item : msg.cooccurrence) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RecognizedObjectArray & msg, bool use_flow_style = false)
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
  const object_recognition_msgs::msg::RecognizedObjectArray & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::msg::RecognizedObjectArray & msg)
{
  return object_recognition_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::msg::RecognizedObjectArray>()
{
  return "object_recognition_msgs::msg::RecognizedObjectArray";
}

template<>
inline const char * name<object_recognition_msgs::msg::RecognizedObjectArray>()
{
  return "object_recognition_msgs/msg/RecognizedObjectArray";
}

template<>
struct has_fixed_size<object_recognition_msgs::msg::RecognizedObjectArray>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::msg::RecognizedObjectArray>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<object_recognition_msgs::msg::RecognizedObjectArray>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__TRAITS_HPP_
