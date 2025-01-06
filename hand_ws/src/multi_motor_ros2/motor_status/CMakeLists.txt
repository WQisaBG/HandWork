cmake_minimum_required(VERSION 3.8)
project(motor_status)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(serial REQUIRED)  # 添加这一行以找到 serial 库


# 添加 cpp-httplib 的路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/cpp-httplib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/serial)


#=======================================================
add_executable(motor_status src/motor_status.cpp)
ament_target_dependencies(
  motor_status
  "rclcpp"
  "std_msgs"
  "serial"
  
)




if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()


install(TARGETS
  motor_status
  DESTINATION lib/${PROJECT_NAME})

ament_package()
