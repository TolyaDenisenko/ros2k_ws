// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:msg/ObjectType.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/msg/object_type.h"


#ifndef OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'key'
// Member 'db'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/ObjectType in the package object_recognition_msgs.
/**
  * OBJECT ID #########################################################
 */
typedef struct object_recognition_msgs__msg__ObjectType
{
  /// Contains information about the type of a found object. Those two sets of parameters together uniquely define an
  /// object
  /// The key of the found object: the unique identifier in the given db
  rosidl_runtime_c__String key;
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
  rosidl_runtime_c__String db;
} object_recognition_msgs__msg__ObjectType;

// Struct for a sequence of object_recognition_msgs__msg__ObjectType.
typedef struct object_recognition_msgs__msg__ObjectType__Sequence
{
  object_recognition_msgs__msg__ObjectType * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__msg__ObjectType__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__MSG__DETAIL__OBJECT_TYPE__STRUCT_H_
