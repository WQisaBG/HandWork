// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#include "motor_control_command_msgs/msg/detail/motor_control_command__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_motor_control_command_msgs
const rosidl_type_hash_t *
motor_control_command_msgs__msg__MotorControlCommand__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4f, 0x3b, 0xa5, 0x43, 0xfd, 0x52, 0x14, 0x8c,
      0xf6, 0x40, 0x73, 0xd6, 0xd3, 0x82, 0x23, 0x80,
      0x11, 0x66, 0xba, 0x73, 0xb8, 0xd0, 0x3e, 0x08,
      0x17, 0x1c, 0xea, 0x5d, 0xa0, 0x78, 0x4b, 0x83,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "motor_control_command_msgs/msg/detail/motor__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t motor_control_command_msgs__msg__Motor__EXPECTED_HASH = {1, {
    0xa0, 0x01, 0xa3, 0xdc, 0x89, 0x05, 0xff, 0xd4,
    0x43, 0x62, 0xd6, 0xb7, 0xb5, 0x62, 0xcc, 0xc2,
    0x2e, 0xcc, 0x06, 0x87, 0x22, 0x01, 0x92, 0x39,
    0x24, 0x71, 0xf1, 0xec, 0x05, 0x30, 0x00, 0x62,
  }};
#endif

static char motor_control_command_msgs__msg__MotorControlCommand__TYPE_NAME[] = "motor_control_command_msgs/msg/MotorControlCommand";
static char motor_control_command_msgs__msg__Motor__TYPE_NAME[] = "motor_control_command_msgs/msg/Motor";

// Define type names, field names, and default values
static char motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__id[] = "id";
static char motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__timestamp[] = "timestamp";
static char motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__motors[] = "motors";

static rosidl_runtime_c__type_description__Field motor_control_command_msgs__msg__MotorControlCommand__FIELDS[] = {
  {
    {motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__timestamp, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {motor_control_command_msgs__msg__MotorControlCommand__FIELD_NAME__motors, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {motor_control_command_msgs__msg__Motor__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription motor_control_command_msgs__msg__MotorControlCommand__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {motor_control_command_msgs__msg__Motor__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
motor_control_command_msgs__msg__MotorControlCommand__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {motor_control_command_msgs__msg__MotorControlCommand__TYPE_NAME, 50, 50},
      {motor_control_command_msgs__msg__MotorControlCommand__FIELDS, 3, 3},
    },
    {motor_control_command_msgs__msg__MotorControlCommand__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&motor_control_command_msgs__msg__Motor__EXPECTED_HASH, motor_control_command_msgs__msg__Motor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = motor_control_command_msgs__msg__Motor__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string id\n"
  "string timestamp\n"
  "Motor[] motors  # \\xe5\\xae\\x9a\\xe4\\xb9\\x89\\xe4\\xb8\\x80\\xe4\\xb8\\xaa Motor \\xe7\\xb1\\xbb\\xe5\\x9e\\x8b\\xe6\\x95\\xb0\\xe7\\xbb\\x84\\xef\\xbc\\x8c\\xe8\\xa1\\xa8\\xe7\\xa4\\xba\\xe5\\xa4\\x9a\\xe4\\xb8\\xaa\\xe7\\x94\\xb5\\xe6\\x9c\\xba";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
motor_control_command_msgs__msg__MotorControlCommand__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {motor_control_command_msgs__msg__MotorControlCommand__TYPE_NAME, 50, 50},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 67, 67},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
motor_control_command_msgs__msg__MotorControlCommand__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *motor_control_command_msgs__msg__MotorControlCommand__get_individual_type_description_source(NULL),
    sources[1] = *motor_control_command_msgs__msg__Motor__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
