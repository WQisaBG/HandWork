// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_H_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'id'
// Member 'timestamp'
#include "rosidl_runtime_c/string.h"
// Member 'motors'
#include "motor_control_command_msgs/msg/detail/motor__struct.h"

/// Struct defined in msg/MotorControlCommand in the package motor_control_command_msgs.
typedef struct motor_control_command_msgs__msg__MotorControlCommand
{
  rosidl_runtime_c__String id;
  rosidl_runtime_c__String timestamp;
  /// 定义一个 Motor 类型数组，表示多个电机
  motor_control_command_msgs__msg__Motor__Sequence motors;
} motor_control_command_msgs__msg__MotorControlCommand;

// Struct for a sequence of motor_control_command_msgs__msg__MotorControlCommand.
typedef struct motor_control_command_msgs__msg__MotorControlCommand__Sequence
{
  motor_control_command_msgs__msg__MotorControlCommand * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} motor_control_command_msgs__msg__MotorControlCommand__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_H_
