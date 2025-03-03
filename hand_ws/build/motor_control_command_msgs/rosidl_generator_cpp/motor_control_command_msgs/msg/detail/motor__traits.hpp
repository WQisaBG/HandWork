// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from motor_control_command_msgs:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__TRAITS_HPP_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "motor_control_command_msgs/msg/detail/motor__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace motor_control_command_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Motor & msg,
  std::ostream & out)
{
  out << "{";
  // member: index
  {
    out << "index: ";
    rosidl_generator_traits::value_to_yaml(msg.index, out);
    out << ", ";
  }

  // member: target_position
  {
    out << "target_position: ";
    rosidl_generator_traits::value_to_yaml(msg.target_position, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Motor & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: index
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "index: ";
    rosidl_generator_traits::value_to_yaml(msg.index, out);
    out << "\n";
  }

  // member: target_position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_position: ";
    rosidl_generator_traits::value_to_yaml(msg.target_position, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Motor & msg, bool use_flow_style = false)
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
  const motor_control_command_msgs::msg::Motor & msg,
  std::ostream & out, size_t indentation = 0)
{
  motor_control_command_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use motor_control_command_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const motor_control_command_msgs::msg::Motor & msg)
{
  return motor_control_command_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<motor_control_command_msgs::msg::Motor>()
{
  return "motor_control_command_msgs::msg::Motor";
}

template<>
inline const char * name<motor_control_command_msgs::msg::Motor>()
{
  return "motor_control_command_msgs/msg/Motor";
}

template<>
struct has_fixed_size<motor_control_command_msgs::msg::Motor>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<motor_control_command_msgs::msg::Motor>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<motor_control_command_msgs::msg::Motor>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__TRAITS_HPP_
