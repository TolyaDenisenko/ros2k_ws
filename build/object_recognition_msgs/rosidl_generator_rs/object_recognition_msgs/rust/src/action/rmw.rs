
#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Goal() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_Goal__init(msg: *mut ObjectRecognition_Goal) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Goal>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Goal>);
    fn object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_Goal>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Goal>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_Goal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub use_roi: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub filter_limits: rosidl_runtime_rs::Sequence<f32>,

}



impl Default for ObjectRecognition_Goal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_Goal__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_Goal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_Goal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Goal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_Goal where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_Goal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Goal() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Result() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_Result__init(msg: *mut ObjectRecognition_Result) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Result>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Result>);
    fn object_recognition_msgs__action__ObjectRecognition_Result__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_Result>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Result>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_Result
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Result {

    // This member is not documented.
    #[allow(missing_docs)]
    pub recognized_objects: super::super::msg::rmw::RecognizedObjectArray,

}



impl Default for ObjectRecognition_Result {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_Result__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_Result__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_Result {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Result__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Result {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_Result where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_Result";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Result() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Feedback() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_Feedback__init(msg: *mut ObjectRecognition_Feedback) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Feedback>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Feedback>);
    fn object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_Feedback>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_Feedback>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_Feedback
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_Feedback {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for ObjectRecognition_Feedback {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_Feedback__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_Feedback__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_Feedback {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_Feedback {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_Feedback where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_Feedback";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_Feedback() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_FeedbackMessage() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(msg: *mut ObjectRecognition_FeedbackMessage) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_FeedbackMessage>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_FeedbackMessage>);
    fn object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_FeedbackMessage>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_FeedbackMessage>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_FeedbackMessage
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_FeedbackMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub feedback: super::super::action::rmw::ObjectRecognition_Feedback,

}



impl Default for ObjectRecognition_FeedbackMessage {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_FeedbackMessage {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_FeedbackMessage {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_FeedbackMessage where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_FeedbackMessage";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_FeedbackMessage() }
  }
}




#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal_Request() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(msg: *mut ObjectRecognition_SendGoal_Request) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Request>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Request>);
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Request>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_SendGoal_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_SendGoal_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: super::super::action::rmw::ObjectRecognition_Goal,

}



impl Default for ObjectRecognition_SendGoal_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_SendGoal_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_SendGoal_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_SendGoal_Request where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_SendGoal_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal_Request() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal_Response() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(msg: *mut ObjectRecognition_SendGoal_Response) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Response>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Response>);
    fn object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_SendGoal_Response>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_SendGoal_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_SendGoal_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub accepted: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}



impl Default for ObjectRecognition_SendGoal_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_SendGoal_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_SendGoal_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_SendGoal_Response where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_SendGoal_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_SendGoal_Response() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult_Request() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(msg: *mut ObjectRecognition_GetResult_Request) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Request>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Request>);
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Request>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_GetResult_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_GetResult_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,

}



impl Default for ObjectRecognition_GetResult_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_GetResult_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_GetResult_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_GetResult_Request where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_GetResult_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult_Request() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult_Response() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(msg: *mut ObjectRecognition_GetResult_Response) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Response>, size: usize) -> bool;
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Response>);
    fn object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectRecognition_GetResult_Response>) -> bool;
}

// Corresponds to object_recognition_msgs__action__ObjectRecognition_GetResult_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectRecognition_GetResult_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub result: super::super::action::rmw::ObjectRecognition_Result,

}



impl Default for ObjectRecognition_GetResult_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectRecognition_GetResult_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectRecognition_GetResult_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectRecognition_GetResult_Response where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/action/ObjectRecognition_GetResult_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__action__ObjectRecognition_GetResult_Response() }
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


