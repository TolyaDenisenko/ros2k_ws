// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from object_recognition_msgs:srv/GetObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/srv/get_object_information.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__TRAITS_HPP_
#define OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "object_recognition_msgs/srv/detail/get_object_information__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'type'
#include "object_recognition_msgs/msg/detail/object_type__traits.hpp"

namespace object_recognition_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetObjectInformation_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: type
  {
    out << "type: ";
    to_flow_style_yaml(msg.type, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GetObjectInformation_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: type
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "type:\n";
    to_block_style_yaml(msg.type, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetObjectInformation_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace object_recognition_msgs

namespace rosidl_generator_traits
{

[[deprecated("use object_recognition_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const object_recognition_msgs::srv::GetObjectInformation_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::srv::GetObjectInformation_Request & msg)
{
  return object_recognition_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::srv::GetObjectInformation_Request>()
{
  return "object_recognition_msgs::srv::GetObjectInformation_Request";
}

template<>
inline const char * name<object_recognition_msgs::srv::GetObjectInformation_Request>()
{
  return "object_recognition_msgs/srv/GetObjectInformation_Request";
}

template<>
struct has_fixed_size<object_recognition_msgs::srv::GetObjectInformation_Request>
  : std::integral_constant<bool, has_fixed_size<object_recognition_msgs::msg::ObjectType>::value> {};

template<>
struct has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Request>
  : std::integral_constant<bool, has_bounded_size<object_recognition_msgs::msg::ObjectType>::value> {};

template<>
struct is_message<object_recognition_msgs::srv::GetObjectInformation_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'information'
#include "object_recognition_msgs/msg/detail/object_information__traits.hpp"

namespace object_recognition_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetObjectInformation_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: information
  {
    out << "information: ";
    to_flow_style_yaml(msg.information, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GetObjectInformation_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: information
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "information:\n";
    to_block_style_yaml(msg.information, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetObjectInformation_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace object_recognition_msgs

namespace rosidl_generator_traits
{

[[deprecated("use object_recognition_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const object_recognition_msgs::srv::GetObjectInformation_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::srv::GetObjectInformation_Response & msg)
{
  return object_recognition_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::srv::GetObjectInformation_Response>()
{
  return "object_recognition_msgs::srv::GetObjectInformation_Response";
}

template<>
inline const char * name<object_recognition_msgs::srv::GetObjectInformation_Response>()
{
  return "object_recognition_msgs/srv/GetObjectInformation_Response";
}

template<>
struct has_fixed_size<object_recognition_msgs::srv::GetObjectInformation_Response>
  : std::integral_constant<bool, has_fixed_size<object_recognition_msgs::msg::ObjectInformation>::value> {};

template<>
struct has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Response>
  : std::integral_constant<bool, has_bounded_size<object_recognition_msgs::msg::ObjectInformation>::value> {};

template<>
struct is_message<object_recognition_msgs::srv::GetObjectInformation_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace object_recognition_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const GetObjectInformation_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
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
  const GetObjectInformation_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GetObjectInformation_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace object_recognition_msgs

namespace rosidl_generator_traits
{

[[deprecated("use object_recognition_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const object_recognition_msgs::srv::GetObjectInformation_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  object_recognition_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use object_recognition_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const object_recognition_msgs::srv::GetObjectInformation_Event & msg)
{
  return object_recognition_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<object_recognition_msgs::srv::GetObjectInformation_Event>()
{
  return "object_recognition_msgs::srv::GetObjectInformation_Event";
}

template<>
inline const char * name<object_recognition_msgs::srv::GetObjectInformation_Event>()
{
  return "object_recognition_msgs/srv/GetObjectInformation_Event";
}

template<>
struct has_fixed_size<object_recognition_msgs::srv::GetObjectInformation_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Event>
  : std::integral_constant<bool, has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Request>::value && has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<object_recognition_msgs::srv::GetObjectInformation_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<object_recognition_msgs::srv::GetObjectInformation>()
{
  return "object_recognition_msgs::srv::GetObjectInformation";
}

template<>
inline const char * name<object_recognition_msgs::srv::GetObjectInformation>()
{
  return "object_recognition_msgs/srv/GetObjectInformation";
}

template<>
struct has_fixed_size<object_recognition_msgs::srv::GetObjectInformation>
  : std::integral_constant<
    bool,
    has_fixed_size<object_recognition_msgs::srv::GetObjectInformation_Request>::value &&
    has_fixed_size<object_recognition_msgs::srv::GetObjectInformation_Response>::value
  >
{
};

template<>
struct has_bounded_size<object_recognition_msgs::srv::GetObjectInformation>
  : std::integral_constant<
    bool,
    has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Request>::value &&
    has_bounded_size<object_recognition_msgs::srv::GetObjectInformation_Response>::value
  >
{
};

template<>
struct is_service<object_recognition_msgs::srv::GetObjectInformation>
  : std::true_type
{
};

template<>
struct is_service_request<object_recognition_msgs::srv::GetObjectInformation_Request>
  : std::true_type
{
};

template<>
struct is_service_response<object_recognition_msgs::srv::GetObjectInformation_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__TRAITS_HPP_
