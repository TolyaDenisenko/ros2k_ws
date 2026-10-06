// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:msg/TableArray.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/msg/detail/table_array__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `tables`
#include "object_recognition_msgs/msg/detail/table__functions.h"

bool
object_recognition_msgs__msg__TableArray__init(object_recognition_msgs__msg__TableArray * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    object_recognition_msgs__msg__TableArray__fini(msg);
    return false;
  }
  // tables
  if (!object_recognition_msgs__msg__Table__Sequence__init(&msg->tables, 0)) {
    object_recognition_msgs__msg__TableArray__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__msg__TableArray__fini(object_recognition_msgs__msg__TableArray * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // tables
  object_recognition_msgs__msg__Table__Sequence__fini(&msg->tables);
}

bool
object_recognition_msgs__msg__TableArray__are_equal(const object_recognition_msgs__msg__TableArray * lhs, const object_recognition_msgs__msg__TableArray * rhs)
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
  // tables
  if (!object_recognition_msgs__msg__Table__Sequence__are_equal(
      &(lhs->tables), &(rhs->tables)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__msg__TableArray__copy(
  const object_recognition_msgs__msg__TableArray * input,
  object_recognition_msgs__msg__TableArray * output)
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
  // tables
  if (!object_recognition_msgs__msg__Table__Sequence__copy(
      &(input->tables), &(output->tables)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__msg__TableArray *
object_recognition_msgs__msg__TableArray__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__TableArray * msg = (object_recognition_msgs__msg__TableArray *)allocator.allocate(sizeof(object_recognition_msgs__msg__TableArray), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__msg__TableArray));
  bool success = object_recognition_msgs__msg__TableArray__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__msg__TableArray__destroy(object_recognition_msgs__msg__TableArray * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__msg__TableArray__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__msg__TableArray__Sequence__init(object_recognition_msgs__msg__TableArray__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__TableArray * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__msg__TableArray)) {
      return false;
    }
    data = (object_recognition_msgs__msg__TableArray *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__msg__TableArray), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__msg__TableArray__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__msg__TableArray__fini(&data[i - 1]);
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
object_recognition_msgs__msg__TableArray__Sequence__fini(object_recognition_msgs__msg__TableArray__Sequence * array)
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
      object_recognition_msgs__msg__TableArray__fini(&array->data[i]);
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

object_recognition_msgs__msg__TableArray__Sequence *
object_recognition_msgs__msg__TableArray__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__TableArray__Sequence * array = (object_recognition_msgs__msg__TableArray__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__msg__TableArray__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__msg__TableArray__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__msg__TableArray__Sequence__destroy(object_recognition_msgs__msg__TableArray__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__msg__TableArray__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__msg__TableArray__Sequence__are_equal(const object_recognition_msgs__msg__TableArray__Sequence * lhs, const object_recognition_msgs__msg__TableArray__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__msg__TableArray__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__msg__TableArray__Sequence__copy(
  const object_recognition_msgs__msg__TableArray__Sequence * input,
  object_recognition_msgs__msg__TableArray__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__msg__TableArray)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__msg__TableArray);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__msg__TableArray * data =
      (object_recognition_msgs__msg__TableArray *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__msg__TableArray__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__msg__TableArray__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__msg__TableArray__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
