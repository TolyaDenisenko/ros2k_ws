#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to object_recognition_msgs__msg__ObjectInformation
/// VISUALIZATION INFO ######################################################
/// THIS INFO SHOULD BE OBTAINED INDEPENDENTLY FROM THE CORE, LIKE IN AN RVIZ PLUGIN ###################

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectInformation {
    /// The human readable name of the object
    pub name: std::string::String,

    /// The full mesh of the object: this can be useful for display purposes, augmented reality ... but it can be big
    /// Make sure the type is MESH
    pub ground_truth_mesh: shape_msgs::msg::Mesh,

    /// Sometimes, you only have a cloud in the DB
    /// Make sure the type is POINTS
    pub ground_truth_point_cloud: sensor_msgs::msg::PointCloud2,

}



impl Default for ObjectInformation {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ObjectInformation::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectInformation {
  type RmwMsg = super::msg::rmw::ObjectInformation;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        ground_truth_mesh: shape_msgs::msg::Mesh::into_rmw_message(std::borrow::Cow::Owned(msg.ground_truth_mesh)).into_owned(),
        ground_truth_point_cloud: sensor_msgs::msg::PointCloud2::into_rmw_message(std::borrow::Cow::Owned(msg.ground_truth_point_cloud)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        ground_truth_mesh: shape_msgs::msg::Mesh::into_rmw_message(std::borrow::Cow::Borrowed(&msg.ground_truth_mesh)).into_owned(),
        ground_truth_point_cloud: sensor_msgs::msg::PointCloud2::into_rmw_message(std::borrow::Cow::Borrowed(&msg.ground_truth_point_cloud)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      ground_truth_mesh: shape_msgs::msg::Mesh::from_rmw_message(msg.ground_truth_mesh),
      ground_truth_point_cloud: sensor_msgs::msg::PointCloud2::from_rmw_message(msg.ground_truth_point_cloud),
    }
  }
}


// Corresponds to object_recognition_msgs__msg__ObjectType
/// OBJECT ID #########################################################

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ObjectType {
    /// Contains information about the type of a found object. Those two sets of parameters together uniquely define an
    /// object
    /// The key of the found object: the unique identifier in the given db
    pub key: std::string::String,

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
    pub db: std::string::String,

}



impl Default for ObjectType {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ObjectType::default())
  }
}

impl rosidl_runtime_rs::Message for ObjectType {
  type RmwMsg = super::msg::rmw::ObjectType;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        key: msg.key.as_str().into(),
        db: msg.db.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        key: msg.key.as_str().into(),
        db: msg.db.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      key: msg.key.to_string(),
      db: msg.db.to_string(),
    }
  }
}


// Corresponds to object_recognition_msgs__msg__RecognizedObject
/// HEADER ###########################################################

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RecognizedObject {
    /// The header frame corresponds to the pose frame, NOT the point_cloud frame.
    pub header: std_msgs::msg::Header,

    /// OBJECT INFO #########################################################
    /// Contains information about the type and the position of a found object
    /// Some of those fields might not be filled because the used techniques do not fill them or because the user does not
    /// request them
    /// The type of the found object
    pub type_: super::msg::ObjectType,

    /// confidence: how sure you are it is that object and not another one.
    ///  It is between 0 and 1 and the closer to one it is the better
    pub confidence: f32,

    /// OBJECT CLUSTERS #######################################################
    /// Sometimes you can extract the 3d points that belong to the object, in the frames of the original sensors
    /// (it is an array as you might have several sensors)
    pub point_clouds: Vec<sensor_msgs::msg::PointCloud2>,

    /// Sometimes, you can only provide a bounding box/shape, even in 3d
    /// This is in the pose frame
    pub bounding_mesh: shape_msgs::msg::Mesh,

    /// Sometimes, you only have 2d input so you can't really give a pose, you just get a contour, or a box
    /// The last point will be linked to the first one automatically
    pub bounding_contours: Vec<geometry_msgs::msg::Point>,

    /// POSE INFO #########################################################
    /// This is the result that everybody expects : the pose in some frame given with the input. The units are radian/meters
    /// as usual
    pub pose: geometry_msgs::msg::PoseWithCovarianceStamped,

}



impl Default for RecognizedObject {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RecognizedObject::default())
  }
}

