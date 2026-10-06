
#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to object_recognition_msgs__action__ObjectRecognition_Goal

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub use_roi: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub filter_limits: Vec<f32>,

}



impl Default for ObjectRecognition_Goal {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_Goal::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Goal {
  type RmwMsg = super::action::rmw::ObjectRecognition_Goal;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        use_roi: msg.use_roi,
        filter_limits: msg.filter_limits.as_slice().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      use_roi: msg.use_roi,
        filter_limits: msg.filter_limits.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      use_roi: msg.use_roi,
      filter_limits: msg.filter_limits.into(),
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_Result

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Result {

    // This member is not documented.
    #[allow(missing_docs)]
    pub recognized_objects: super::msg::RecognizedObjectArray,

}



impl Default for ObjectRecognition_Result {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_Result::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Result {
  type RmwMsg = super::action::rmw::ObjectRecognition_Result;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        recognized_objects: super::msg::RecognizedObjectArray::into_rmw_message(std::borrow::Cow::Owned(msg.recognized_objects)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        recognized_objects: super::msg::RecognizedObjectArray::into_rmw_message(std::borrow::Cow::Borrowed(&msg.recognized_objects)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      recognized_objects: super::msg::RecognizedObjectArray::from_rmw_message(msg.recognized_objects),
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_Feedback

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Feedback {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for ObjectRecognition_Feedback {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_Feedback::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Feedback {
  type RmwMsg = super::action::rmw::ObjectRecognition_Feedback;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_FeedbackMessage

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_FeedbackMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub feedback: super::action::ObjectRecognition_Feedback,

}



impl Default for ObjectRecognition_FeedbackMessage {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_FeedbackMessage::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_FeedbackMessage {
  type RmwMsg = super::action::rmw::ObjectRecognition_FeedbackMessage;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
        feedback: super::action::ObjectRecognition_Feedback::into_rmw_message(std::borrow::Cow::Owned(msg.feedback)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
        feedback: super::action::ObjectRecognition_Feedback::into_rmw_message(std::borrow::Cow::Borrowed(&msg.feedback)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
      feedback: super::action::ObjectRecognition_Feedback::from_rmw_message(msg.feedback),
    }
  }
}






// Corresponds to object_recognition_msgs__action__ObjectRecognition_SendGoal_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_SendGoal_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: super::action::ObjectRecognition_Goal,

}



impl Default for ObjectRecognition_SendGoal_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_SendGoal_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_SendGoal_Request {
  type RmwMsg = super::action::rmw::ObjectRecognition_SendGoal_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
        goal: super::action::ObjectRecognition_Goal::into_rmw_message(std::borrow::Cow::Owned(msg.goal)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
        goal: super::action::ObjectRecognition_Goal::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
      goal: super::action::ObjectRecognition_Goal::from_rmw_message(msg.goal),
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_SendGoal_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_SendGoal_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub accepted: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::Time,

}



impl Default for ObjectRecognition_SendGoal_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_SendGoal_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_SendGoal_Response {
  type RmwMsg = super::action::rmw::ObjectRecognition_SendGoal_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        accepted: msg.accepted,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      accepted: msg.accepted,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      accepted: msg.accepted,
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_GetResult_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_GetResult_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,

}



impl Default for ObjectRecognition_GetResult_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_GetResult_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_GetResult_Request {
  type RmwMsg = super::action::rmw::ObjectRecognition_GetResult_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
    }
  }
}


// Corresponds to object_recognition_msgs__action__ObjectRecognition_GetResult_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_GetResult_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub result: super::action::ObjectRecognition_Result,

}



impl Default for ObjectRecognition_GetResult_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::ObjectRecognition_GetResult_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_GetResult_Response {
  type RmwMsg = super::action::rmw::ObjectRecognition_GetResult_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        status: msg.status,
        result: super::action::ObjectRecognition_Result::into_rmw_message(std::borrow::Cow::Owned(msg.result)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      status: msg.status,
        result: super::action::ObjectRecognition_Result::into_rmw_message(std::borrow::Cow::Borrowed(&msg.result)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      status: msg.status,
      result: super::action::ObjectRecognition_Result::from_rmw_message(msg.result),
    }
  }
}






#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal() -> *const std::ffi::c_void;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_SendGoal
#[allow(missing_docs, non_camel_case_types)]
pub struct ObjectRecognition_SendGoal;

impl rosidl_runtime_rs::Service for ObjectRecognition_SendGoal {
    type Request = ObjectRecognition_SendGoal_Request;
    type Response = ObjectRecognition_SendGoal_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal() }
    }
}




#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult() -> *const std::ffi::c_void;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_GetResult
#[allow(missing_docs, non_camel_case_types)]
pub struct ObjectRecognition_GetResult;

impl rosidl_runtime_rs::Service for ObjectRecognition_GetResult {
    type Request = ObjectRecognition_GetResult_Request;
    type Response = ObjectRecognition_GetResult_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult() }
    }
}






#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_action_type_support_handle__object_recognition_msgs__action__ObjectRecognition() -> *const std::ffi::c_void;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition
#[allow(missing_docs, non_camel_case_types)]
pub struct ObjectRecognition;

impl rosidl_runtime_rs::Action for ObjectRecognition {
  // --- Associated types for client library users ---
  /// The goal message defined in the action definition.
  type Goal = ObjectRecognition_Goal;

