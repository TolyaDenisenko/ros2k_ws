// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:msg/RecognizedObject.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/msg/detail/recognized_object__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `type`
#include "object_recognition_msgs/msg/detail/object_type__functions.h"
// Member `point_clouds`
#include "sensor_msgs/msg/detail/point_cloud2__functions.h"
// Member `bounding_mesh`
#include "shape_msgs/msg/detail/mesh__functions.h"
// Member `bounding_contours`
#include "geometry_msgs/msg/detail/point__functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose_with_covariance_stamped__functions.h"

bool
object_recognition_msgs__msg__RecognizedObject__init(object_recognition_msgs__msg__RecognizedObject * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  // type
  if (!object_recognition_msgs__msg__ObjectType__init(&msg->type)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  // confidence
  // point_clouds
  if (!sensor_msgs__msg__PointCloud2__Sequence__init(&msg->point_clouds, 0)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  // bounding_mesh
  if (!shape_msgs__msg__Mesh__init(&msg->bounding_mesh)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  // bounding_contours
  if (!geometry_msgs__msg__Point__Sequence__init(&msg->bounding_contours, 0)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  // pose
  if (!geometry_msgs__msg__PoseWithCovarianceStamped__init(&msg->pose)) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__msg__RecognizedObject__fini(object_recognition_msgs__msg__RecognizedObject * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // type
  object_recognition_msgs__msg__ObjectType__fini(&msg->type);
  // confidence
  // point_clouds
  sensor_msgs__msg__PointCloud2__Sequence__fini(&msg->point_clouds);
  // bounding_mesh
  shape_msgs__msg__Mesh__fini(&msg->bounding_mesh);
  // bounding_contours
  geometry_msgs__msg__Point__Sequence__fini(&msg->bounding_contours);
  // pose
  geometry_msgs__msg__PoseWithCovarianceStamped__fini(&msg->pose);
}

bool
object_recognition_msgs__msg__RecognizedObject__are_equal(const object_recognition_msgs__msg__RecognizedObject * lhs, const object_recognition_msgs__msg__RecognizedObject * rhs)
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
  // type
  if (!object_recognition_msgs__msg__ObjectType__are_equal(
      &(lhs->type), &(rhs->type)))
  {
    return false;
  }
  // confidence
  if (lhs->confidence != rhs->confidence) {
    return false;
  }
  // point_clouds
  if (!sensor_msgs__msg__PointCloud2__Sequence__are_equal(
      &(lhs->point_clouds), &(rhs->point_clouds)))
  {
    return false;
  }
  // bounding_mesh
  if (!shape_msgs__msg__Mesh__are_equal(
      &(lhs->bounding_mesh), &(rhs->bounding_mesh)))
  {
    return false;
  }
  // bounding_contours
  if (!geometry_msgs__msg__Point__Sequence__are_equal(
      &(lhs->bounding_contours), &(rhs->bounding_contours)))
  {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__PoseWithCovarianceStamped__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__msg__RecognizedObject__copy(
  const object_recognition_msgs__msg__RecognizedObject * input,
  object_recognition_msgs__msg__RecognizedObject * output)
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
  // type
  if (!object_recognition_msgs__msg__ObjectType__copy(
      &(input->type), &(output->type)))
  {
    return false;
  }
  // confidence
  output->confidence = input->confidence;
  // point_clouds
  if (!sensor_msgs__msg__PointCloud2__Sequence__copy(
      &(input->point_clouds), &(output->point_clouds)))
  {
    return false;
  }
  // bounding_mesh
  if (!shape_msgs__msg__Mesh__copy(
      &(input->bounding_mesh), &(output->bounding_mesh)))
  {
    return false;
  }
  // bounding_contours
  if (!geometry_msgs__msg__Point__Sequence__copy(
      &(input->bounding_contours), &(output->bounding_contours)))
  {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__PoseWithCovarianceStamped__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__msg__RecognizedObject *
object_recognition_msgs__msg__RecognizedObject__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObject * msg = (object_recognition_msgs__msg__RecognizedObject *)allocator.allocate(sizeof(object_recognition_msgs__msg__RecognizedObject), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__msg__RecognizedObject));
  bool success = object_recognition_msgs__msg__RecognizedObject__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__msg__RecognizedObject__destroy(object_recognition_msgs__msg__RecognizedObject * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__msg__RecognizedObject__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__msg__RecognizedObject__Sequence__init(object_recognition_msgs__msg__RecognizedObject__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObject * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__msg__RecognizedObject)) {
      return false;
    }
    data = (object_recognition_msgs__msg__RecognizedObject *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__msg__RecognizedObject), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__msg__RecognizedObject__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__msg__RecognizedObject__fini(&data[i - 1]);
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
object_recognition_msgs__msg__RecognizedObject__Sequence__fini(object_recognition_msgs__msg__RecognizedObject__Sequence * array)
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
      object_recognition_msgs__msg__RecognizedObject__fini(&array->data[i]);
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

object_recognition_msgs__msg__RecognizedObject__Sequence *
object_recognition_msgs__msg__RecognizedObject__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__msg__RecognizedObject__Sequence * array = (object_recognition_msgs__msg__RecognizedObject__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__msg__RecognizedObject__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__msg__RecognizedObject__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__msg__RecognizedObject__Sequence__destroy(object_recognition_msgs__msg__RecognizedObject__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__msg__RecognizedObject__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__msg__RecognizedObject__Sequence__are_equal(const object_recognition_msgs__msg__RecognizedObject__Sequence * lhs, const object_recognition_msgs__msg__RecognizedObject__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__msg__RecognizedObject__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__msg__RecognizedObject__Sequence__copy(
  const object_recognition_msgs__msg__RecognizedObject__Sequence * input,
  object_recognition_msgs__msg__RecognizedObject__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__msg__RecognizedObject)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__msg__RecognizedObject);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__msg__RecognizedObject * data =
      (object_recognition_msgs__msg__RecognizedObject *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__msg__RecognizedObject__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__msg__RecognizedObject__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__msg__RecognizedObject__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
