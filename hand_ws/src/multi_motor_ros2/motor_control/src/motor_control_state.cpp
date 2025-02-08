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
#include <condition_variable>
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

class FeedbackQueue 
{
    public:
        void push(const std::pair<int, std::string> &feedback) 
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                feedbacks_.push(feedback);
            }
            cv_.notify_one(); 
        }

        std::pair<int, std::string> pop() 
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return !feedbacks_.empty(); });
            std::pair<int, std::string> feedback = feedbacks_.front();
            feedbacks_.pop();
            return feedback;
        }

        bool is_empty() 
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return feedbacks_.empty();
        }

    private:
        std::queue<std::pair<int, std::string>> feedbacks_;
        std::mutex mutex_;
        std::condition_variable cv_;
};

class HttpServerNode : public rclcpp::Node {
public:
    HttpServerNode()
        : Node("http_server_node"), 
          stop_server_(false),
          serial_port_("/dev/ttyUSB0", 921600, serial::Timeout::simpleTimeout(1000)) 
    {
        pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);
        status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

        RCLCPP_INFO(this->get_logger(), "HTTP server starting on http://127.0.0.1:10088/motor/task");
        
        http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);  //开启服务

        if (!serial_port_.isOpen()) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");

        // 状态发布定时器
        timer_ = this->create_wall_timer(std::chrono::seconds(1), 
            std::bind(&HttpServerNode::publish_motor_status, this));

        feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);

        request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);  // 启动请求处理线程

    }

    ~HttpServerNode() 
    {
        stop_server_ = true; 
        if (http_server_thread_.joinable()) 
        {
            http_server_thread_.join();
        }
        if (feedback_thread_.joinable()) 
        {
            feedback_thread_.join();
        }
        if (request_processing_thread_.joinable())  // 停止请求处理线程
        {
            request_processing_thread_.join();
        }
        if (serial_port_.isOpen()) 
        {
            serial_port_.close(); 
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
            process_feedback();
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
        try 
        {
            auto json_data = json::parse(req.body);
            RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

            if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) 
            {
                request_queue_.push(json_data);  // 将请求放入队列
                request_cv_.notify_one();  // 通知请求处理线程
                res.set_content("{\"status\":\"received\"}", "application/json");
            } 
            else {
                throw std::invalid_argument("Invalid JSON structure");
            }
        } 
        catch (const std::exception &ex) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
        }
    }

    void process_requests() 
    {
        while (!stop_server_) 
        {
            std::unique_lock<std::mutex> lock(request_mutex_);
            request_cv_.wait(lock, [this] { return !request_queue_.empty() || stop_server_; });
            if (stop_server_) break;

            auto json_data = request_queue_.front();
            request_queue_.pop();  // 从队列中移除请求

            lock.unlock();  // 尽量减少锁的持有时间
            process_request(json_data);  // 处理请求
        }
    }

    void process_request(const json& json_data) 
    {
        send_request_topic(json_data);
        send_request_motors(json_data);
    }

    void send_request_topic(const json& json_data)
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

        pub_->publish(msg);    //将解析后的信息  发布到主题
        RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu",
                                        msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
    }

    void send_request_motors(const json& json_data)
    {
        std::vector<uint8_t> trans_cmd;
        trans_cmd.reserve(256); // 预留空间以避免多次分配

        const auto& motors = json_data["motor"];
        size_t motor_num = motors.size();

        trans_cmd.push_back(0x55);
        trans_cmd.push_back(0xAA);
        trans_cmd.push_back((motor_num * 3 + 1) & 0xFF);
        trans_cmd.push_back(0xFF);
        trans_cmd.push_back(0xF2);

        for (const auto& motor : motors)
        {
            uint8_t motor_index = motor["index"];
            uint16_t target_position = motor["targetPosition"];
            trans_cmd.push_back(motor_index);
            trans_cmd.push_back(target_position & 0xFF);
            trans_cmd.push_back((target_position >> 8) & 0xFF);
        }

        uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
        trans_cmd.push_back(checkSum);


        size_t bytes_to_write = trans_cmd.size();
        RCLCPP_INFO(this->get_logger(), "Writing %zu bytes to serial port", bytes_to_write);

        // 发送命令
        size_t bytes_written = serial_port_.write(trans_cmd.data(), bytes_to_write);
        if (bytes_written != bytes_to_write) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", bytes_to_write, bytes_written);
        } 
        else 
        {
            RCLCPP_INFO(this->get_logger(), "Successfully wrote %zu bytes to serial port", bytes_written);
            printCommand(trans_cmd.data(), trans_cmd.size());

        }

    }


    json query_motor_status() {
        std::int32_t decimalValue = 0;
        if (serial_port_.isOpen()) {
            std::string result = "aa 55 11 02 21 37 f8 fd d5 df e3 37 ea e8 03 e6 47 be 35 17 bd d0 a2 08 a3 08 2a";
            try {
                std::string combinedHex = extractAndCombineHexValues(result);
                decimalValue = parseHexToDecimal(combinedHex);
                RCLCPP_INFO(this->get_logger(), "Combined Hex value: %s, Decimal value: %d", combinedHex.c_str(), decimalValue);
            } catch (const std::invalid_argument& e) {
                RCLCPP_ERROR(this->get_logger(), "Error: %s", e.what());
            }

            json motor_status;
            motor_status["timestamp"] = get_current_time();

            std::vector<json> motors;
            std::int32_t motor_num = 5; // Hypothetical motor count
            for (std::int32_t i = 0; i < motor_num; i++) {
                json motor;
                motor["index"] = i;
                motor["currentPosition"] = decimalValue; // 从电缸获取
                motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 从电缸获取
                motor["error"] = "No error"; 
                motor["mode"] = "normal"; // 从电缸获取
                motors.push_back(motor);
            }
            motor_status["motor"] = motors;
            last_motor_status_ = motor_status.dump(); // Store last status
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
        std::istringstream ss(hexString);
        std::vector<std::string> hexValues;
        std::string item;
        while (ss >> item) 
        {
            hexValues.push_back(item);
        }
        if (hexValues.size() < 8) 
        {
            throw std::invalid_argument("Not enough hex values in the string.");
        }
        return hexValues[7] + hexValues[6]; // Combine specific hex values
    }

    std::int32_t parseHexToDecimal(const std::string& hexValue) 
    {
        return std::stoi(hexValue, nullptr, 16);
    }

    std::string get_current_time() 
    {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        auto utc_tm = *std::gmtime(&now_time); 
        utc_tm.tm_hour += 8;

        if (utc_tm.tm_hour >= 24) 
        {
            utc_tm.tm_hour -= 24;
            utc_tm.tm_mday += 1; 
        }

        std::ostringstream oss;
        oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%S") << "CST";
        return oss.str();
    }

    void publish_motor_status() 
    {
        std_msgs::msg::String msg;
        msg.data = last_motor_status_;
        status_publisher_->publish(msg);
    }



    uint8_t calculateChecksum(const uint8_t* data, size_t length) 
    {
        uint16_t checksum = 0;  
        for (size_t i = 0; i < length; ++i) 
        {
            checksum += data[i];
        }
        return static_cast<uint8_t>(checksum & 0xFF); 
    }

    // void send_command_to_motor(const std::vector<uint8_t>& command) 
    // {
    //     // 检查串口是否打开
    //     if (!serial_port_.isOpen()) {
    //         RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
    //         return;
    //     }

    //     // 检查命令是否为空
    //     if (command.empty()) {
    //         RCLCPP_ERROR(this->get_logger(), "Command vector is empty");
    //         return;
    //     }


    //     // 发送命令
    //     serial_port_.write(command.data(), command.size());


    //     printCommand(command.data(), command.size());

    //     // 通知 feedback_listener 开始读取反馈
    //     {
    //         std::lock_guard<std::mutex> lock(command_mutex_); 
    //         command_sent_ = true;
    //     }
    //     command_cv_.notify_one();
    //     RCLCPP_INFO(this->get_logger(), "Command sent for motor %d, notifying feedback listener");
    //     // std::this_thread::sleep_for(std::chrono::milliseconds(90)); //延时   后面看去掉之后行不行?
    // }

    void printCommand(const uint8_t* command, size_t length) 
    {
        std::cout << "Command: ";
        for (size_t i = 0; i < length; ++i) 
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " "; 
        }
        std::cout << std::dec << std::endl; 
    }

    void feedback_listener() 
    {
        while (!stop_server_) 
        {
            std::unique_lock<std::mutex> lock(command_mutex_);
            command_cv_.wait_for(lock, std::chrono::seconds(1), [this] { return command_sent_ || stop_server_; });
            if (stop_server_) break;

            int motor_index = current_motor_index_;
            std::string feedback;
            try {
                
                // 读取数据包头
                uint8_t header[2];
                size_t bytes_read = serial_port_.read(header, 2); // 读取2个字节的头
                if (bytes_read != 2 || header[0] != 0xAA || header[1] != 0x55) 
                {
                    continue; // 跳过无效的数据包头
                }

                // 读取数据体长度
                uint8_t length;
                bytes_read = serial_port_.read(&length, 1); // 读取1个字节的数据体长度
                if (bytes_read != 1) {
                    RCLCPP_ERROR(this->get_logger(), "Failed to read packet length");
                    continue; // 跳过无法读取长度的情况
                }

                // 读取数据体
                std::vector<uint8_t> data_body(length);
                bytes_read = serial_port_.read(data_body.data(), length); // 根据长度读取数据体
                if (bytes_read != length) {
                    RCLCPP_ERROR(this->get_logger(), "Incomplete packet body: expected %d bytes, got %zu bytes", length, bytes_read);
                    continue; // 跳过不完整的数据体
                }

                // 构建完整的反馈信息
                feedback.assign(reinterpret_cast<char*>(header), 2);
                feedback.push_back(static_cast<char>(length));
                feedback.append(reinterpret_cast<char*>(data_body.data()), length);

                if (!feedback.empty()) 
                {
                    feedback_queue_.push({motor_index, feedback}); 
                    parse_feedback(feedback); 
                }
            
            } catch (const std::exception &e) {
                RCLCPP_ERROR(this->get_logger(), "Error reading from serial port: %s", e.what());
                // 尝试重新连接或重试读取
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 短暂等待后重试
            }

            // 重置命令发送标志
            command_sent_ = false;
        }
    }

    void process_feedback() 
    {
        if (!feedback_queue_.is_empty()) 
        {
            auto feedback_pair = feedback_queue_.pop();
            int motor_index = feedback_pair.first;
            std::string feedback = feedback_pair.second;
            parse_feedback(feedback); 
            RCLCPP_INFO(this->get_logger(), "Processing feedback for motor %d: %s", motor_index, feedback.c_str());
        }
    }

    void parse_feedback(const std::string &feedback) 
    {
        std::cout << "Parsing feedback: ";
        
        // 遍历反馈字符串的每个字符，并以十六进制输出
        for (unsigned char c : feedback) 
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                    << static_cast<int>(c) << " "; // 格式化输出为十六进制
        }

        std::cout <<"\n" << std::endl; // 恢复为十进制输出格式
        // 这里可以添加其他解析逻辑
    }

    FeedbackQueue feedback_queue_; 
    rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    serial::Serial serial_port_;
    std::string last_motor_status_ = "Motor status initialized";
    // std::mutex request_mutex_;
    std::thread http_server_thread_;
    std::thread feedback_thread_;
    std::atomic<bool> stop_server_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::mutex command_mutex_;
    std::condition_variable command_cv_;
    std::atomic<bool> command_sent_ = false;
    std::atomic<int> current_motor_index_ = -1;
    std::mutex feedback_mutex_;

    std::queue<json> request_queue_;  // 请求队列
    std::mutex request_mutex_;
    std::condition_variable request_cv_;
    std::thread request_processing_thread_;  // 请求处理线程

};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;
    auto http_server_node = std::make_shared<HttpServerNode>();
    executor.add_node(http_server_node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}