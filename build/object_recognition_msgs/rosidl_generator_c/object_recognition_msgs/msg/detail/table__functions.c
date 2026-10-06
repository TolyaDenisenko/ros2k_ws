// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:msg/Table.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/msg/detail/table__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"
// Member `convex_hull`
#include "geometry_msgs/msg/detail/point__functions.h"

bool
object_recognition_msgs__msg__Table__init(object_recognition_msgs__msg__Table * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    object_recognition_msgs__msg__Table__fini(msg);
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    object_recognition_msgs__msg__Table__fini(msg);
    return false;
  }
  // convex_hull
  if (!geometry_msgs__msg__Point__Sequence__init(&msg->convex_hull, 0)) {
    object_recognition_msgs__msg__Table__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__msg__Table__fini(object_recognition_msgs__msg__Table * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
  // convex_hull
  geometry_msgs__msg__Point__Sequence__fini(&msg->convex_hull);
}

bool
object_recognition_msgs__msg__Table__are_equal(const object_recognition_msgs__msg__Table * lhs, const object_recognition_msgs__msg__Table * rhs)
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
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  // convex_hull
  if (!geometry_msgs__msg__Point__Sequence__are_equal(
      &(lhs->convex_hull), &(rhs->convex_hull)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__msg__Table__copy(
  const object_recognition_msgs__msg__Table * input,
  object_recognition_msgs__msg__Table * output)
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
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  // convex_hull
  if (!geometry_msgs__msg__Point__Sequence__copy(
      &(input->convex_hull), &(output->convex_hull)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__msg__Table *
object_recognition_msgs__msg__Table__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__Table * msg = (object_recognition_msgs__msg__Table *)allocator.allocate(sizeof(object_recognition_msgs__msg__Table), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__msg__Table));
  bool success = object_recognition_msgs__msg__Table__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__msg__Table__destroy(object_recognition_msgs__msg__Table * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__msg__Table__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__msg__Table__Sequence__init(object_recognition_msgs__msg__Table__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__Table * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__msg__Table)) {
      return false;
    }
    data = (object_recognition_msgs__msg__Table *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__msg__Table), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__msg__Table__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__msg__Table__fini(&data[i - 1]);
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
object_recognition_msgs__msg__Table__Sequence__fini(object_recognition_msgs__msg__Table__Sequence * array)
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
      object_recognition_msgs__msg__Table__fini(&array->data[i]);
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

object_recognition_msgs__msg__Table__Sequence *
object_recognition_msgs__msg__Table__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__Table__Sequence * array = (object_recognition_msgs__msg__Table__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__msg__Table__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__msg__Table__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__msg__Table__Sequence__destroy(object_recognition_msgs__msg__Table__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__msg__Table__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__msg__Table__Sequence__are_equal(const object_recognition_msgs__msg__Table__Sequence * lhs, const object_recognition_msgs__msg__Table__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__msg__Table__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__msg__Table__Sequence__copy(
  const object_recognition_msgs__msg__Table__Sequence * input,
  object_recognition_msgs__msg__Table__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__msg__Table)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__msg__Table);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__msg__Table * data =
      (object_recognition_msgs__msg__Table *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__msg__Table__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__msg__Table__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__msg__Table__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
