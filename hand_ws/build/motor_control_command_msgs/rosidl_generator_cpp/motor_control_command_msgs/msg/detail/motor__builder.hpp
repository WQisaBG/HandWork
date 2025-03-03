// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from motor_control_command_msgs:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__BUILDER_HPP_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "motor_control_command_msgs/msg/detail/motor__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace motor_control_command_msgs
{

namespace msg
{

namespace builder
{

class Init_Motor_target_position
{
public:
  explicit Init_Motor_target_position(::motor_control_command_msgs::msg::Motor & msg)
  : msg_(msg)
  {}
  ::motor_control_command_msgs::msg::Motor target_position(::motor_control_command_msgs::msg::Motor::_target_position_type arg)
  {
    msg_.target_position = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_control_command_msgs::msg::Motor msg_;
};

class Init_Motor_index
{
public:
  Init_Motor_index()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Motor_target_position index(::motor_control_command_msgs::msg::Motor::_index_type arg)
  {
    msg_.index = std::move(arg);
    return Init_Motor_target_position(msg_);
  }

private:
  ::motor_control_command_msgs::msg::Motor msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_control_command_msgs::msg::Motor>()
{
  return motor_control_command_msgs::msg::builder::Init_Motor_index();
}

}  // namespace motor_control_command_msgs

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR__BUILDER_HPP_
