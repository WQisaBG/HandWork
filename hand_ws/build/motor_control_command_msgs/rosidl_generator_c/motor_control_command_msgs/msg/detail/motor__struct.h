// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from motor_control_command_msgs:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__STRUCT_H_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/Motor in the package motor_control_command_msgs.
/**
  * 创建 Motor.msg
 */
typedef struct motor_control_command_msgs__msg__Motor
{
  uint32_t index;
  int32_t target_position;
} motor_control_command_msgs__msg__Motor;

// Struct for a sequence of motor_control_command_msgs__msg__Motor.
typedef struct motor_control_command_msgs__msg__Motor__Sequence
{
  motor_control_command_msgs__msg__Motor * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} motor_control_command_msgs__msg__Motor__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__STRUCT_H_