  /// The result message defined in the action definition.
  type Result = ObjectRecognition_Result;

  /// The feedback message defined in the action definition.
  type Feedback = ObjectRecognition_Feedback;

  // --- Associated types for client library implementation ---
  /// The feedback message with generic fields which wraps the feedback message.
  type FeedbackMessage = super::action::ObjectRecognition_FeedbackMessage;

  /// The send_goal service using a wrapped version of the goal message as a request.
  type SendGoalService = super::action::ObjectRecognition_SendGoal;

  /// The generic service to cancel a goal.
  type CancelGoalService = action_msgs::srv::rmw::CancelGoal;

  /// The get_result service using a wrapped version of the result message as a response.
  type GetResultService = super::action::ObjectRecognition_GetResult;

  // --- Methods for client library implementation ---
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_action_type_support_handle__object_recognition_msgs__action__ObjectRecognition() }
  }

  fn create_goal_request(
    goal_id: &[u8; 16],
    goal: super::action::rmw::ObjectRecognition_Goal,
  ) -> super::action::rmw::ObjectRecognition_SendGoal_Request {
   super::action::rmw::ObjectRecognition_SendGoal_Request {
      goal_id: unique_identifier_msgs::msg::rmw::UUID { uuid: *goal_id },
      goal,
    }
  }

  fn split_goal_request(
    request: super::action::rmw::ObjectRecognition_SendGoal_Request,
  ) -> (
    [u8; 16],
   super::action::rmw::ObjectRecognition_Goal,
  ) {
    (request.goal_id.uuid, request.goal)
  }

  fn create_goal_response(
    accepted: bool,
    stamp: (i32, u32),
  ) -> super::action::rmw::ObjectRecognition_SendGoal_Response {
   super::action::rmw::ObjectRecognition_SendGoal_Response {
      accepted,
      stamp: builtin_interfaces::msg::rmw::Time {
        sec: stamp.0,
        nanosec: stamp.1,
      },
    }
  }

  fn get_goal_response_accepted(
    response: &super::action::rmw::ObjectRecognition_SendGoal_Response,
  ) -> bool {
    response.accepted
  }

  fn get_goal_response_stamp(
    response: &super::action::rmw::ObjectRecognition_SendGoal_Response,
  ) -> (i32, u32) {
    (response.stamp.sec, response.stamp.nanosec)
  }

  fn create_feedback_message(
    goal_id: &[u8; 16],
    feedback: super::action::rmw::ObjectRecognition_Feedback,
  ) -> super::action::rmw::ObjectRecognition_FeedbackMessage {
    let mut message = super::action::rmw::ObjectRecognition_FeedbackMessage::default();
    message.goal_id.uuid = *goal_id;
    message.feedback = feedback;
    message
  }

  fn split_feedback_message(
    feedback: super::action::rmw::ObjectRecognition_FeedbackMessage,
  ) -> (
    [u8; 16],
   super::action::rmw::ObjectRecognition_Feedback,
  ) {
    (feedback.goal_id.uuid, feedback.feedback)
  }

  fn create_result_request(
    goal_id: &[u8; 16],
  ) -> super::action::rmw::ObjectRecognition_GetResult_Request {
   super::action::rmw::ObjectRecognition_GetResult_Request {
      goal_id: unique_identifier_msgs::msg::rmw::UUID { uuid: *goal_id },
    }
  }

  fn get_result_request_uuid(
    request: &super::action::rmw::ObjectRecognition_GetResult_Request,
  ) -> &[u8; 16] {
    &request.goal_id.uuid
  }

  fn create_result_response(
    status: i8,
    result: super::action::rmw::ObjectRecognition_Result,
  ) -> super::action::rmw::ObjectRecognition_GetResult_Response {
   super::action::rmw::ObjectRecognition_GetResult_Response {
      status,
      result,
    }
  }

  fn split_result_response(
    response: super::action::rmw::ObjectRecognition_GetResult_Response
  ) -> (
    i8,
   super::action::rmw::ObjectRecognition_Result,
  ) {
    (response.status, response.result)
  }
}


