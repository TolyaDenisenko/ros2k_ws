// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:msg/RecognizedObjectArray.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/msg/detail/recognized_object_array__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `objects`
#include "object_recognition_msgs/msg/detail/recognized_object__functions.h"
// Member `cooccurrence`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
object_recognition_msgs__msg__RecognizedObjectArray__init(object_recognition_msgs__msg__RecognizedObjectArray * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    object_recognition_msgs__msg__RecognizedObjectArray__fini(msg);
    return false;
  }
  // objects
  if (!object_recognition_msgs__msg__RecognizedObject__Sequence__init(&msg->objects, 0)) {
    object_recognition_msgs__msg__RecognizedObjectArray__fini(msg);
    return false;
  }
  // cooccurrence
  if (!rosidl_runtime_c__float__Sequence__init(&msg->cooccurrence, 0)) {
    object_recognition_msgs__msg__RecognizedObjectArray__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__msg__RecognizedObjectArray__fini(object_recognition_msgs__msg__RecognizedObjectArray * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // objects
  object_recognition_msgs__msg__RecognizedObject__Sequence__fini(&msg->objects);
  // cooccurrence
  rosidl_runtime_c__float__Sequence__fini(&msg->cooccurrence);
}

bool
object_recognition_msgs__msg__RecognizedObjectArray__are_equal(const object_recognition_msgs__msg__RecognizedObjectArray * lhs, const object_recognition_msgs__msg__RecognizedObjectArray * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // objects
  if (!object_recognition_msgs__msg__RecognizedObject__Sequence__are_equal(
      &(lhs->objects), &(rhs->objects)))
  {
    return false;
  }
  // cooccurrence
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->cooccurrence), &(rhs->cooccurrence)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__msg__RecognizedObjectArray__copy(
  const object_recognition_msgs__msg__RecognizedObjectArray * input,
  object_recognition_msgs__msg__RecognizedObjectArray * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // objects
  if (!object_recognition_msgs__msg__RecognizedObject__Sequence__copy(
      &(input->objects), &(output->objects)))
  {
    return false;
  }
  // cooccurrence
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->cooccurrence), &(output->cooccurrence)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__msg__RecognizedObjectArray *
object_recognition_msgs__msg__RecognizedObjectArray__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObjectArray * msg = (object_recognition_msgs__msg__RecognizedObjectArray *)allocator.allocate(sizeof(object_recognition_msgs__msg__RecognizedObjectArray), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__msg__RecognizedObjectArray));
  bool success = object_recognition_msgs__msg__RecognizedObjectArray__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__msg__RecognizedObjectArray__destroy(object_recognition_msgs__msg__RecognizedObjectArray * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__msg__RecognizedObjectArray__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__init(object_recognition_msgs__msg__RecognizedObjectArray__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObjectArray * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__msg__RecognizedObjectArray)) {
      return false;
    }
    data = (object_recognition_msgs__msg__RecognizedObjectArray *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__msg__RecognizedObjectArray), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__msg__RecognizedObjectArray__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__msg__RecognizedObjectArray__fini(&data[i - 1]);
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
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__fini(object_recognition_msgs__msg__RecognizedObjectArray__Sequence * array)
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
      object_recognition_msgs__msg__RecognizedObjectArray__fini(&array->data[i]);
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

object_recognition_msgs__msg__RecognizedObjectArray__Sequence *
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObjectArray__Sequence * array = (object_recognition_msgs__msg__RecognizedObjectArray__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__msg__RecognizedObjectArray__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__msg__RecognizedObjectArray__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__destroy(object_recognition_msgs__msg__RecognizedObjectArray__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__msg__RecognizedObjectArray__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__are_equal(const object_recognition_msgs__msg__RecognizedObjectArray__Sequence * lhs, const object_recognition_msgs__msg__RecognizedObjectArray__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__msg__RecognizedObjectArray__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__msg__RecognizedObjectArray__Sequence__copy(
  const object_recognition_msgs__msg__RecognizedObjectArray__Sequence * input,
  object_recognition_msgs__msg__RecognizedObjectArray__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__msg__RecognizedObjectArray)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__msg__RecognizedObjectArray);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__msg__RecognizedObjectArray * data =
      (object_recognition_msgs__msg__RecognizedObjectArray *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__msg__RecognizedObjectArray__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__msg__RecognizedObjectArray__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__msg__RecognizedObjectArray__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
