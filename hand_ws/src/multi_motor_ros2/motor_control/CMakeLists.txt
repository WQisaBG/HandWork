cmake_minimum_required(VERSION 3.8)
project(motor_control)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(motor_control_command_msgs REQUIRED)

# 添加 cpp-httplib 的路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/cpp-httplib)

#=======================================================
add_executable(adapter_http src/adapter_http.cpp)
ament_target_dependencies(
  adapter_http
  "rclcpp"
  "std_msgs"
  "motor_control_command_msgs"
)





#=======================================================
add_executable(deal_http src/deal_http.cpp)
ament_target_dependencies(
  deal_http
  "rclcpp"
  "std_msgs"
  "motor_control_command_msgs"
)







install(TARGETS
  adapter_http
  deal_http
  DESTINATION lib/${PROJECT_NAME})
ament_package()
