// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:action/ObjectRecognition.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/action/detail/object_recognition__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `filter_limits`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_Goal__init(object_recognition_msgs__action__ObjectRecognition_Goal * msg)
{
  if (!msg) {
    return false;
  }
  // use_roi
  // filter_limits
  if (!rosidl_runtime_c__float__Sequence__init(&msg->filter_limits, 0)) {
    object_recognition_msgs__action__ObjectRecognition_Goal__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Goal__fini(object_recognition_msgs__action__ObjectRecognition_Goal * msg)
{
  if (!msg) {
    return;
  }
  // use_roi
  // filter_limits
  rosidl_runtime_c__float__Sequence__fini(&msg->filter_limits);
}

bool
object_recognition_msgs__action__ObjectRecognition_Goal__are_equal(const object_recognition_msgs__action__ObjectRecognition_Goal * lhs, const object_recognition_msgs__action__ObjectRecognition_Goal * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // use_roi
  if (lhs->use_roi != rhs->use_roi) {
    return false;
  }
  // filter_limits
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->filter_limits), &(rhs->filter_limits)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Goal__copy(
  const object_recognition_msgs__action__ObjectRecognition_Goal * input,
  object_recognition_msgs__action__ObjectRecognition_Goal * output)
{
  if (!input || !output) {
    return false;
  }
  // use_roi
  output->use_roi = input->use_roi;
  // filter_limits
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->filter_limits), &(output->filter_limits)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_Goal *
object_recognition_msgs__action__ObjectRecognition_Goal__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Goal * msg = (object_recognition_msgs__action__ObjectRecognition_Goal *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Goal), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_Goal));
  bool success = object_recognition_msgs__action__ObjectRecognition_Goal__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_Goal__destroy(object_recognition_msgs__action__ObjectRecognition_Goal * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_Goal__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Goal * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Goal)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_Goal *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_Goal), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_Goal__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_Goal__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_Goal__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_Goal__Sequence *
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_Goal__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Goal__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Goal__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Goal__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Goal)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_Goal);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_Goal * data =
      (object_recognition_msgs__action__ObjectRecognition_Goal *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_Goal__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_Goal__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Goal__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `recognized_objects`
#include "object_recognition_msgs/msg/detail/recognized_object_array__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_Result__init(object_recognition_msgs__action__ObjectRecognition_Result * msg)
{
  if (!msg) {
    return false;
  }
  // recognized_objects
  if (!object_recognition_msgs__msg__RecognizedObjectArray__init(&msg->recognized_objects)) {
    object_recognition_msgs__action__ObjectRecognition_Result__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Result__fini(object_recognition_msgs__action__ObjectRecognition_Result * msg)
{
  if (!msg) {
    return;
  }
  // recognized_objects
  object_recognition_msgs__msg__RecognizedObjectArray__fini(&msg->recognized_objects);
}

bool
object_recognition_msgs__action__ObjectRecognition_Result__are_equal(const object_recognition_msgs__action__ObjectRecognition_Result * lhs, const object_recognition_msgs__action__ObjectRecognition_Result * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // recognized_objects
  if (!object_recognition_msgs__msg__RecognizedObjectArray__are_equal(
      &(lhs->recognized_objects), &(rhs->recognized_objects)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Result__copy(
  const object_recognition_msgs__action__ObjectRecognition_Result * input,
  object_recognition_msgs__action__ObjectRecognition_Result * output)
{
  if (!input || !output) {
    return false;
  }
  // recognized_objects
  if (!object_recognition_msgs__msg__RecognizedObjectArray__copy(
      &(input->recognized_objects), &(output->recognized_objects)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_Result *
object_recognition_msgs__action__ObjectRecognition_Result__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Result * msg = (object_recognition_msgs__action__ObjectRecognition_Result *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Result), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_Result));
  bool success = object_recognition_msgs__action__ObjectRecognition_Result__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_Result__destroy(object_recognition_msgs__action__ObjectRecognition_Result * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_Result__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Result * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Result)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_Result *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_Result), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_Result__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_Result__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_Result__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_Result__Sequence *
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_Result__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Result__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_Result__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Result__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_Result__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Result__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Result__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Result__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Result__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Result)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_Result);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_Result * data =
      (object_recognition_msgs__action__ObjectRecognition_Result *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_Result__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_Result__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Result__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
object_recognition_msgs__action__ObjectRecognition_Feedback__init(object_recognition_msgs__action__ObjectRecognition_Feedback * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Feedback__fini(object_recognition_msgs__action__ObjectRecognition_Feedback * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
object_recognition_msgs__action__ObjectRecognition_Feedback__are_equal(const object_recognition_msgs__action__ObjectRecognition_Feedback * lhs, const object_recognition_msgs__action__ObjectRecognition_Feedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // structure_needs_at_least_one_member
  if (lhs->structure_needs_at_least_one_member != rhs->structure_needs_at_least_one_member) {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Feedback__copy(
  const object_recognition_msgs__action__ObjectRecognition_Feedback * input,
  object_recognition_msgs__action__ObjectRecognition_Feedback * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

object_recognition_msgs__action__ObjectRecognition_Feedback *
object_recognition_msgs__action__ObjectRecognition_Feedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Feedback * msg = (object_recognition_msgs__action__ObjectRecognition_Feedback *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback));
  bool success = object_recognition_msgs__action__ObjectRecognition_Feedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_Feedback__destroy(object_recognition_msgs__action__ObjectRecognition_Feedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_Feedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Feedback * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_Feedback *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_Feedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_Feedback__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_Feedback__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence *
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Feedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_Feedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_Feedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_Feedback * data =
      (object_recognition_msgs__action__ObjectRecognition_Feedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_Feedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_Feedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_Feedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `goal`
// already included above
// #include "object_recognition_msgs/action/detail/object_recognition__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(msg);
    return false;
  }
  // goal
  if (!object_recognition_msgs__action__ObjectRecognition_Goal__init(&msg->goal)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // goal
  object_recognition_msgs__action__ObjectRecognition_Goal__fini(&msg->goal);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // goal
  if (!object_recognition_msgs__action__ObjectRecognition_Goal__are_equal(
      &(lhs->goal), &(rhs->goal)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // goal
  if (!object_recognition_msgs__action__ObjectRecognition_Goal__copy(
      &(input->goal), &(output->goal)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Request *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Request *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request));
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Request *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Request * data =
      (object_recognition_msgs__action__ObjectRecognition_SendGoal_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg)
{
  if (!msg) {
    return false;
  }
  // accepted
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg)
{
  if (!msg) {
    return;
  }
  // accepted
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // accepted
  if (lhs->accepted != rhs->accepted) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // accepted
  output->accepted = input->accepted;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Response *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Response *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response));
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Response *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Response * data =
      (object_recognition_msgs__action__ObjectRecognition_SendGoal_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "object_recognition_msgs/action/detail/object_recognition__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(msg);
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__init(&msg->request, 0)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(msg);
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__init(&msg->response, 0)) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__fini(&msg->request);
  // response
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__fini(&msg->response);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Event *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Event *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event));
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__init(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Event *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence *
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_SendGoal_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_SendGoal_Event * data =
      (object_recognition_msgs__action__ObjectRecognition_SendGoal_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_SendGoal_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Request * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Request *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg = (object_recognition_msgs__action__ObjectRecognition_GetResult_Request *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request));
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_GetResult_Request *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_GetResult_Request * data =
      (object_recognition_msgs__action__ObjectRecognition_GetResult_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_GetResult_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `result`
// already included above
// #include "object_recognition_msgs/action/detail/object_recognition__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg)
{
  if (!msg) {
    return false;
  }
  // status
  // result
  if (!object_recognition_msgs__action__ObjectRecognition_Result__init(&msg->result)) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg)
{
  if (!msg) {
    return;
  }
  // status
  // result
  object_recognition_msgs__action__ObjectRecognition_Result__fini(&msg->result);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // status
  if (lhs->status != rhs->status) {
    return false;
  }
  // result
  if (!object_recognition_msgs__action__ObjectRecognition_Result__are_equal(
      &(lhs->result), &(rhs->result)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Response * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // status
  output->status = input->status;
  // result
  if (!object_recognition_msgs__action__ObjectRecognition_Result__copy(
      &(input->result), &(output->result)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Response *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg = (object_recognition_msgs__action__ObjectRecognition_GetResult_Response *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response));
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_GetResult_Response *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_GetResult_Response * data =
      (object_recognition_msgs__action__ObjectRecognition_GetResult_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_GetResult_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
// already included above
// #include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "object_recognition_msgs/action/detail/object_recognition__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(msg);
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__init(&msg->request, 0)) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(msg);
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__init(&msg->response, 0)) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__fini(&msg->request);
  // response
  object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__fini(&msg->response);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Event * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Event *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg = (object_recognition_msgs__action__ObjectRecognition_GetResult_Event *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event));
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__init(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_GetResult_Event *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence *
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_GetResult_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_GetResult_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_GetResult_Event * data =
      (object_recognition_msgs__action__ObjectRecognition_GetResult_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_GetResult_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_GetResult_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `feedback`
// already included above
// #include "object_recognition_msgs/action/detail/object_recognition__functions.h"

bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(msg);
    return false;
  }
  // feedback
  if (!object_recognition_msgs__action__ObjectRecognition_Feedback__init(&msg->feedback)) {
    object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // feedback
  object_recognition_msgs__action__ObjectRecognition_Feedback__fini(&msg->feedback);
}

bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__are_equal(const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * lhs, const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // feedback
  if (!object_recognition_msgs__action__ObjectRecognition_Feedback__are_equal(
      &(lhs->feedback), &(rhs->feedback)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__copy(
  const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * input,
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // feedback
  if (!object_recognition_msgs__action__ObjectRecognition_Feedback__copy(
      &(input->feedback), &(output->feedback)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__action__ObjectRecognition_FeedbackMessage *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg = (object_recognition_msgs__action__ObjectRecognition_FeedbackMessage *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage));
  bool success = object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__destroy(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage)) {
      return false;
    }
    data = (object_recognition_msgs__action__ObjectRecognition_FeedbackMessage *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence *
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array = (object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__destroy(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__are_equal(const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * lhs, const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence__copy(
  const object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * input,
  object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__action__ObjectRecognition_FeedbackMessage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__action__ObjectRecognition_FeedbackMessage * data =
      (object_recognition_msgs__action__ObjectRecognition_FeedbackMessage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__action__ObjectRecognition_FeedbackMessage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
