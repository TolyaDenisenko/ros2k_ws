#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to object_recognition_msgs__srv__GetObjectInformation_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetObjectInformation_Request {
    /// The type of the object to retrieve info from
    pub type_: super::msg::ObjectType,

}



impl Default for GetObjectInformation_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetObjectInformation_Request::default())
  }
}

impl rosidl_runtime_rs::Message for GetObjectInformation_Request {
  type RmwMsg = super::srv::rmw::GetObjectInformation_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        type_: super::msg::ObjectType::into_rmw_message(std::borrow::Cow::Owned(msg.type_)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        type_: super::msg::ObjectType::into_rmw_message(std::borrow::Cow::Borrowed(&msg.type_)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      type_: super::msg::ObjectType::from_rmw_message(msg.type_),
    }
  }
}


// Corresponds to object_recognition_msgs__srv__GetObjectInformation_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GetObjectInformation_Response {
    /// Extra object info
    pub information: super::msg::ObjectInformation,

}



impl Default for GetObjectInformation_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::GetObjectInformation_Response::default())
  }
}

impl rosidl_runtime_rs::Message for GetObjectInformation_Response {
  type RmwMsg = super::srv::rmw::GetObjectInformation_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        information: super::msg::ObjectInformation::into_rmw_message(std::borrow::Cow::Owned(msg.information)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        information: super::msg::ObjectInformation::into_rmw_message(std::borrow::Cow::Borrowed(&msg.information)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      information: super::msg::ObjectInformation::from_rmw_message(msg.information),
    }
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


