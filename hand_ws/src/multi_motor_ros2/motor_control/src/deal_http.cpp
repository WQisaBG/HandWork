#include <iostream>
#include <cstring> 
#include <rclcpp/rclcpp.hpp>
#include "motor_control_command_msgs/msg/motor_control_command.hpp" // 替换为您的消息头文件
#include "motor_control_command_msgs/msg/motor.hpp" // 替换为 Motor 消息的头文件


class MotorCommandSubscriber : public rclcpp::Node {
public:
    MotorCommandSubscriber() : Node("motor_command_subscriber") {
        // 创建订阅者并注册回调函数
        subscription_ = this->create_subscription<motor_control_command_msgs::msg::MotorControlCommand>(
            "motor_command", 10, std::bind(&MotorCommandSubscriber::on_message, this, std::placeholders::_1));
        
        RCLCPP_INFO(this->get_logger(), "Subscribed to 'motor_command' topic");
    }

private:
    void on_message(const motor_control_command_msgs::msg::MotorControlCommand::SharedPtr msg) {
        // 处理接收到的 MotorCommand 消息
        RCLCPP_INFO(this->get_logger(), "Received MotorCommand: id=%s, timestamp=%s", 
                    msg->id.c_str(), msg->timestamp.c_str());
        
        for (const auto &motor : msg->motors) {
            RCLCPP_INFO(this->get_logger(), " Motor index: %d, Target Position: %d", 
                        motor.index, motor.target_position);
        }
    }

    rclcpp::Subscription<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr subscription_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorCommandSubscriber>());
    rclcpp::shutdown();
    return 0;
}