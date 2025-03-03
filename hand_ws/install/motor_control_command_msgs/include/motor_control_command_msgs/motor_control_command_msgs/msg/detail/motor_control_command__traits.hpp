// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__TRAITS_HPP_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "motor_control_command_msgs/msg/detail/motor_control_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'motors'
#include "motor_control_command_msgs/msg/detail/motor__traits.hpp"

namespace motor_control_command_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const MotorControlCommand & msg,
  std::ostream & out)
{
  out << "{";
  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: timestamp
  {
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
    out << ", ";
  }

  // member: motors
  {
    if (msg.motors.size() == 0) {
      out << "motors: []";
    } else {
      out << "motors: [";
      size_t pending_items = msg.motors.size();
      for (auto item : msg.motors) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MotorControlCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: timestamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
    out << "\n";
  }

  // member: motors
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motors.size() == 0) {
      out << "motors: []\n";
    } else {
      out << "motors:\n";
      for (auto item : msg.motors) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MotorControlCommand & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace motor_control_command_msgs

namespace rosidl_generator_traits
{

[[deprecated("use motor_control_command_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const motor_control_command_msgs::msg::MotorControlCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  motor_control_command_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use motor_control_command_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const motor_control_command_msgs::msg::MotorControlCommand & msg)
{
  return motor_control_command_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<motor_control_command_msgs::msg::MotorControlCommand>()
{
  return "motor_control_command_msgs::msg::MotorControlCommand";
}

template<>
inline const char * name<motor_control_command_msgs::msg::MotorControlCommand>()
{
  return "motor_control_command_msgs/msg/MotorControlCommand";
}

template<>
struct has_fixed_size<motor_control_command_msgs::msg::MotorControlCommand>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<motor_control_command_msgs::msg::MotorControlCommand>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<motor_control_command_msgs::msg::MotorControlCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__TRAITS_HPP_
