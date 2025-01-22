#include <rclcpp/rclcpp.hpp>
#include "motor_control_v2/HttpServer.hpp"
#include "motor_control_v2/MotorControl.hpp" 
#include <fstream>
#include <nlohmann/json.hpp>

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);

    // 读取配置文件
    std::ifstream config_file("config.json");
    if (!config_file.is_open()) 
    {
        RCLCPP_ERROR(rclcpp::get_logger("main"), "Could not open config.json");
        return -1;
    }

    nlohmann::json config;
    config_file >> config;

    std::string serial_port = config["serial_port"];
    int baudrate = config["baudrate"];
    int http_port = config["http_port"];


    Hand::serialCommunication::SerialCommunication serial_comm(serial_port, baudrate); 
    Hand::motorControl::MotorControl motor_control(serial_comm); // 初始化 MotorControlModule
    Hand::httpServer::HttpServer http_server(http_port); // 创建 HTTP 服务器

    // 设置电机控制和停止的回调
    http_server.setMotorControlCallback([&motor_control](int motor_index, int target_position) 
    {
        motor_control.executeMotorCommand(motor_index, target_position); // 执行电机控制
    });

    http_server.setMotorStopCallback([&motor_control](int motor_index) 
    {
        motor_control.stopMotor(motor_index); // 停止电机
    });

    // 设置获取电机状态的回调
    http_server.setMotorStatusCallback([&motor_control](int motor_index) 
    {
        return motor_control.queryMotorStatus(motor_index); // 查询电机状态
    });

    auto node = rclcpp::Node::make_shared("motor_control_node");
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);

    // 启动 HTTP 服务器
    http_server.start(http_port);

    executor.spin(); // 处理 ROS2 的回调
    rclcpp::shutdown();
    http_server.stop(); // 停止 HTTP 服务器

    return 0;
}