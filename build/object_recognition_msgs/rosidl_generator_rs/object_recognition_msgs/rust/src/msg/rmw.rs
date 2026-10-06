#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__ObjectInformation() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__ObjectInformation__init(msg: *mut ObjectInformation) -> bool;
    fn object_recognition_msgs__msg__ObjectInformation__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectInformation>, size: usize) -> bool;
    fn object_recognition_msgs__msg__ObjectInformation__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectInformation>);
    fn object_recognition_msgs__msg__ObjectInformation__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectInformation>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectInformation>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__ObjectInformation
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// VISUALIZATION INFO ######################################################
/// THIS INFO SHOULD BE OBTAINED INDEPENDENTLY FROM THE CORE, LIKE IN AN RVIZ PLUGIN ###################

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectInformation {
    /// The human readable name of the object
    pub name: rosidl_runtime_rs::String,

    /// The full mesh of the object: this can be useful for display purposes, augmented reality ... but it can be big
    /// Make sure the type is MESH
    pub ground_truth_mesh: shape_msgs::msg::rmw::Mesh,

    /// Sometimes, you only have a cloud in the DB
    /// Make sure the type is POINTS
    pub ground_truth_point_cloud: sensor_msgs::msg::rmw::PointCloud2,

}



impl Default for ObjectInformation {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__ObjectInformation__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__ObjectInformation__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectInformation {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectInformation__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectInformation__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectInformation__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectInformation {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectInformation where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/ObjectInformation";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__ObjectInformation() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__ObjectType() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__ObjectType__init(msg: *mut ObjectType) -> bool;
    fn object_recognition_msgs__msg__ObjectType__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ObjectType>, size: usize) -> bool;
    fn object_recognition_msgs__msg__ObjectType__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ObjectType>);
    fn object_recognition_msgs__msg__ObjectType__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ObjectType>, out_seq: *mut rosidl_runtime_rs::Sequence<ObjectType>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__ObjectType
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// OBJECT ID #########################################################

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectType {
    /// Contains information about the type of a found object. Those two sets of parameters together uniquely define an
    /// object
    /// The key of the found object: the unique identifier in the given db
    pub key: rosidl_runtime_rs::String,

    /// The db parameters stored as a JSON/compressed YAML string. An object id does not make sense without the corresponding
    /// database. E.g., in object_recognition, it can look like: "{'type':'CouchDB', 'root':'http://localhost'}"
    /// There is no conventional format for those parameters and it's nice to keep that flexibility.
    /// The object_recognition_core as a generic DB type that can read those fields
    /// Current examples:
    /// For CouchDB:
    ///   type: 'CouchDB'
    ///   root: 'http://localhost:5984'
    ///   collection: 'object_recognition'
    /// For SQL household database:
    ///   type: 'SqlHousehold'
    ///   host: 'wgs36'
    ///   port: 5432
    ///   user: 'willow'
    ///   password: 'willow'
    ///   name: 'household_objects'
    ///   module: 'tabletop'
    pub db: rosidl_runtime_rs::String,

}



impl Default for ObjectType {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__ObjectType__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__ObjectType__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ObjectType {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectType__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectType__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__ObjectType__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ObjectType {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ObjectType where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/ObjectType";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__ObjectType() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__RecognizedObject() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__RecognizedObject__init(msg: *mut RecognizedObject) -> bool;
    fn object_recognition_msgs__msg__RecognizedObject__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RecognizedObject>, size: usize) -> bool;
    fn object_recognition_msgs__msg__RecognizedObject__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RecognizedObject>);
    fn object_recognition_msgs__msg__RecognizedObject__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RecognizedObject>, out_seq: *mut rosidl_runtime_rs::Sequence<RecognizedObject>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__RecognizedObject
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// HEADER ###########################################################

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RecognizedObject {
    /// The header frame corresponds to the pose frame, NOT the point_cloud frame.
    pub header: std_msgs::msg::rmw::Header,

    /// OBJECT INFO #########################################################
    /// Contains information about the type and the position of a found object
    /// Some of those fields might not be filled because the used techniques do not fill them or because the user does not
    /// request them
    /// The type of the found object
    pub type_: super::super::msg::rmw::ObjectType,

    /// confidence: how sure you are it is that object and not another one.
    ///  It is between 0 and 1 and the closer to one it is the better
    pub confidence: f32,

    /// OBJECT CLUSTERS #######################################################
    /// Sometimes you can extract the 3d points that belong to the object, in the frames of the original sensors
    /// (it is an array as you might have several sensors)
    pub point_clouds: rosidl_runtime_rs::Sequence<sensor_msgs::msg::rmw::PointCloud2>,

    /// Sometimes, you can only provide a bounding box/shape, even in 3d
    /// This is in the pose frame
    pub bounding_mesh: shape_msgs::msg::rmw::Mesh,

    /// Sometimes, you only have 2d input so you can't really give a pose, you just get a contour, or a box
    /// The last point will be linked to the first one automatically
    pub bounding_contours: rosidl_runtime_rs::Sequence<geometry_msgs::msg::rmw::Point>,

    /// POSE INFO #########################################################
    /// This is the result that everybody expects : the pose in some frame given with the input. The units are radian/meters
    /// as usual
    pub pose: geometry_msgs::msg::rmw::PoseWithCovarianceStamped,

}



impl Default for RecognizedObject {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__RecognizedObject__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__RecognizedObject__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RecognizedObject {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObject__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObject__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObject__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RecognizedObject {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RecognizedObject where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/RecognizedObject";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__RecognizedObject() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__RecognizedObjectArray() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__RecognizedObjectArray__init(msg: *mut RecognizedObjectArray) -> bool;
    fn object_recognition_msgs__msg__RecognizedObjectArray__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RecognizedObjectArray>, size: usize) -> bool;
    fn object_recognition_msgs__msg__RecognizedObjectArray__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RecognizedObjectArray>);
    fn object_recognition_msgs__msg__RecognizedObjectArray__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RecognizedObjectArray>, out_seq: *mut rosidl_runtime_rs::Sequence<RecognizedObjectArray>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__RecognizedObjectArray
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// HEADER ###########################################################

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RecognizedObjectArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// This message type describes a potential scene configuration: a set of objects that can explain the scene
    pub objects: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RecognizedObject>,

