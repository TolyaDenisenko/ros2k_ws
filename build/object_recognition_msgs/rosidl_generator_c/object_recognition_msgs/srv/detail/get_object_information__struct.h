// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from object_recognition_msgs:srv/GetObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/srv/get_object_information.h"


#ifndef OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__STRUCT_H_
#define OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'type'
#include "object_recognition_msgs/msg/detail/object_type__struct.h"

/// Struct defined in srv/GetObjectInformation in the package object_recognition_msgs.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Request
{
  /// The type of the object to retrieve info from
  object_recognition_msgs__msg__ObjectType type;
} object_recognition_msgs__srv__GetObjectInformation_Request;

// Struct for a sequence of object_recognition_msgs__srv__GetObjectInformation_Request.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Request__Sequence
{
  object_recognition_msgs__srv__GetObjectInformation_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__srv__GetObjectInformation_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'information'
#include "object_recognition_msgs/msg/detail/object_information__struct.h"

/// Struct defined in srv/GetObjectInformation in the package object_recognition_msgs.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Response
{
  /// Extra object info
  object_recognition_msgs__msg__ObjectInformation information;
} object_recognition_msgs__srv__GetObjectInformation_Response;

// Struct for a sequence of object_recognition_msgs__srv__GetObjectInformation_Response.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Response__Sequence
{
  object_recognition_msgs__srv__GetObjectInformation_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__srv__GetObjectInformation_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  object_recognition_msgs__srv__GetObjectInformation_Event__request__MAX_SIZE = 1
};
// response
enum
{
  object_recognition_msgs__srv__GetObjectInformation_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/GetObjectInformation in the package object_recognition_msgs.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Event
{
  service_msgs__msg__ServiceEventInfo info;
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence request;
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence response;
} object_recognition_msgs__srv__GetObjectInformation_Event;

// Struct for a sequence of object_recognition_msgs__srv__GetObjectInformation_Event.
typedef struct object_recognition_msgs__srv__GetObjectInformation_Event__Sequence
{
  object_recognition_msgs__srv__GetObjectInformation_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} object_recognition_msgs__srv__GetObjectInformation_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__STRUCT_H_
