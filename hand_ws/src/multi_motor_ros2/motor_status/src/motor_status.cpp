#include <chrono>
#include <iostream>
#include <cstring>
#include <sstream>
#include <thread>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "httplib.h"
#include "serial/serial.h"
#include "nlohmann/json.hpp"


using json = nlohmann::json;

class MotorStatusNode : public rclcpp::Node {
public:
    MotorStatusNode() : Node("motor_status_node")
    {
        // 定义串口
        serial_port_.setPort("/dev/ttyV20");
        serial_port_.setBaudrate(115200);
        // 设置超时
        serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
        serial_port_.setTimeout(timeout);

        try 
        {
            serial_port_.open();
            RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
        } 
        catch (const std::exception &e) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port: %s", e.what());
            rclcpp::shutdown();
        }

        // 启动 HTTP 服务器
        start_http_server();

        // 定义发布者（可以用来发布电机状态）
        status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

        // 创建定时器以定期更新电机状态
        timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&MotorStatusNode::publish_motor_status, this));
    }

private:
    void start_http_server() 
    {
        // 启动 HTTP 服务器的线程
        std::thread([this]() 
        {
            httplib::Server svr;    //实例化http svr
            //状态查询的GET请求
            svr.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) 
                {
                    // 查询电机状态并发送 JSON 响应
                    json status = query_motor_status();
                    res.set_content(status.dump(), "application/json");
                }
            );


            svr.listen("127.0.0.1", 10089); //启动监听服务器
        }).detach();
    }

    json query_motor_status() 
    {
        std::int32_t decimalValue = 0;
        if (serial_port_.isOpen()) 
        {
            std::string command = "GET_STATUS \n"; // 根据电机协议发送查询命令  目前GET请求获取,固定的
            serial_port_.write(command);    //写指令

            std::string result = serial_port_.readline(100); // 读取串口响应  获取电机返回的结果
            try 
            {
                std::string combinedHex = extractAndCombineHexValues(result);
                decimalValue = parseHexToDecimal(combinedHex);
                std::cout << "Combined Hex value: " << combinedHex << ", Decimal value: " << decimalValue << std::endl;

            } 
            catch (const std::invalid_argument& e) 
            {
                std::cerr << "Error: " << e.what() << std::endl;
            } 
            catch (const std::out_of_range& e) 
            {
                std::cerr << "Hex string out of range: " << e.what() << std::endl;
            }

            json motor_status;
            motor_status["timestamp"] = get_current_time();

            // 解析结果并封装为 JSON
            std::vector<json> motors;
            std::int32_t index = 0;
            json motor;
                try 
                {
                    motor["index"] = index++;
                    motor["currentPosition"] = decimalValue;
                    motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 示例
                    motor["error"] = "No error"; 
                    motor["mode"] = "normal"; // 示例
                    motors.push_back(motor);
                } 
                catch (const std::exception &e) 
                {
                    RCLCPP_ERROR(this->get_logger(), "Error parsing motor data: %s", e.what());
                }


            motor_status["motor"] = motors;
            last_motor_status_ = motor_status.dump(); // 保存最后状态
            return motor_status;
        } 
        else 
        {
            RCLCPP_WARN(this->get_logger(), "Serial port is not open");
            return json{};
        }
    }

    std::string extractAndCombineHexValues(const std::string& hexString) 
    {
        size_t pos = hexString.find("0x");
        std::string formattedString = (pos != std::string::npos) ? hexString.substr(pos + 2) : hexString;

        std::istringstream ss(formattedString);
        std::vector<std::string> hexValues;
        std::string item;

        // 使用循环提取十六进制数
        while (ss >> item) 
        {
            hexValues.push_back(item);
        }

        // 检查是否有足够的元素来提取
        if (hexValues.size() < 8) {
            throw std::invalid_argument("Not enough hex values in the string.");
        }

        // 提取 "AB" 和 "CD"，并合并成 "ABCD"
        return hexValues[7] + hexValues[6];
    }

    // 解析合并后的十六进制字符串
    std::int32_t parseHexToDecimal(const std::string& hexValue) 
    {
        return std::stoi(hexValue, nullptr, 16);
    }




    void publish_motor_status() 
    {
        // 发布电机状态到 ROS 主题
        std_msgs::msg::String msg;
        msg.data = last_motor_status_;
        status_publisher_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published motor status: %s", msg.data.c_str());
    }

    std::string get_current_time() 
    {
        // 获取当前时间，并返回为 ISO 8601 字符串，您可以用 chrono 库实现
        return "2025-01-06T07:11:08Z"; // 示例时间
    }

    serial::Serial serial_port_;
    std::string last_motor_status_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorStatusNode>());
    rclcpp::shutdown();
    return 0;
}