impl rosidl_runtime_rs::Message for RecognizedObject {
  type RmwMsg = super::msg::rmw::RecognizedObject;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        type_: super::msg::ObjectType::into_rmw_message(std::borrow::Cow::Owned(msg.type_)).into_owned(),
        confidence: msg.confidence,
        point_clouds: msg.point_clouds
          .into_iter()
          .map(|elem| sensor_msgs::msg::PointCloud2::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        bounding_mesh: shape_msgs::msg::Mesh::into_rmw_message(std::borrow::Cow::Owned(msg.bounding_mesh)).into_owned(),
        bounding_contours: msg.bounding_contours
          .into_iter()
          .map(|elem| geometry_msgs::msg::Point::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        pose: geometry_msgs::msg::PoseWithCovarianceStamped::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        type_: super::msg::ObjectType::into_rmw_message(std::borrow::Cow::Borrowed(&msg.type_)).into_owned(),
      confidence: msg.confidence,
        point_clouds: msg.point_clouds
          .iter()
          .map(|elem| sensor_msgs::msg::PointCloud2::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        bounding_mesh: shape_msgs::msg::Mesh::into_rmw_message(std::borrow::Cow::Borrowed(&msg.bounding_mesh)).into_owned(),
        bounding_contours: msg.bounding_contours
          .iter()
          .map(|elem| geometry_msgs::msg::Point::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        pose: geometry_msgs::msg::PoseWithCovarianceStamped::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      type_: super::msg::ObjectType::from_rmw_message(msg.type_),
      confidence: msg.confidence,
      point_clouds: msg.point_clouds
          .into_iter()
          .map(sensor_msgs::msg::PointCloud2::from_rmw_message)
          .collect(),
      bounding_mesh: shape_msgs::msg::Mesh::from_rmw_message(msg.bounding_mesh),
      bounding_contours: msg.bounding_contours
          .into_iter()
          .map(geometry_msgs::msg::Point::from_rmw_message)
          .collect(),
      pose: geometry_msgs::msg::PoseWithCovarianceStamped::from_rmw_message(msg.pose),
    }
  }
}


// Corresponds to object_recognition_msgs__msg__RecognizedObjectArray
/// HEADER ###########################################################

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RecognizedObjectArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// This message type describes a potential scene configuration: a set of objects that can explain the scene
    pub objects: Vec<super::msg::RecognizedObject>,

    /// SEARCH ###########################################################
    /// The co-occurrence matrix between the recognized objects
    pub cooccurrence: Vec<f32>,

}



impl Default for RecognizedObjectArray {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RecognizedObjectArray::default())
  }
}

impl rosidl_runtime_rs::Message for RecognizedObjectArray {
  type RmwMsg = super::msg::rmw::RecognizedObjectArray;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        objects: msg.objects
          .into_iter()
          .map(|elem| super::msg::RecognizedObject::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        cooccurrence: msg.cooccurrence.as_slice().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        objects: msg.objects
          .iter()
          .map(|elem| super::msg::RecognizedObject::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        cooccurrence: msg.cooccurrence.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      objects: msg.objects
          .into_iter()
          .map(super::msg::RecognizedObject::from_rmw_message)
          .collect(),
      cooccurrence: msg.cooccurrence.into(),
    }
  }
}


// Corresponds to object_recognition_msgs__msg__Table
/// Informs that a planar table has been detected at a given location

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Table {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// The pose gives you the transform that take you to the coordinate system
    /// of the table, with the origin somewhere in the table plane and the
    /// z axis normal to the plane
    pub pose: geometry_msgs::msg::Pose,

    /// There is no guarantee that the table does NOT extend further than the
    /// convex hull; this is just as far as we've observed it.
    /// The origin of the table coordinate system is inside the convex hull
    /// Set of points forming the convex hull of the table
    pub convex_hull: Vec<geometry_msgs::msg::Point>,

}



impl Default for Table {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Table::default())
  }
}

impl rosidl_runtime_rs::Message for Table {
  type RmwMsg = super::msg::rmw::Table;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
        convex_hull: msg.convex_hull
          .into_iter()
          .map(|elem| geometry_msgs::msg::Point::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
        convex_hull: msg.convex_hull
          .iter()
          .map(|elem| geometry_msgs::msg::Point::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
      convex_hull: msg.convex_hull
          .into_iter()
          .map(geometry_msgs::msg::Point::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to object_recognition_msgs__msg__TableArray

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TableArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// Just an array of tables
    pub tables: Vec<super::msg::Table>,

}



impl Default for TableArray {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::TableArray::default())
  }
}

impl rosidl_runtime_rs::Message for TableArray {
  type RmwMsg = super::msg::rmw::TableArray;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        tables: msg.tables
          .into_iter()
          .map(|elem| super::msg::Table::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        tables: msg.tables
          .iter()
          .map(|elem| super::msg::Table::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      tables: msg.tables
          .into_iter()
          .map(super::msg::Table::from_rmw_message)
          .collect(),
    }
  }
}


