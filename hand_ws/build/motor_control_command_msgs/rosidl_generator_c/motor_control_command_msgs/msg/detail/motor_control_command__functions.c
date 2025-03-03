// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice
#include "motor_control_command_msgs/msg/detail/motor_control_command__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `id`
// Member `timestamp`
#include "rosidl_runtime_c/string_functions.h"
// Member `motors`
#include "motor_control_command_msgs/msg/detail/motor__functions.h"

bool
motor_control_command_msgs__msg__MotorControlCommand__init(motor_control_command_msgs__msg__MotorControlCommand * msg)
{
  if (!msg) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    motor_control_command_msgs__msg__MotorControlCommand__fini(msg);
    return false;
  }
  // timestamp
  if (!rosidl_runtime_c__String__init(&msg->timestamp)) {
    motor_control_command_msgs__msg__MotorControlCommand__fini(msg);
    return false;
  }
  // motors
  if (!motor_control_command_msgs__msg__Motor__Sequence__init(&msg->motors, 0)) {
    motor_control_command_msgs__msg__MotorControlCommand__fini(msg);
    return false;
  }
  return true;
}

void
motor_control_command_msgs__msg__MotorControlCommand__fini(motor_control_command_msgs__msg__MotorControlCommand * msg)
{
  if (!msg) {
    return;
  }
  // id
  rosidl_runtime_c__String__fini(&msg->id);
  // timestamp
  rosidl_runtime_c__String__fini(&msg->timestamp);
  // motors
  motor_control_command_msgs__msg__Motor__Sequence__fini(&msg->motors);
}

bool
motor_control_command_msgs__msg__MotorControlCommand__are_equal(const motor_control_command_msgs__msg__MotorControlCommand * lhs, const motor_control_command_msgs__msg__MotorControlCommand * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  // timestamp
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->timestamp), &(rhs->timestamp)))
  {
    return false;
  }
  // motors
  if (!motor_control_command_msgs__msg__Motor__Sequence__are_equal(
      &(lhs->motors), &(rhs->motors)))
  {
    return false;
  }
  return true;
}

bool
motor_control_command_msgs__msg__MotorControlCommand__copy(
  const motor_control_command_msgs__msg__MotorControlCommand * input,
  motor_control_command_msgs__msg__MotorControlCommand * output)
{
  if (!input || !output) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  // timestamp
  if (!rosidl_runtime_c__String__copy(
      &(input->timestamp), &(output->timestamp)))
  {
    return false;
  }
  // motors
  if (!motor_control_command_msgs__msg__Motor__Sequence__copy(
      &(input->motors), &(output->motors)))
  {
    return false;
  }
  return true;
}

motor_control_command_msgs__msg__MotorControlCommand *
motor_control_command_msgs__msg__MotorControlCommand__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_control_command_msgs__msg__MotorControlCommand * msg = (motor_control_command_msgs__msg__MotorControlCommand *)allocator.allocate(sizeof(motor_control_command_msgs__msg__MotorControlCommand), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(motor_control_command_msgs__msg__MotorControlCommand));
  bool success = motor_control_command_msgs__msg__MotorControlCommand__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
motor_control_command_msgs__msg__MotorControlCommand__destroy(motor_control_command_msgs__msg__MotorControlCommand * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    motor_control_command_msgs__msg__MotorControlCommand__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
motor_control_command_msgs__msg__MotorControlCommand__Sequence__init(motor_control_command_msgs__msg__MotorControlCommand__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_control_command_msgs__msg__MotorControlCommand * data = NULL;

  if (size) {
    data = (motor_control_command_msgs__msg__MotorControlCommand *)allocator.zero_allocate(size, sizeof(motor_control_command_msgs__msg__MotorControlCommand), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = motor_control_command_msgs__msg__MotorControlCommand__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        motor_control_command_msgs__msg__MotorControlCommand__fini(&data[i - 1]);
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
motor_control_command_msgs__msg__MotorControlCommand__Sequence__fini(motor_control_command_msgs__msg__MotorControlCommand__Sequence * array)
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
      motor_control_command_msgs__msg__MotorControlCommand__fini(&array->data[i]);
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

motor_control_command_msgs__msg__MotorControlCommand__Sequence *
motor_control_command_msgs__msg__MotorControlCommand__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_control_command_msgs__msg__MotorControlCommand__Sequence * array = (motor_control_command_msgs__msg__MotorControlCommand__Sequence *)allocator.allocate(sizeof(motor_control_command_msgs__msg__MotorControlCommand__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = motor_control_command_msgs__msg__MotorControlCommand__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
motor_control_command_msgs__msg__MotorControlCommand__Sequence__destroy(motor_control_command_msgs__msg__MotorControlCommand__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    motor_control_command_msgs__msg__MotorControlCommand__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
motor_control_command_msgs__msg__MotorControlCommand__Sequence__are_equal(const motor_control_command_msgs__msg__MotorControlCommand__Sequence * lhs, const motor_control_command_msgs__msg__MotorControlCommand__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!motor_control_command_msgs__msg__MotorControlCommand__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
motor_control_command_msgs__msg__MotorControlCommand__Sequence__copy(
  const motor_control_command_msgs__msg__MotorControlCommand__Sequence * input,
  motor_control_command_msgs__msg__MotorControlCommand__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(motor_control_command_msgs__msg__MotorControlCommand);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    motor_control_command_msgs__msg__MotorControlCommand * data =
      (motor_control_command_msgs__msg__MotorControlCommand *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!motor_control_command_msgs__msg__MotorControlCommand__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          motor_control_command_msgs__msg__MotorControlCommand__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!motor_control_command_msgs__msg__MotorControlCommand__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
