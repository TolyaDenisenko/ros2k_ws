// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/ObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_information.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/object_information__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_ObjectInformation_ground_truth_point_cloud
{
public:
  explicit Init_ObjectInformation_ground_truth_point_cloud(::object_recognition_msgs::msg::ObjectInformation & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::ObjectInformation ground_truth_point_cloud(::object_recognition_msgs::msg::ObjectInformation::_ground_truth_point_cloud_type arg)
  {
    msg_.ground_truth_point_cloud = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::ObjectInformation msg_;
};

class Init_ObjectInformation_ground_truth_mesh
{
public:
  explicit Init_ObjectInformation_ground_truth_mesh(::object_recognition_msgs::msg::ObjectInformation & msg)
  : msg_(msg)
  {}
  Init_ObjectInformation_ground_truth_point_cloud ground_truth_mesh(::object_recognition_msgs::msg::ObjectInformation::_ground_truth_mesh_type arg)
  {
    msg_.ground_truth_mesh = std::move(arg);
    return Init_ObjectInformation_ground_truth_point_cloud(msg_);
  }

private:
  ::object_recognition_msgs::msg::ObjectInformation msg_;
};

class Init_ObjectInformation_name
{
public:
  Init_ObjectInformation_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectInformation_ground_truth_mesh name(::object_recognition_msgs::msg::ObjectInformation::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_ObjectInformation_ground_truth_mesh(msg_);
  }

private:
  ::object_recognition_msgs::msg::ObjectInformation msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::ObjectInformation>()
{
  return object_recognition_msgs::msg::builder::Init_ObjectInformation_name();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_INFORMATION__BUILDER_HPP_
