// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "motor_control_command_msgs/msg/detail/motor_control_command__rosidl_typesupport_introspection_c.h"
#include "motor_control_command_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "motor_control_command_msgs/msg/detail/motor_control_command__functions.h"
#include "motor_control_command_msgs/msg/detail/motor_control_command__struct.h"


// Include directives for member types
// Member `id`
// Member `timestamp`
#include "rosidl_runtime_c/string_functions.h"
// Member `motors`
#include "motor_control_command_msgs/msg/motor.h"
// Member `motors`
#include "motor_control_command_msgs/msg/detail/motor__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  motor_control_command_msgs__msg__MotorControlCommand__init(message_memory);
}

void motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_fini_function(void * message_memory)
{
  motor_control_command_msgs__msg__MotorControlCommand__fini(message_memory);
}

size_t motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__size_function__MotorControlCommand__motors(
  const void * untyped_member)
{
  const motor_control_command_msgs__msg__Motor__Sequence * member =
    (const motor_control_command_msgs__msg__Motor__Sequence *)(untyped_member);
  return member->size;
}

const void * motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_const_function__MotorControlCommand__motors(
  const void * untyped_member, size_t index)
{
  const motor_control_command_msgs__msg__Motor__Sequence * member =
    (const motor_control_command_msgs__msg__Motor__Sequence *)(untyped_member);
  return &member->data[index];
}

void * motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_function__MotorControlCommand__motors(
  void * untyped_member, size_t index)
{
  motor_control_command_msgs__msg__Motor__Sequence * member =
    (motor_control_command_msgs__msg__Motor__Sequence *)(untyped_member);
  return &member->data[index];
}

void motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__fetch_function__MotorControlCommand__motors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const motor_control_command_msgs__msg__Motor * item =
    ((const motor_control_command_msgs__msg__Motor *)
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_const_function__MotorControlCommand__motors(untyped_member, index));
  motor_control_command_msgs__msg__Motor * value =
    (motor_control_command_msgs__msg__Motor *)(untyped_value);
  *value = *item;
}

void motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__assign_function__MotorControlCommand__motors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  motor_control_command_msgs__msg__Motor * item =
    ((motor_control_command_msgs__msg__Motor *)
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_function__MotorControlCommand__motors(untyped_member, index));
  const motor_control_command_msgs__msg__Motor * value =
    (const motor_control_command_msgs__msg__Motor *)(untyped_value);
  *item = *value;
}

bool motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__resize_function__MotorControlCommand__motors(
  void * untyped_member, size_t size)
{
  motor_control_command_msgs__msg__Motor__Sequence * member =
    (motor_control_command_msgs__msg__Motor__Sequence *)(untyped_member);
  motor_control_command_msgs__msg__Motor__Sequence__fini(member);
  return motor_control_command_msgs__msg__Motor__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_member_array[3] = {
  {
    "id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(motor_control_command_msgs__msg__MotorControlCommand, id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "timestamp",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(motor_control_command_msgs__msg__MotorControlCommand, timestamp),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "motors",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(motor_control_command_msgs__msg__MotorControlCommand, motors),  // bytes offset in struct
    NULL,  // default value
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__size_function__MotorControlCommand__motors,  // size() function pointer
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_const_function__MotorControlCommand__motors,  // get_const(index) function pointer
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__get_function__MotorControlCommand__motors,  // get(index) function pointer
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__fetch_function__MotorControlCommand__motors,  // fetch(index, &value) function pointer
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__assign_function__MotorControlCommand__motors,  // assign(index, value) function pointer
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__resize_function__MotorControlCommand__motors  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_members = {
  "motor_control_command_msgs__msg",  // message namespace
  "MotorControlCommand",  // message name
  3,  // number of fields
  sizeof(motor_control_command_msgs__msg__MotorControlCommand),
  motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_member_array,  // message members
  motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_init_function,  // function to initialize message memory (memory has to be allocated)
  motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_type_support_handle = {
  0,
  &motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_motor_control_command_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, motor_control_command_msgs, msg, MotorControlCommand)() {
  motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, motor_control_command_msgs, msg, Motor)();
  if (!motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_type_support_handle.typesupport_identifier) {
    motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &motor_control_command_msgs__msg__MotorControlCommand__rosidl_typesupport_introspection_c__MotorControlCommand_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
