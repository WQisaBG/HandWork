#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <sstream>
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <nlohmann/json.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/executor.hpp>
#include <std_msgs/msg/string.hpp>
#include "httplib.h"
#include "serial/serial.h"
#include "motor_control_command_msgs/msg/motor_control_command.hpp"
#include "motor_control_command_msgs/msg/motor.hpp"

using json = nlohmann::json;
using namespace std;

class HttpServerNode : public rclcpp::Node {
public:
    HttpServerNode()
        : Node("http_server_node"), 
        stop_server_(false), 
        serial_port_("/dev/ttyV20", 115200, serial::Timeout::simpleTimeout(1000)) 
    {
        pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);
        status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

        RCLCPP_INFO(this->get_logger(), "HTTP server starting on http://127.0.0.1:10088/motor/task");

        // 启动HTTP服务器线程
        http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);

        // 打开串口
        if (!serial_port_.isOpen()) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");

        // 启动定时器以发布电机状态
        timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&HttpServerNode::publish_motor_status, this));
    }

    ~HttpServerNode() 
    {
        stop_server_ = true;
        if (http_server_thread_.joinable())
        {
            http_server_thread_.join();
        }
    }

private:
    void start_http_server() 
    {
        httplib::Server server;

        server.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
            handle_post(req, res);
        });

        server.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) {
            // 查询电机状态并发送 JSON 响应
            json status = query_motor_status();
            res.set_content(status.dump(), "application/json");
        });

        while (!stop_server_) 
        {
            server.listen("127.0.0.1", 10088);
        }
    }

    void handle_post(const httplib::Request &req, httplib::Response &res) 
    {
        std::lock_guard<std::mutex> lock(request_mutex_); 
        try {
            auto json_data = json::parse(req.body);
            RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

            if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) 
            {
                motor_control_command_msgs::msg::MotorControlCommand msg;

                msg.id = json_data["id"];
                msg.timestamp = json_data["timestamp"];
                const auto& motors = json_data["motor"];
                for (const auto& motor : motors)
                {
                    motor_control_command_msgs::msg::Motor motor_msg;
                    motor_msg.index = motor["index"];
                    motor_msg.target_position = motor["targetPosition"];
                    msg.motors.push_back(motor_msg);
                }

                pub_->publish(msg);    //将转换后的数据,发布到topic
                RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu",
                            msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());

                // 将接受到的指令进行转换发送对对应的电缸
                for (const auto &motor : msg.motors) 
                {
                    std::string motor_command = transform_command(motor.index, motor.target_position);
                    send_command_to_motor(motor.index, motor_command);
                }

                res.set_content("{\"status\":\"received\"}", "application/json");
            } 
            else
            {
                throw std::invalid_argument("Invalid JSON structure");
            }
        } 
        catch (const std::exception &ex) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
        }
    }

    json query_motor_status()      //该状态查询是查询所有电缸的状态
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
            std::int32_t motor_num = 5;
            json motor;
            for(std::int32_t i = 0; i < motor_num; i++)
            {
                try 
                {
                    motor["index"] = index++;
                    motor["currentPosition"] = decimalValue;   //从电缸获取
                    motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 从电缸获取
                    motor["error"] = "No error"; 
                    motor["mode"] = "normal"; // 从电缸获取
                    motors.push_back(motor);
                } 
                catch (const std::exception &e) 
                {
                    RCLCPP_ERROR(this->get_logger(), "Error parsing motor data: %s", e.what());
                }
            }

            motor_status["motor"] = motors;
            last_motor_status_ = motor_status.dump(); // last state
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

    std::string get_current_time() 
    {
            // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        
        // 转换为 time_t
        auto now_time = std::chrono::system_clock::to_time_t(now);
    
        // 使用 std::gmtime 获取 UTC 时间
        auto utc_tm = *std::gmtime(&now_time); 

        // 将时间偏移 8 小时，以适应中国标准时间
        utc_tm.tm_hour += 8;

        // 修正可能的日期溢出, 如果时间超过24小时，进行日期调整
        if (utc_tm.tm_hour >= 24) {
            utc_tm.tm_hour -= 24;
            utc_tm.tm_mday += 1; // 增加天数
        }

        // 格式化时间为 ISO 8601
        std::ostringstream oss;
        oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%S") << "CST"; // 添加 CST 表示中国标准时间

        return oss.str();
    }

    void publish_motor_status() 
    {
        std_msgs::msg::String msg;
        msg.data = last_motor_status_;  // Update with actual motor status
        status_publisher_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published motor status: %s", msg.data.c_str());

    }

    //将指令转换成电缸可识别的指令.
    std::string transform_command(std::int32_t motor_index, std::int32_t target_position) 
    {
        std::string frame_data = "55 AA 04";
        std::string motor_id = decimalToHex(motor_index);
        std::string control_mode = "21";
        std::string target_pose = decimalToHex(target_position);
        std::string target_pose_command = reverseFormattedHexPairs(target_pose);
        std::string check_data = "76";
        std::string command = frame_data + " " + motor_id + " " + control_mode + " " +
                                   target_pose_command + " " + check_data;
        return command; 
    }

    // 函数将十进制整数转换为十六进制字符串
    std::string decimalToHex(int decimalNumber) 
    {
        if (decimalNumber == 0)
        {
            return "00"; // 处理特别情况
        }
        std::string hexStr; // 存储十六进制字符串
        while (decimalNumber > 0) 
        {
            int remainder = decimalNumber % 16; // 取余，获取当前位的值
            if (remainder < 10) 
            {
                hexStr = char(remainder + '0') + hexStr; // 0-9 转换为字符
            } 
            else 
            {
                hexStr = char(remainder - 10 + 'A') + hexStr; // 10-15 转换为 A-F
            }
            decimalNumber /= 16; 
        }
        // 确保长度为偶数，不足两位的前面补零
        if (hexStr.length() % 2 != 0) 
        {
            hexStr = "0" + hexStr; 
        }
        return formatHexToPairs(hexStr); // 返回构建的十六进制字符串
    }

    std::string formatHexToPairs(const std::string& hexStr) 
    {
        std::ostringstream formattedHex;
        for (size_t i = 0; i < hexStr.length(); i += 2) 
        {
            if (i > 0) 
            {
                formattedHex << " "; // 每组前加空格，除了第一组
            }
            formattedHex << hexStr.substr(i, 2); // 每次取两位
        }
        return formattedHex.str();
    }

    std::string reverseFormattedHexPairs(const std::string& formattedHex) 
    {
        std::istringstream iss(formattedHex);
        std::vector<std::string> pairs;
        std::string pair;

        while (iss >> pair) 
        {
            pairs.push_back(pair); // 将每个部分存入 vector
        }

        // 反转 vector 中的元素顺序
        std::reverse(pairs.begin(), pairs.end());

        // 合并反转后的部分
        std::ostringstream reversedHex;
        for (size_t i = 0; i < pairs.size(); ++i) 
        {
            if (i > 0) 
            {
                reversedHex << " "; // 每组前加空格，除了第一组
            }
            reversedHex << pairs[i]; // 添加每一个反转后的部分
        }
        return reversedHex.str();
    }






    void send_command_to_motor(std::int32_t index, std::string command) 
    {
        serial_port_.write(command + "\n");
        RCLCPP_INFO(this->get_logger(), "Sent command to motor: '%s'", command.c_str());
    }

    rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    serial::Serial serial_port_;
    std::string last_motor_status_ = "Motor status initialized"; // Example initialization
    std::mutex request_mutex_;
    std::thread http_server_thread_;
    std::atomic<bool> stop_server_;
    rclcpp::TimerBase::SharedPtr timer_;
};


int main(int argc, char *argv[]) 
{
    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;
    auto http_server_node = std::make_shared<HttpServerNode>();
    executor.add_node(http_server_node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}