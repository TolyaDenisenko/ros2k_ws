// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/RecognizedObject.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/recognized_object.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/recognized_object__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_RecognizedObject_pose
{
public:
  explicit Init_RecognizedObject_pose(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::RecognizedObject pose(::object_recognition_msgs::msg::RecognizedObject::_pose_type arg)
  {
    msg_.pose = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_bounding_contours
{
public:
  explicit Init_RecognizedObject_bounding_contours(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  Init_RecognizedObject_pose bounding_contours(::object_recognition_msgs::msg::RecognizedObject::_bounding_contours_type arg)
  {
    msg_.bounding_contours = std::move(arg);
    return Init_RecognizedObject_pose(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_bounding_mesh
{
public:
  explicit Init_RecognizedObject_bounding_mesh(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  Init_RecognizedObject_bounding_contours bounding_mesh(::object_recognition_msgs::msg::RecognizedObject::_bounding_mesh_type arg)
  {
    msg_.bounding_mesh = std::move(arg);
    return Init_RecognizedObject_bounding_contours(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_point_clouds
{
public:
  explicit Init_RecognizedObject_point_clouds(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  Init_RecognizedObject_bounding_mesh point_clouds(::object_recognition_msgs::msg::RecognizedObject::_point_clouds_type arg)
  {
    msg_.point_clouds = std::move(arg);
    return Init_RecognizedObject_bounding_mesh(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_confidence
{
public:
  explicit Init_RecognizedObject_confidence(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  Init_RecognizedObject_point_clouds confidence(::object_recognition_msgs::msg::RecognizedObject::_confidence_type arg)
  {
    msg_.confidence = std::move(arg);
    return Init_RecognizedObject_point_clouds(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_type
{
public:
  explicit Init_RecognizedObject_type(::object_recognition_msgs::msg::RecognizedObject & msg)
  : msg_(msg)
  {}
  Init_RecognizedObject_confidence type(::object_recognition_msgs::msg::RecognizedObject::_type_type arg)
  {
    msg_.type = std::move(arg);
    return Init_RecognizedObject_confidence(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

class Init_RecognizedObject_header
{
public:
  Init_RecognizedObject_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RecognizedObject_type header(::object_recognition_msgs::msg::RecognizedObject::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_RecognizedObject_type(msg_);
  }

private:
  ::object_recognition_msgs::msg::RecognizedObject msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::RecognizedObject>()
{
  return object_recognition_msgs::msg::builder::Init_RecognizedObject_header();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__RECOGNIZED_OBJECT__BUILDER_HPP_
