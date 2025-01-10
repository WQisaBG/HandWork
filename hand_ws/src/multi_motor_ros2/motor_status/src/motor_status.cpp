#include <chrono>
#include <iostream>
#include <cstring>
#include <sstream>
#include <thread>
#include <memory>
#include <nlohmann/json.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "httplib.h"
#include "serial/serial.h"

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

            //状态查询的POST请求    使用post请求查询 是针对某一状态进行查询    后面添加具体的解析
            // svr.Post("/motor/status/query", [this](const httplib::Request &req, httplib::Response &res) 
            //  {
            //     // 解析 JSON 请求
            //     json request_json;
            //     try {
            //         request_json = json::parse(req.body);
            //     } catch (const std::exception &e) {
            //         res.status = 400; // Bad Request
            //         res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
            //         return;
            //     }

            //     // 提取任务ID和时间戳
            //     std::string task_id = request_json.value("id", "unknown");
            //     std::string timestamp = request_json.value("timestamp", "unknown");

            //     // 查找 motor 数组
            //     if (!request_json.contains("motor")) {
            //         res.status = 400; 
            //         res.set_content("{\"error\": \"Missing 'motor' field\"}", "application/json");
            //         return;
            //     }

            //     json response_json;
            //     for (const auto &motor : request_json["motor"]) {
            //         if (!motor.contains("index") || !motor.contains("params")) {
            //             continue; // 跳过无效的 motor 条目
            //         }

            //         int index = motor["index"].get<int>();
            //         std::vector<std::string> params = motor["params"].get<std::vector<std::string>>();

            //         json motor_status = get_motor_status(index, params);
            //         response_json["motor"].push_back({{"index", index}, {"status", motor_status}});
            //     }

            //     // 构建响应
            //     response_json["id"] = task_id;
            //     response_json["timestamp"] = timestamp;
            //     res.set_content(response_json.dump(), "application/json");
            // });

            svr.listen("127.0.0.1", 10088); //启动监听服务器
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


//=============================状态查询的POST=====================================
    // json get_motor_status(int index, const std::vector<std::string> &requested_params) 
    // {
    //     json status;

    //     // 假设您有一个函数来获取电机的实际状态
    //     if (serial_port_.isOpen()) {
    //         std::string command = "GET_STATUS " + std::to_string(index) + "\n"; // 发送查询命令
    //         serial_port_.write(command);

    //         std::string result = serial_port_.readline(100); // 读取串口响应
            
    //         // 示例解析：假设每个项是 "速度,位置,模式"
    //         std::istringstream ss(result);
    //         std::string item;
    //         std::vector<std::string> status_data;

    //         while (std::getline(ss, item, ',')) {
    //             status_data.push_back(item);
    //         }

    //         // 基于请求参数返回相应的状态
    //         for (const auto &param : requested_params) {
    //             if (param == "currentVelocity") {
    //                 status["currentVelocity"] = status_data.size() > 0 ? std::stof(status_data[0]) : 0.0;
    //             } else if (param == "currentPosition") {
    //                 status["currentPosition"] = status_data.size() > 1 ? std::stof(status_data[1]) : 0.0;
    //             } else if (param == "motionMode") {
    //                 status["motionMode"] = status_data.size() > 2 ? status_data[2] : "unknown";
    //             }
    //         }
    //     } 
    //     else 
    //     {
    //         RCLCPP_WARN(this->get_logger(), "Serial port is not open");
    //         return json{};
    //     }

    //     return status;
    // }
//=============================状态查询的POST=====================================

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