// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from object_recognition_msgs:srv/GetObjectInformation.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/srv/get_object_information.h"


#ifndef OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__FUNCTIONS_H_
#define OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"
#include "object_recognition_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "object_recognition_msgs/srv/detail/get_object_information__struct.h"

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__srv__GetObjectInformation__get_type_hash(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__srv__GetObjectInformation__get_type_description(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__srv__GetObjectInformation__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__srv__GetObjectInformation__get_type_description_sources(
  const rosidl_service_type_support_t * type_support);

/// Initialize srv/GetObjectInformation message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__srv__GetObjectInformation_Request
 * )) before or use
 * object_recognition_msgs__srv__GetObjectInformation_Request__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__init(object_recognition_msgs__srv__GetObjectInformation_Request * msg);

/// Finalize srv/GetObjectInformation message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Request__fini(object_recognition_msgs__srv__GetObjectInformation_Request * msg);

/// Create srv/GetObjectInformation message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Request *
object_recognition_msgs__srv__GetObjectInformation_Request__create(void);

/// Destroy srv/GetObjectInformation message.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Request__destroy(object_recognition_msgs__srv__GetObjectInformation_Request * msg);

/// Check for srv/GetObjectInformation message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Request * lhs, const object_recognition_msgs__srv__GetObjectInformation_Request * rhs);

/// Copy a srv/GetObjectInformation message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Request * input,
  object_recognition_msgs__srv__GetObjectInformation_Request * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__srv__GetObjectInformation_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__srv__GetObjectInformation_Request__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__srv__GetObjectInformation_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array, size_t size);

/// Finalize array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array);

/// Create array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__create(size_t size);

/// Destroy array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array);

/// Check for srv/GetObjectInformation message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * rhs);

/// Copy an array of srv/GetObjectInformation messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * output);

/// Initialize srv/GetObjectInformation message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__srv__GetObjectInformation_Response
 * )) before or use
 * object_recognition_msgs__srv__GetObjectInformation_Response__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__init(object_recognition_msgs__srv__GetObjectInformation_Response * msg);

/// Finalize srv/GetObjectInformation message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Response__fini(object_recognition_msgs__srv__GetObjectInformation_Response * msg);

/// Create srv/GetObjectInformation message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Response *
object_recognition_msgs__srv__GetObjectInformation_Response__create(void);

/// Destroy srv/GetObjectInformation message.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Response__destroy(object_recognition_msgs__srv__GetObjectInformation_Response * msg);

/// Check for srv/GetObjectInformation message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Response * lhs, const object_recognition_msgs__srv__GetObjectInformation_Response * rhs);

/// Copy a srv/GetObjectInformation message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Response * input,
  object_recognition_msgs__srv__GetObjectInformation_Response * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__srv__GetObjectInformation_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__srv__GetObjectInformation_Response__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__srv__GetObjectInformation_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array, size_t size);

/// Finalize array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array);

/// Create array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__create(size_t size);

/// Destroy array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array);

/// Check for srv/GetObjectInformation message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * rhs);

/// Copy an array of srv/GetObjectInformation messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * output);

/// Initialize srv/GetObjectInformation message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__srv__GetObjectInformation_Event
 * )) before or use
 * object_recognition_msgs__srv__GetObjectInformation_Event__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__init(object_recognition_msgs__srv__GetObjectInformation_Event * msg);

/// Finalize srv/GetObjectInformation message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Event__fini(object_recognition_msgs__srv__GetObjectInformation_Event * msg);

/// Create srv/GetObjectInformation message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Event *
object_recognition_msgs__srv__GetObjectInformation_Event__create(void);

/// Destroy srv/GetObjectInformation message.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Event__destroy(object_recognition_msgs__srv__GetObjectInformation_Event * msg);

/// Check for srv/GetObjectInformation message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Event * lhs, const object_recognition_msgs__srv__GetObjectInformation_Event * rhs);

/// Copy a srv/GetObjectInformation message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Event * input,
  object_recognition_msgs__srv__GetObjectInformation_Event * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__srv__GetObjectInformation_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__srv__GetObjectInformation_Event__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__srv__GetObjectInformation_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array, size_t size);

/// Finalize array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array);

/// Create array of srv/GetObjectInformation messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__create(size_t size);

/// Destroy array of srv/GetObjectInformation messages.
/**
 * It calls
 * object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array);

/// Check for srv/GetObjectInformation message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * rhs);

/// Copy an array of srv/GetObjectInformation messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * output);
#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__SRV__DETAIL__GET_OBJECT_INFORMATION__FUNCTIONS_H_
