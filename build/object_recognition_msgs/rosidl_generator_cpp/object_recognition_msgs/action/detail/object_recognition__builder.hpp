// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from object_recognition_msgs:action/ObjectRecognition.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/action/object_recognition.hpp"


#ifndef OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__BUILDER_HPP_
#define OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "object_recognition_msgs/action/detail/object_recognition__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_Goal_filter_limits
{
public:
  explicit Init_ObjectRecognition_Goal_filter_limits(::object_recognition_msgs::action::ObjectRecognition_Goal & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_Goal filter_limits(::object_recognition_msgs::action::ObjectRecognition_Goal::_filter_limits_type arg)
  {
    msg_.filter_limits = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_Goal msg_;
};

class Init_ObjectRecognition_Goal_use_roi
{
public:
  Init_ObjectRecognition_Goal_use_roi()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_Goal_filter_limits use_roi(::object_recognition_msgs::action::ObjectRecognition_Goal::_use_roi_type arg)
  {
    msg_.use_roi = std::move(arg);
    return Init_ObjectRecognition_Goal_filter_limits(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_Goal msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_Goal>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_Goal_use_roi();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_Result_recognized_objects
{
public:
  Init_ObjectRecognition_Result_recognized_objects()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_Result recognized_objects(::object_recognition_msgs::action::ObjectRecognition_Result::_recognized_objects_type arg)
  {
    msg_.recognized_objects = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_Result msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_Result>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_Result_recognized_objects();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{


}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_Feedback>()
{
  return ::object_recognition_msgs::action::ObjectRecognition_Feedback(rosidl_runtime_cpp::MessageInitialization::ZERO);
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_SendGoal_Request_goal
{
public:
  explicit Init_ObjectRecognition_SendGoal_Request_goal(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request goal(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request msg_;
};

class Init_ObjectRecognition_SendGoal_Request_goal_id
{
public:
  Init_ObjectRecognition_SendGoal_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_SendGoal_Request_goal goal_id(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_ObjectRecognition_SendGoal_Request_goal(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_SendGoal_Request>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_SendGoal_Request_goal_id();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_SendGoal_Response_stamp
{
public:
  explicit Init_ObjectRecognition_SendGoal_Response_stamp(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response stamp(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response msg_;
};

class Init_ObjectRecognition_SendGoal_Response_accepted
{
public:
  Init_ObjectRecognition_SendGoal_Response_accepted()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_SendGoal_Response_stamp accepted(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response::_accepted_type arg)
  {
    msg_.accepted = std::move(arg);
    return Init_ObjectRecognition_SendGoal_Response_stamp(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_SendGoal_Response>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_SendGoal_Response_accepted();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_SendGoal_Event_response
{
public:
  explicit Init_ObjectRecognition_SendGoal_Event_response(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event response(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event msg_;
};

class Init_ObjectRecognition_SendGoal_Event_request
{
public:
  explicit Init_ObjectRecognition_SendGoal_Event_request(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event & msg)
  : msg_(msg)
  {}
  Init_ObjectRecognition_SendGoal_Event_response request(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_ObjectRecognition_SendGoal_Event_response(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event msg_;
};

class Init_ObjectRecognition_SendGoal_Event_info
{
public:
  Init_ObjectRecognition_SendGoal_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_SendGoal_Event_request info(::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_ObjectRecognition_SendGoal_Event_request(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_SendGoal_Event>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_SendGoal_Event_info();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_GetResult_Request_goal_id
{
public:
  Init_ObjectRecognition_GetResult_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Request goal_id(::object_recognition_msgs::action::ObjectRecognition_GetResult_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_GetResult_Request>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_GetResult_Request_goal_id();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_GetResult_Response_result
{
public:
  explicit Init_ObjectRecognition_GetResult_Response_result(::object_recognition_msgs::action::ObjectRecognition_GetResult_Response & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Response result(::object_recognition_msgs::action::ObjectRecognition_GetResult_Response::_result_type arg)
  {
    msg_.result = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Response msg_;
};

class Init_ObjectRecognition_GetResult_Response_status
{
public:
  Init_ObjectRecognition_GetResult_Response_status()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_GetResult_Response_result status(::object_recognition_msgs::action::ObjectRecognition_GetResult_Response::_status_type arg)
  {
    msg_.status = std::move(arg);
    return Init_ObjectRecognition_GetResult_Response_result(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_GetResult_Response>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_GetResult_Response_status();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_GetResult_Event_response
{
public:
  explicit Init_ObjectRecognition_GetResult_Event_response(::object_recognition_msgs::action::ObjectRecognition_GetResult_Event & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Event response(::object_recognition_msgs::action::ObjectRecognition_GetResult_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Event msg_;
};

class Init_ObjectRecognition_GetResult_Event_request
{
public:
  explicit Init_ObjectRecognition_GetResult_Event_request(::object_recognition_msgs::action::ObjectRecognition_GetResult_Event & msg)
  : msg_(msg)
  {}
  Init_ObjectRecognition_GetResult_Event_response request(::object_recognition_msgs::action::ObjectRecognition_GetResult_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_ObjectRecognition_GetResult_Event_response(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Event msg_;
};

class Init_ObjectRecognition_GetResult_Event_info
{
public:
  Init_ObjectRecognition_GetResult_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_GetResult_Event_request info(::object_recognition_msgs::action::ObjectRecognition_GetResult_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_ObjectRecognition_GetResult_Event_request(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_GetResult_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_GetResult_Event>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_GetResult_Event_info();
}

}  // namespace object_recognition_msgs


namespace object_recognition_msgs
{

namespace action
{

namespace builder
{

class Init_ObjectRecognition_FeedbackMessage_feedback
{
public:
  explicit Init_ObjectRecognition_FeedbackMessage_feedback(::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage & msg)
  : msg_(msg)
  {}
  ::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage feedback(::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage::_feedback_type arg)
  {
    msg_.feedback = std::move(arg);
    return std::move(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage msg_;
};

class Init_ObjectRecognition_FeedbackMessage_goal_id
{
public:
  Init_ObjectRecognition_FeedbackMessage_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ObjectRecognition_FeedbackMessage_feedback goal_id(::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_ObjectRecognition_FeedbackMessage_feedback(msg_);
  }

private:
  ::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::object_recognition_msgs::action::ObjectRecognition_FeedbackMessage>()
{
  return object_recognition_msgs::action::builder::Init_ObjectRecognition_FeedbackMessage_goal_id();
}

}  // namespace object_recognition_msgs

#endif  // OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__BUILDER_HPP_
