// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:msg/ObjectType.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_type.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/msg/detail/object_type__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace msg
{

namespace builder
{

class Init_ObjectType_db
{
public:
  explicit Init_ObjectType_db(::object_recognition_msgs::msg::ObjectType & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::msg::ObjectType db(::object_recognition_msgs::msg::ObjectType::_db_type arg)
  {
    msg_.db = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::msg::ObjectType msg_;
};

class Init_ObjectType_key
{
public:
  Init_ObjectType_key()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectType_db key(::object_recognition_msgs::msg::ObjectType::_key_type arg)
  {
    msg_.key = std::move(arg);
    return Init_ObjectType_db(msg_);
  }

private:
  ::object_recognition_msgs::msg::ObjectType msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::msg::ObjectType>()
{
  return object_recognition_msgs::msg::builder::Init_ObjectType_key();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__BUILDER_HPP_
