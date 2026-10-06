// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from object_recognition_msgs:srv/GetObjectInformation.idl
// generated code does not contain a copyright notice
#include "object_recognition_msgs/srv/detail/get_object_information__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `type`
#include "object_recognition_msgs/msg/detail/object_type__functions.h"

bool
object_recognition_msgs__srv__GetObjectInformation_Request__init(object_recognition_msgs__srv__GetObjectInformation_Request * msg)
{
  if (!msg) {
    return false;
  }
  // type
  if (!object_recognition_msgs__msg__ObjectType__init(&msg->type)) {
    object_recognition_msgs__srv__GetObjectInformation_Request__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__srv__GetObjectInformation_Request__fini(object_recognition_msgs__srv__GetObjectInformation_Request * msg)
{
  if (!msg) {
    return;
  }
  // type
  object_recognition_msgs__msg__ObjectType__fini(&msg->type);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Request__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Request * lhs, const object_recognition_msgs__srv__GetObjectInformation_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // type
  if (!object_recognition_msgs__msg__ObjectType__are_equal(
      &(lhs->type), &(rhs->type)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Request__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Request * input,
  object_recognition_msgs__srv__GetObjectInformation_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // type
  if (!object_recognition_msgs__msg__ObjectType__copy(
      &(input->type), &(output->type)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__srv__GetObjectInformation_Request *
object_recognition_msgs__srv__GetObjectInformation_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Request * msg = (object_recognition_msgs__srv__GetObjectInformation_Request *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__srv__GetObjectInformation_Request));
  bool success = object_recognition_msgs__srv__GetObjectInformation_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__srv__GetObjectInformation_Request__destroy(object_recognition_msgs__srv__GetObjectInformation_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__srv__GetObjectInformation_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Request)) {
      return false;
    }
    data = (object_recognition_msgs__srv__GetObjectInformation_Request *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__srv__GetObjectInformation_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__srv__GetObjectInformation_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__srv__GetObjectInformation_Request__fini(&data[i - 1]);
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
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array)
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
      object_recognition_msgs__srv__GetObjectInformation_Request__fini(&array->data[i]);
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

object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array = (object_recognition_msgs__srv__GetObjectInformation_Request__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__srv__GetObjectInformation_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__srv__GetObjectInformation_Request * data =
      (object_recognition_msgs__srv__GetObjectInformation_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__srv__GetObjectInformation_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__srv__GetObjectInformation_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `information`
#include "object_recognition_msgs/msg/detail/object_information__functions.h"

bool
object_recognition_msgs__srv__GetObjectInformation_Response__init(object_recognition_msgs__srv__GetObjectInformation_Response * msg)
{
  if (!msg) {
    return false;
  }
  // information
  if (!object_recognition_msgs__msg__ObjectInformation__init(&msg->information)) {
    object_recognition_msgs__srv__GetObjectInformation_Response__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__srv__GetObjectInformation_Response__fini(object_recognition_msgs__srv__GetObjectInformation_Response * msg)
{
  if (!msg) {
    return;
  }
  // information
  object_recognition_msgs__msg__ObjectInformation__fini(&msg->information);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Response__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Response * lhs, const object_recognition_msgs__srv__GetObjectInformation_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // information
  if (!object_recognition_msgs__msg__ObjectInformation__are_equal(
      &(lhs->information), &(rhs->information)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Response__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Response * input,
  object_recognition_msgs__srv__GetObjectInformation_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // information
  if (!object_recognition_msgs__msg__ObjectInformation__copy(
      &(input->information), &(output->information)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__srv__GetObjectInformation_Response *
object_recognition_msgs__srv__GetObjectInformation_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Response * msg = (object_recognition_msgs__srv__GetObjectInformation_Response *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__srv__GetObjectInformation_Response));
  bool success = object_recognition_msgs__srv__GetObjectInformation_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__srv__GetObjectInformation_Response__destroy(object_recognition_msgs__srv__GetObjectInformation_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__srv__GetObjectInformation_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Response)) {
      return false;
    }
    data = (object_recognition_msgs__srv__GetObjectInformation_Response *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__srv__GetObjectInformation_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__srv__GetObjectInformation_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__srv__GetObjectInformation_Response__fini(&data[i - 1]);
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
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array)
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
      object_recognition_msgs__srv__GetObjectInformation_Response__fini(&array->data[i]);
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

object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array = (object_recognition_msgs__srv__GetObjectInformation_Response__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__srv__GetObjectInformation_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__srv__GetObjectInformation_Response * data =
      (object_recognition_msgs__srv__GetObjectInformation_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__srv__GetObjectInformation_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__srv__GetObjectInformation_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Response__copy(
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
// #include "object_recognition_msgs/srv/detail/get_object_information__functions.h"

bool
object_recognition_msgs__srv__GetObjectInformation_Event__init(object_recognition_msgs__srv__GetObjectInformation_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    object_recognition_msgs__srv__GetObjectInformation_Event__fini(msg);
    return false;
  }
  // request
  if (!object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__init(&msg->request, 0)) {
    object_recognition_msgs__srv__GetObjectInformation_Event__fini(msg);
    return false;
  }
  // response
  if (!object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__init(&msg->response, 0)) {
    object_recognition_msgs__srv__GetObjectInformation_Event__fini(msg);
    return false;
  }
  return true;
}

void
object_recognition_msgs__srv__GetObjectInformation_Event__fini(object_recognition_msgs__srv__GetObjectInformation_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__fini(&msg->request);
  // response
  object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__fini(&msg->response);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Event__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Event * lhs, const object_recognition_msgs__srv__GetObjectInformation_Event * rhs)
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
  if (!object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Event__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Event * input,
  object_recognition_msgs__srv__GetObjectInformation_Event * output)
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
  if (!object_recognition_msgs__srv__GetObjectInformation_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!object_recognition_msgs__srv__GetObjectInformation_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

object_recognition_msgs__srv__GetObjectInformation_Event *
object_recognition_msgs__srv__GetObjectInformation_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Event * msg = (object_recognition_msgs__srv__GetObjectInformation_Event *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(object_recognition_msgs__srv__GetObjectInformation_Event));
  bool success = object_recognition_msgs__srv__GetObjectInformation_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
object_recognition_msgs__srv__GetObjectInformation_Event__destroy(object_recognition_msgs__srv__GetObjectInformation_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    object_recognition_msgs__srv__GetObjectInformation_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__init(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Event)) {
      return false;
    }
    data = (object_recognition_msgs__srv__GetObjectInformation_Event *)allocator.zero_allocate(size, sizeof(object_recognition_msgs__srv__GetObjectInformation_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = object_recognition_msgs__srv__GetObjectInformation_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        object_recognition_msgs__srv__GetObjectInformation_Event__fini(&data[i - 1]);
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
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__fini(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array)
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
      object_recognition_msgs__srv__GetObjectInformation_Event__fini(&array->data[i]);
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

object_recognition_msgs__srv__GetObjectInformation_Event__Sequence *
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array = (object_recognition_msgs__srv__GetObjectInformation_Event__Sequence *)allocator.allocate(sizeof(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__destroy(object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__are_equal(const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * lhs, const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
object_recognition_msgs__srv__GetObjectInformation_Event__Sequence__copy(
  const object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * input,
  object_recognition_msgs__srv__GetObjectInformation_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(object_recognition_msgs__srv__GetObjectInformation_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(object_recognition_msgs__srv__GetObjectInformation_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    object_recognition_msgs__srv__GetObjectInformation_Event * data =
      (object_recognition_msgs__srv__GetObjectInformation_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!object_recognition_msgs__srv__GetObjectInformation_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          object_recognition_msgs__srv__GetObjectInformation_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!object_recognition_msgs__srv__GetObjectInformation_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
