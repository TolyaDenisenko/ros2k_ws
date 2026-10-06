// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:msg/ObjectInformation.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/msg/detail/object_information__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `ground_truth_mesh`
#include "shape_msgs/msg/detail/mesh__functions.h"
// Member `ground_truth_point_cloud`
#include "sensor_msgs/msg/detail/point_cloud2__functions.h"

bool
object_recognition_msgs__msg__ObjectInformation__init(object_recognition_msgs__msg__ObjectInformation * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    object_recognition_msgs__msg__ObjectInformation__fini(msg);
    return false;
  }
  // ground_truth_mesh
  if (!shape_msgs__msg__Mesh__init(&msg->ground_truth_mesh)) {
    object_recognition_msgs__msg__ObjectInformation__fini(msg);
    return false;
  }
  // ground_truth_point_cloud
  if (!sensor_msgs__msg__PointCloud2__init(&msg->ground_truth_point_cloud)) {
    object_recognition_msgs__msg__ObjectInformation__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__msg__ObjectInformation__fini(object_recognition_msgs__msg__ObjectInformation * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // ground_truth_mesh
  shape_msgs__msg__Mesh__fini(&msg->ground_truth_mesh);
  // ground_truth_point_cloud
  sensor_msgs__msg__PointCloud2__fini(&msg->ground_truth_point_cloud);
}

bool
object_recognition_msgs__msg__ObjectInformation__are_equal(const object_recognition_msgs__msg__ObjectInformation * lhs, const object_recognition_msgs__msg__ObjectInformation * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // ground_truth_mesh
  if (!shape_msgs__msg__Mesh__are_equal(
      &(lhs->ground_truth_mesh), &(rhs->ground_truth_mesh)))
  {
    return false;
  }
  // ground_truth_point_cloud
  if (!sensor_msgs__msg__PointCloud2__are_equal(
      &(lhs->ground_truth_point_cloud), &(rhs->ground_truth_point_cloud)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__msg__ObjectInformation__copy(
  const object_recognition_msgs__msg__ObjectInformation * input,
  object_recognition_msgs__msg__ObjectInformation * output)
{
  if (!input || !output) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // ground_truth_mesh
  if (!shape_msgs__msg__Mesh__copy(
      &(input->ground_truth_mesh), &(output->ground_truth_mesh)))
  {
    return false;
  }
  // ground_truth_point_cloud
  if (!sensor_msgs__msg__PointCloud2__copy(
      &(input->ground_truth_point_cloud), &(output->ground_truth_point_cloud)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__msg__ObjectInformation *
object_recognition_msgs__msg__ObjectInformation__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__ObjectInformation * msg = (object_recognition_msgs__msg__ObjectInformation *)allocator.allocate(sizeof(object_recognition_msgs__msg__ObjectInformation), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__msg__ObjectInformation));
  bool success = object_recognition_msgs__msg__ObjectInformation__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__msg__ObjectInformation__destroy(object_recognition_msgs__msg__ObjectInformation * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__msg__ObjectInformation__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__msg__ObjectInformation__Sequence__init(object_recognition_msgs__msg__ObjectInformation__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__ObjectInformation * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__msg__ObjectInformation)) {
      return false;
    }
    data = (object_recognition_msgs__msg__ObjectInformation *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__msg__ObjectInformation), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__msg__ObjectInformation__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__msg__ObjectInformation__fini(&data[i - 1]);
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
object_recognition_msgs__msg__ObjectInformation__Sequence__fini(object_recognition_msgs__msg__ObjectInformation__Sequence * array)
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
      object_recognition_msgs__msg__ObjectInformation__fini(&array->data[i]);
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

object_recognition_msgs__msg__ObjectInformation__Sequence *
object_recognition_msgs__msg__ObjectInformation__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__ObjectInformation__Sequence * array = (object_recognition_msgs__msg__ObjectInformation__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__msg__ObjectInformation__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__msg__ObjectInformation__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__msg__ObjectInformation__Sequence__destroy(object_recognition_msgs__msg__ObjectInformation__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__msg__ObjectInformation__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__msg__ObjectInformation__Sequence__are_equal(const object_recognition_msgs__msg__ObjectInformation__Sequence * lhs, const object_recognition_msgs__msg__ObjectInformation__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__msg__ObjectInformation__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__msg__ObjectInformation__Sequence__copy(
  const object_recognition_msgs__msg__ObjectInformation__Sequence * input,
  object_recognition_msgs__msg__ObjectInformation__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__msg__ObjectInformation)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__msg__ObjectInformation);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__msg__ObjectInformation * data =
      (object_recognition_msgs__msg__ObjectInformation *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__msg__ObjectInformation__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__msg__ObjectInformation__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__msg__ObjectInformation__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
