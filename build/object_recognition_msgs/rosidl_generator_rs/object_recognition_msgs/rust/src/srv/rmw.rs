#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__srv__GetObjectInformation_Request() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__srv__GetObjectInformation_Request__init(msg: *mut GetObjectInformation_Request) -> bool;
    fn object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Request>, size: usize) -> bool;
    fn object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Request>);
    fn object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetObjectInformation_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Request>) -> bool;
}

// Corresponds to object_recognition_msgs__srv__GetObjectInformation_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetObjectInformation_Request {
    /// The type of the object to retrieve info from
    pub type_: super::super::msg::rmw::ObjectType,

}



impl Default for GetObjectInformation_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__srv__GetObjectInformation_Request__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__srv__GetObjectInformation_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetObjectInformation_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetObjectInformation_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetObjectInformation_Request where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/srv/GetObjectInformation_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__srv__GetObjectInformation_Request() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__srv__GetObjectInformation_Response() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__srv__GetObjectInformation_Response__init(msg: *mut GetObjectInformation_Response) -> bool;
    fn object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Response>, size: usize) -> bool;
    fn object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Response>);
    fn object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GetObjectInformation_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<GetObjectInformation_Response>) -> bool;
}

// Corresponds to object_recognition_msgs__srv__GetObjectInformation_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetObjectInformation_Response {
    /// Extra object info
    pub information: super::super::msg::rmw::ObjectInformation,

}



impl Default for GetObjectInformation_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__srv__GetObjectInformation_Response__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__srv__GetObjectInformation_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GetObjectInformation_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GetObjectInformation_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GetObjectInformation_Response where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/srv/GetObjectInformation_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__srv__GetObjectInformation_Response() }
  }
}






#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__srv__GetObjectInformation() -> *const std::ffi::c_void;
}

// Corresponds to object_recognition_msgs__srv__GetObjectInformation
#[allow(missing_docs, non_camel_case_types)]
pub struct GetObjectInformation;

impl rosidl_runtime_rs::Service for GetObjectInformation {
    type Request = GetObjectInformation_Request;
    type Response = GetObjectInformation_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__object_recognition_msgs__srv__GetObjectInformation() }
    }
}


