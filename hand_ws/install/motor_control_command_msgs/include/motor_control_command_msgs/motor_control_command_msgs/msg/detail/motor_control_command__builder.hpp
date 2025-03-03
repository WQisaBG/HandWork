// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__BUILDER_HPP_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "motor_control_command_msgs/msg/detail/motor_control_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace motor_control_command_msgs
{

namespace msg
{

namespace builder
{

class Init_MotorControlCommand_motors
{
public:
  explicit Init_MotorControlCommand_motors(::motor_control_command_msgs::msg::MotorControlCommand & msg)
  : msg_(msg)
  {}
  ::motor_control_command_msgs::msg::MotorControlCommand motors(::motor_control_command_msgs::msg::MotorControlCommand::_motors_type arg)
  {
    msg_.motors = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_control_command_msgs::msg::MotorControlCommand msg_;
};

class Init_MotorControlCommand_timestamp
{
public:
  explicit Init_MotorControlCommand_timestamp(::motor_control_command_msgs::msg::MotorControlCommand & msg)
  : msg_(msg)
  {}
  Init_MotorControlCommand_motors timestamp(::motor_control_command_msgs::msg::MotorControlCommand::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_MotorControlCommand_motors(msg_);
  }

private:
  ::motor_control_command_msgs::msg::MotorControlCommand msg_;
};

class Init_MotorControlCommand_id
{
public:
  Init_MotorControlCommand_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MotorControlCommand_timestamp id(::motor_control_command_msgs::msg::MotorControlCommand::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_MotorControlCommand_timestamp(msg_);
  }

private:
  ::motor_control_command_msgs::msg::MotorControlCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_control_command_msgs::msg::MotorControlCommand>()
{
  return motor_control_command_msgs::msg::builder::Init_MotorControlCommand_id();
}

}  // namespace motor_control_command_msgs

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__BUILDER_HPP_
