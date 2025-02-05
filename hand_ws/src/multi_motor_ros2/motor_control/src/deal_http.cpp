// #include <iostream>
// #include <string>
// #include <thread>
// #include <mutex>
// #include <queue>
// #include <condition_variable>
// #include <atomic>
// #include "serial/serial.h"

// // 全局变量
// std::queue<std::string> response_queue;
// std::mutex queue_mutex;
// std::condition_variable queue_condition;
// std::atomic<bool> running(true); // 用于控制线程的运行状态

// void read_from_serial(serial::Serial &serial_port) 
// {
//     while (running) 
//     {
//         if (serial_port.isOpen()) 
//         {
//             try {
//                 std::string response = serial_port.readline(100, "\n"); // 读取一行
//                 if (!response.empty()) 
//                 {
//                     {
//                         std::lock_guard<std::mutex> lock(queue_mutex);
//                         response_queue.push(response);
//                     }
//                     queue_condition.notify_one(); // 通知主线程有新数据可用
//                 }
//             } 
//             catch (const std::exception &e) 
//             {
//                 std::cerr << "Error reading from serial port: " << e.what() << std::endl;
//             }
//         } else {
//             std::cerr << "Serial port not open!" << std::endl;
//         }
//     }
// }

// int main() {
//     // 创建串口对象
//     serial::Serial serial_port;

//     // 设置串口参数
//     serial_port.setPort("/dev/ttyUSB0"); // 请根据实际情况修改
//     serial_port.setBaudrate(9600);
//     serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
//     serial_port.setTimeout(timeout);

//     // 尝试打开串口
//     try {
//         serial_port.open();
//         std::cout << "Serial port opened successfully." << std::endl;
//     } catch (const serial::IOException &e) {
//         std::cerr << "Failed to open serial port: " << e.what() << std::endl;
//         return 1;
//     }

//     // 启动读取串口的线程
//     std::thread read_thread(read_from_serial, std::ref(serial_port));

//     // 发送和接收指令
//     std::string command;
//     while (running) {
//         std::cout << "Enter command (type 'exit' to quit): ";
//         std::getline(std::cin, command);

//         if (command == "exit") {
//             running = false; // 发送退出信号
//             break;
//         }

//         command += "\n"; // 添加换行符以结束指令
//         try {
//             serial_port.write(command); // 发送指令
//         } catch (const std::exception &e) {
//             std::cerr << "Failed to write to serial port: " << e.what() << std::endl;
//             continue; // 继续循环以进行下一次输入
//         }

//         // 检查响应队列
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         if (queue_condition.wait_for(lock, std::chrono::milliseconds(100), [] { return !response_queue.empty(); })) {
//             // 如果队列中有数据则处理
//             std::string response = response_queue.front();
//             response_queue.pop();
//             std::cout << "Received response: " << response << std::endl;
//         } else {
//             std::cout << "No response received within the timeout." << std::endl;
//         }
//     }

//     // 清理工作
//     if (running) 
//     {
//         running = false;
//     }

//     read_thread.join(); // 等待读取线程结束
//     serial_port.close(); // 关闭串口
//     std::cout << "Exited cleanly." << std::endl;
//     return 0;
// }






















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