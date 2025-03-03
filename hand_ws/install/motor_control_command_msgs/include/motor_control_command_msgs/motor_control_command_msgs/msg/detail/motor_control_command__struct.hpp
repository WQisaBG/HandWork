// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from motor_control_command_msgs:msg/MotorControlCommand.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_HPP_
#define MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'motors'
#include "motor_control_command_msgs/msg/detail/motor__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__motor_control_command_msgs__msg__MotorControlCommand __attribute__((deprecated))
#else
# define DEPRECATED__motor_control_command_msgs__msg__MotorControlCommand __declspec(deprecated)
#endif

namespace motor_control_command_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct MotorControlCommand_
{
  using Type = MotorControlCommand_<ContainerAllocator>;

  explicit MotorControlCommand_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->timestamp = "";
    }
  }

  explicit MotorControlCommand_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : id(_alloc),
    timestamp(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->timestamp = "";
    }
  }

  // field types and members
  using _id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _id_type id;
  using _timestamp_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _timestamp_type timestamp;
  using _motors_type =
    std::vector<motor_control_command_msgs::msg::Motor_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<motor_control_command_msgs::msg::Motor_<ContainerAllocator>>>;
  _motors_type motors;

  // setters for named parameter idiom
  Type & set__id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__timestamp(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }
  Type & set__motors(
    const std::vector<motor_control_command_msgs::msg::Motor_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<motor_control_command_msgs::msg::Motor_<ContainerAllocator>>> & _arg)
  {
    this->motors = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> *;
  using ConstRawPtr =
    const motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__motor_control_command_msgs__msg__MotorControlCommand
    std::shared_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__motor_control_command_msgs__msg__MotorControlCommand
    std::shared_ptr<motor_control_command_msgs::msg::MotorControlCommand_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MotorControlCommand_ & other) const
  {
    if (this->id != other.id) {
      return false;
    }
    if (this->timestamp != other.timestamp) {
      return false;
    }
    if (this->motors != other.motors) {
      return false;
    }
    return true;
  }
  bool operator!=(const MotorControlCommand_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MotorControlCommand_

// alias to use template instance with default allocator
using MotorControlCommand =
  motor_control_command_msgs::msg::MotorControlCommand_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace motor_control_command_msgs

#endif  // MOTOR_CONTROL_COMMAND_MSGS__MSG__DETAIL__MOTOR_CONTROL_COMMAND__STRUCT_HPP_
