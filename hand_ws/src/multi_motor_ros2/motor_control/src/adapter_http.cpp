/**
 * 1 该节点 从web端获取JSON格式的控制指令 
 * 2 将其解析成ROS2中msg格式的数据类型
 * 3 将填充的msg 发布到一个主题上
 * 
 */


#include <iostream>
#include <cstring> 
#include <rclcpp/rclcpp.hpp>
#include "httplib.h"
#include <nlohmann/json.hpp>
#include "motor_control_command_msgs/msg/motor_control_command.hpp" 
#include "motor_control_command_msgs/msg/motor.hpp" // Motor 消息的头文件

using json = nlohmann::json;

class HttpServerNode : public rclcpp::Node {
public:
    HttpServerNode()
        : Node("http_server_node") {
        
        // 创建 HTTP 服务器
        server_.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
            handle_post(req, res);
        });
        
        // 启动 HTTP 服务器
        std::thread([this]() {
            server_.listen("127.0.0.1", 10088); // 在10088端口上监听
        }).detach();
        
        pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);

        RCLCPP_INFO(this->get_logger(), "HTTP server running at http://127.0.0.1:10088/motor/task");
    }

private:
    void handle_post(const httplib::Request &req, httplib::Response &res) {
        try {
            // 解析请求体中的 JSON
            auto json_data = json::parse(req.body);
            RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());
            
            // 从 JSON 数据提取字段
            if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) {
                motor_control_command_msgs::msg::MotorControlCommand msg;
                
                msg.id = json_data["id"];
                msg.timestamp = json_data["timestamp"];

                // 处理电机信息
                const auto& motors = json_data["motor"];
                for (const auto& motor : motors) {
                    motor_control_command_msgs::msg::Motor motor_msg;
                    motor_msg.index = motor["index"];
                    motor_msg.target_position = motor["targetPosition"];
                    
                    // 添加到 MotorCommand 消息的 motors 数组中
                    msg.motors.push_back(motor_msg);
                }//  完成json数据的解析

                // 发布到 ROS 话题
                pub_->publish(msg);
                RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu", 
                            msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
            }

            // 设置回应
            res.set_content("{\"status\":\"received\"}", "application/json");
        } catch (const std::exception &ex) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
        }
    }

    httplib::Server server_; // http服务器实例
    rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_; 
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<HttpServerNode>());
    rclcpp::shutdown();
    return 0;
}