    /// SEARCH ###########################################################
    /// The co-occurrence matrix between the recognized objects
    pub cooccurrence: rosidl_runtime_rs::Sequence<f32>,

}



impl Default for RecognizedObjectArray {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__RecognizedObjectArray__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__RecognizedObjectArray__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RecognizedObjectArray {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObjectArray__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObjectArray__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__RecognizedObjectArray__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RecognizedObjectArray {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RecognizedObjectArray where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/RecognizedObjectArray";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__RecognizedObjectArray() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__Table() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__Table__init(msg: *mut Table) -> bool;
    fn object_recognition_msgs__msg__Table__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Table>, size: usize) -> bool;
    fn object_recognition_msgs__msg__Table__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Table>);
    fn object_recognition_msgs__msg__Table__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Table>, out_seq: *mut rosidl_runtime_rs::Sequence<Table>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__Table
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Informs that a planar table has been detected at a given location

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Table {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// The pose gives you the transform that take you to the coordinate system
    /// of the table, with the origin somewhere in the table plane and the
    /// z axis normal to the plane
    pub pose: geometry_msgs::msg::rmw::Pose,

    /// There is no guarantee that the table does NOT extend further than the
    /// convex hull; this is just as far as we've observed it.
    /// The origin of the table coordinate system is inside the convex hull
    /// Set of points forming the convex hull of the table
    pub convex_hull: rosidl_runtime_rs::Sequence<geometry_msgs::msg::rmw::Point>,

}



impl Default for Table {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__Table__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__Table__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Table {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__Table__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__Table__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__Table__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Table {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Table where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/Table";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__Table() }
  }
}


#[link(name = "object_recognition_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__TableArray() -> *const std::ffi::c_void;
}

#[link(name = "object_recognition_msgs__rosidl_generator_c")]
extern "C" {
    fn object_recognition_msgs__msg__TableArray__init(msg: *mut TableArray) -> bool;
    fn object_recognition_msgs__msg__TableArray__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<TableArray>, size: usize) -> bool;
    fn object_recognition_msgs__msg__TableArray__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<TableArray>);
    fn object_recognition_msgs__msg__TableArray__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<TableArray>, out_seq: *mut rosidl_runtime_rs::Sequence<TableArray>) -> bool;
}

// Corresponds to object_recognition_msgs__msg__TableArray
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TableArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// Just an array of tables
    pub tables: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Table>,

}



impl Default for TableArray {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !object_recognition_msgs__msg__TableArray__init(&mut msg as *mut _) {
        panic!("Call to object_recognition_msgs__msg__TableArray__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for TableArray {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__TableArray__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__TableArray__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { object_recognition_msgs__msg__TableArray__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for TableArray {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for TableArray where Self: Sized {
  const TYPE_NAME: &'static str = "object_recognition_msgs/msg/TableArray";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__object_recognition_msgs__msg__TableArray() }
  }
}


