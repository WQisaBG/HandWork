// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from motor_control_command_msgs:msg/Motor.idl
// generated code does not contain a copyright notice

#include "motor_control_command_msgs/msg/detail/motor__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_motor_control_command_msgs
const rosidl_type_hash_t *
motor_control_command_msgs__msg__Motor__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa0, 0x01, 0xa3, 0xdc, 0x89, 0x05, 0xff, 0xd4,
      0x43, 0x62, 0xd6, 0xb7, 0xb5, 0x62, 0xcc, 0xc2,
      0x2e, 0xcc, 0x06, 0x87, 0x22, 0x01, 0x92, 0x39,
      0x24, 0x71, 0xf1, 0xec, 0x05, 0x30, 0x00, 0x62,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char motor_control_command_msgs__msg__Motor__TYPE_NAME[] = "motor_control_command_msgs/msg/Motor";

// Define type names, field names, and default values
static char motor_control_command_msgs__msg__Motor__FIELD_NAME__index[] = "index";
static char motor_control_command_msgs__msg__Motor__FIELD_NAME__target_position[] = "target_position";

static rosidl_runtime_c__type_description__Field motor_control_command_msgs__msg__Motor__FIELDS[] = {
  {
    {motor_control_command_msgs__msg__Motor__FIELD_NAME__index, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {motor_control_command_msgs__msg__Motor__FIELD_NAME__target_position, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
motor_control_command_msgs__msg__Motor__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {motor_control_command_msgs__msg__Motor__TYPE_NAME, 36, 36},
      {motor_control_command_msgs__msg__Motor__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# \\xe5\\x88\\x9b\\xe5\\xbb\\xba Motor.msg\n"
  "uint32 index\n"
  "int32 target_position";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
motor_control_command_msgs__msg__Motor__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {motor_control_command_msgs__msg__Motor__TYPE_NAME, 36, 36},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 49, 49},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
motor_control_command_msgs__msg__Motor__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *motor_control_command_msgs__msg__Motor__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
