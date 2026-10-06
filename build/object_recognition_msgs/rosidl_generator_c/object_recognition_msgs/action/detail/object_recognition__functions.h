// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from object_recognition_msgs:action/ObjectRecognition.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "object_recognition_msgs/action/object_recognition.h"


#ifndef OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__FUNCTIONS_H_
#define OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__FUNCTIONS_H_

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

#include "object_recognition_msgs/action/detail/object_recognition__struct.h"

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition__get_type_hash(
  const rosidl_action_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition__get_type_description(
  const rosidl_action_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition__get_individual_type_description_source(
  const rosidl_action_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition__get_type_description_sources(
  const rosidl_action_type_support_t * type_support);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_Goal
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_Goal__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Goal__init(object_recognition_msgs__action__ObjectRecognition_Goal * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Goal__fini(object_recognition_msgs__action__ObjectRecognition_Goal * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Goal *
object_recognition_msgs__action__ObjectRecognition_Goal__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Goal__destroy(object_recognition_msgs__action__ObjectRecognition_Goal * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Goal__are_equal(const object_recognition_msgs__action__ObjectRecognition_Goal * lhs, const object_recognition_msgs__action__ObjectRecognition_Goal * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_Goal__copy(
  const object_recognition_msgs__action__ObjectRecognition_Goal * input,
  object_recognition_msgs__action__ObjectRecognition_Goal * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_Goal__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_Goal__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_Goal__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_Goal__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence *
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_Result
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_Result__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Result__init(object_recognition_msgs__action__ObjectRecognition_Result * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Result__fini(object_recognition_msgs__action__ObjectRecognition_Result * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_Result__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Result *
object_recognition_msgs__action__ObjectRecognition_Result__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Result__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Result__destroy(object_recognition_msgs__action__ObjectRecognition_Result * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Result__are_equal(const object_recognition_msgs__action__ObjectRecognition_Result * lhs, const object_recognition_msgs__action__ObjectRecognition_Result * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_Result__copy(
  const object_recognition_msgs__action__ObjectRecognition_Result * input,
  object_recognition_msgs__action__ObjectRecognition_Result * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_Result__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_Result__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_Result__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_Result__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_Result__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Result__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Result__Sequence *
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Result__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_Feedback
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_Feedback__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Feedback__init(object_recognition_msgs__action__ObjectRecognition_Feedback * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Feedback__fini(object_recognition_msgs__action__ObjectRecognition_Feedback * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Feedback *
object_recognition_msgs__action__ObjectRecognition_Feedback__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Feedback__destroy(object_recognition_msgs__action__ObjectRecognition_Feedback * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Feedback__are_equal(const object_recognition_msgs__action__ObjectRecognition_Feedback * lhs, const object_recognition_msgs__action__ObjectRecognition_Feedback * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_Feedback__copy(
  const object_recognition_msgs__action__ObjectRecognition_Feedback * input,
  object_recognition_msgs__action__ObjectRecognition_Feedback * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_Feedback__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_Feedback__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_Feedback__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_Feedback__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence *
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_SendGoal__get_type_hash(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_SendGoal__get_type_description(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_SendGoal__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal__get_type_description_sources(
  const rosidl_service_type_support_t * type_support);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_GetResult__get_type_hash(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_GetResult__get_type_description(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_GetResult__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult__get_type_description_sources(
  const rosidl_service_type_support_t * type_support);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Request *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Response *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Event *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * output);

/// Initialize action/ObjectRecognition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage
 * )) before or use
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg);

/// Finalize action/ObjectRecognition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg);

/// Create action/ObjectRecognition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__create(void);

/// Destroy action/ObjectRecognition message.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__destroy(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg);

/// Check for action/ObjectRecognition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__are_equal(const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * lhs, const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * rhs);

/// Copy a action/ObjectRecognition message.
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
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__copy(
  const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * input,
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the number of elements and calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array, size_t size);

/// Finalize array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array);

/// Create array of action/ObjectRecognition messages.
/**
 * It allocates the memory for the array and calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__create(size_t size);

/// Destroy array of action/ObjectRecognition messages.
/**
 * It calls
 * object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array);

/// Check for action/ObjectRecognition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * rhs);

/// Copy an array of action/ObjectRecognition messages.
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
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // OBJECT_RECOGNITION_MSGS__ACTION__DETAIL__OBJECT_RECOGNITION__FUNCTIONS_H_
