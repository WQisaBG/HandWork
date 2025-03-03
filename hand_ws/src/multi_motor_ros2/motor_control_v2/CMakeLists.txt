cmake_minimum_required(VERSION 3.8)
project(motor_control_v2)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(serial REQUIRED)  # 添加这一行以找到 serial 库
find_package(motor_control_command_msgs REQUIRED)



# 添加 cpp-httplib 的路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/cpp-httplib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/serial)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/json/include)
include_directories(include)

#=======================================================
add_executable(motor_control_node src/main.cpp src/HttpServer.cpp src/MotorControl.cpp src/SerialCommunication.cpp)


ament_target_dependencies(
  motor_control_node
  "rclcpp"
  "std_msgs"
  "motor_control_command_msgs"
  "serial"
              
)


install(TARGETS
  motor_control_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
