// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/TableArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/table_array.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/table_array__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_TableArray_tables
{
public:
  explicit Init_TableArray_tables(::object_recognition_msgs::msg::TableArray & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::TableArray tables(::object_recognition_msgs::msg::TableArray::_tables_type arg)
  {
    msg_.tables = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::TableArray msg_;
};

class Init_TableArray_header
{
public:
  Init_TableArray_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TableArray_tables header(::object_recognition_msgs::msg::TableArray::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_TableArray_tables(msg_);
  }

private:
  ::object_recognition_msgs::msg::TableArray msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::TableArray>()
{
  return object_recognition_msgs::msg::builder::Init_TableArray_header();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__TABLE_ARRAY__BUILDER_HPP_
