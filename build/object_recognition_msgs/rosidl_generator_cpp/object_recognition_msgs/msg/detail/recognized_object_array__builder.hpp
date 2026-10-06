// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object_array.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/recognized_object_array__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_RecognizedObjectArray_cooccurrence
{
public:
  explicit Init_RecognizedObjectArray_cooccurrence(::object_recognition_msgs::msg::RecognizedObjectArray & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::RecognizedObjectArray cooccurrence(::object_recognition_msgs::msg::RecognizedObjectArray::_cooccurrence_type arg)
  {
    msg_.cooccurrence = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObjectArray msg_;
};

class Init_RecognizedObjectArray_objects
{
public:
  explicit Init_RecognizedObjectArray_objects(::object_recognition_msgs::msg::RecognizedObjectArray & msg)
  : msg_(msg)
  {}
  Init_RecognizedObjectArray_cooccurrence objects(::object_recognition_msgs::msg::RecognizedObjectArray::_objects_type arg)
  {
    msg_.objects = std::move(arg);
    return Init_RecognizedObjectArray_cooccurrence(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObjectArray msg_;
};

class Init_RecognizedObjectArray_header
{
public:
  Init_RecognizedObjectArray_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RecognizedObjectArray_objects header(::object_recognition_msgs::msg::RecognizedObjectArray::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_RecognizedObjectArray_objects(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObjectArray msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::RecognizedObjectArray>()
{
  return object_recognition_msgs::msg::builder::Init_RecognizedObjectArray_header();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT_ARRAY__BUILDER_HPP_
