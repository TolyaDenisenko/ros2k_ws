// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/Table.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/table.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/table__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_Table_convex_hull
{
public:
  explicit Init_Table_convex_hull(::object_recognition_msgs::msg::Table & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::Table convex_hull(::object_recognition_msgs::msg::Table::_convex_hull_type arg)
  {
    msg_.convex_hull = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::Table msg_;
};

class Init_Table_pose
{
public:
  explicit Init_Table_pose(::object_recognition_msgs::msg::Table & msg)
  : msg_(msg)
  {}
  Init_Table_convex_hull pose(::object_recognition_msgs::msg::Table::_pose_type arg)
  {
    msg_.pose = std::move(arg);
    return Init_Table_convex_hull(msg_);
  }

private:
  ::object_recognition_msgs::msg::Table msg_;
};

class Init_Table_header
{
public:
  Init_Table_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Table_pose header(::object_recognition_msgs::msg::Table::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Table_pose(msg_);
  }

private:
  ::object_recognition_msgs::msg::Table msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::Table>()
{
  return object_recognition_msgs::msg::builder::Init_Table_header();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE__BUILDER_HPP_
