
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


class FeedbackQueue {
public:
    void push(const std::pair<int, std::string> &feedback) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            feedbacks_.push(feedback);
        }
        cv_.notify_one(); 
    }

    std::pair<int, std::string> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !feedbacks_.empty(); });
        std::pair<int, std::string> feedback = feedbacks_.front();
        feedbacks_.pop();
        return feedback;
    }

    bool is_empty() {
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
          serial_port_("/dev/ttyUSB0", 921600, serial::Timeout::simpleTimeout(1000)) {
        pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);
        status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

        RCLCPP_INFO(this->get_logger(), "HTTP server starting on http://127.0.0.1:10088/motor/task");

        // 初始化电机状态
        initialize_motor_status();
         // 等待电机状态初始化完成
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 启动线程
        http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);
        feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);
        request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);
        motor_status_query_thread_ = std::thread(&HttpServerNode::heartbeat_motor_status_query_thread, this); // 使用心跳机制

        if (!serial_port_.isOpen()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
        timer_ = this->create_wall_timer(std::chrono::milliseconds(100),
            std::bind(&HttpServerNode::publish_motor_status, this));
    }

    ~HttpServerNode() {
        stop_server_ = true;
        if (http_server_thread_.joinable()) http_server_thread_.join();
        if (feedback_thread_.joinable()) feedback_thread_.join();
        if (request_processing_thread_.joinable()) request_processing_thread_.join();
        if (motor_status_query_thread_.joinable()) motor_status_query_thread_.join();
        if (serial_port_.isOpen()) serial_port_.close();
    }

    json get_motor_status() 
    {
        std::lock_guard<std::mutex> lock(motor_status_mutex_);
        return motor_status_;
    }

private:
    void start_http_server() {
        httplib::Server server;

        server.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
            handle_post(req, res);
        });

        server.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) {
            json status = get_motor_status();
            res.set_content(status.dump(), "application/json");
        });

        while (!stop_server_) {
            server.listen("127.0.0.1", 10088);
        }
    }

    void initialize_motor_status() 
    {
        // 初始化电机状态
        motor_status_["motor"] = json::array(); // 确保 "motor" 是一个数组

        for (uint8_t motor_index = 2; motor_index < 4; motor_index++) {
            std::vector<uint8_t> command = create_motor_status_query_command(motor_index);
            send_serial_command(command);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待响应
        }
        RCLCPP_INFO(this->get_logger(), "Motor status initialization completed.");
    } 

    void handle_post(const httplib::Request &req, httplib::Response &res) 
    {
        std::lock_guard<std::mutex> lock(request_mutex_);
        try {
            auto json_data = json::parse(req.body);
            RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

            if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) {
                request_queue_.push(json_data);
                request_cv_.notify_one();
                res.set_content("{\"status\":\"received\"}", "application/json");
            } else {
                throw std::invalid_argument("Invalid JSON structure");
            }
        } catch (const std::exception &ex) {
            RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
            res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
        }
    }

    void process_requests() {
        while (!stop_server_) {
            std::unique_lock<std::mutex> lock(request_mutex_);
            request_cv_.wait(lock, [this] { return !request_queue_.empty() || stop_server_; });
            if (stop_server_) break;

            auto json_data = request_queue_.front();
            request_queue_.pop();
            lock.unlock();
            process_request(json_data);
        }
    }

    void process_request(const json& json_data) 
    {
        send_request_topic(json_data);
        send_request_motors(json_data);
    }

    void send_request_topic(const json& json_data) {
        motor_control_command_msgs::msg::MotorControlCommand msg;
        msg.id = json_data["id"];
        msg.timestamp = json_data["timestamp"];
        
        const auto& motors = json_data["motor"];
        for (const auto& motor : motors) {
            motor_control_command_msgs::msg::Motor motor_msg;
            motor_msg.index = motor["index"];
            motor_msg.target_position = motor["targetPosition"];
            msg.motors.push_back(motor_msg);
        }

        pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu", 
                    msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
    }

    void send_request_motors(const json& json_data) 
    {
        std::vector<uint8_t> trans_cmd = {0x55, 0xAA};
        const auto& motors = json_data["motor"];
        size_t motor_num = motors.size();
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
        
        send_serial_command_async(trans_cmd);
    }

//    void send_request_motors(const json& json_data) 
// {
//     std::vector<uint8_t> trans_cmd = {0x55, 0xAA};
//     const auto& motors = json_data["motor"];
//     size_t motor_num = motors.size();
//     trans_cmd.push_back((motor_num * 3 + 1) & 0xFF);
//     trans_cmd.push_back(0xFF);
//     trans_cmd.push_back(0xF2);

//     for (const auto& motor : motors) 
//     {
//         uint8_t motor_index = motor["index"];
//         uint16_t target_position = motor["targetPosition"];

//         // 等待电机状态更新
//         bool motor_found = false;
//         for (int attempt = 0; attempt < 20; ++attempt) { // 增加尝试次数
//             std::lock_guard<std::mutex> lock(motor_status_mutex_);
//             auto& motors = motor_status_["motor"];
//             for (const auto& m : motors) {
//                 if (m["ID"] == motor_index) {
//                     motor_found = true;
//                     break;
//                 }
//             }
//             if (motor_found) break;
//             std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待100毫秒
//         }

//         if (!motor_found) {
//             RCLCPP_ERROR(this->get_logger(), "Motor index %d not found in motor status after multiple attempts", motor_index);
//             continue;
//         }

//         std::lock_guard<std::mutex> lock(motor_status_mutex_);
//         auto& motors = motor_status_["motor"];
//         uint16_t current_position = 0;
//         for (const auto& m : motors) {
//             if (m["ID"] == motor_index) {
//                 current_position = m["currentPosition"];
//                 break;
//             }
//         }

//         // 计算位置差
//         int16_t position_diff = target_position - current_position;

//         int num_steps = 10; // 分割成10份
//         int step_size = position_diff / num_steps;
//         int remaining = position_diff % num_steps;

//         // 逐份发送
//         for (int i = 0; i < num_steps; ++i) {
//             int step_target = current_position + step_size * (i + 1) + (i < remaining ? 1 : 0);
//             trans_cmd[3] = motor_index;
//             trans_cmd[4] = step_target & 0xFF;
//             trans_cmd[5] = (step_target >> 8) & 0xFF;

//             uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
//             trans_cmd.push_back(checkSum);

//             send_serial_command_async(trans_cmd);
            
//             trans_cmd.pop_back(); // 移除校验和以便下一次使用

//             // 延迟一段时间以确保电机能够处理每个命令
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
//         }
//     }
// }

    void publish_motor_status() {
        std_msgs::msg::String msg;
        msg.data = motor_status_.dump();
        status_publisher_->publish(msg);
    }

    void heartbeat_motor_status_query_thread() 
    {
        using namespace std::chrono;
        const auto heartbeat_interval = milliseconds(10); // 频率
        
        while (!stop_server_) 
        {
            std::this_thread::sleep_for(heartbeat_interval);

            // 执行状态查询
            for (uint8_t motor_index = 2; motor_index < 4; motor_index++) 
            {
                std::vector<uint8_t> command = create_motor_status_query_command(motor_index);
                send_serial_command_async(command);
            }
        }
    }

    void send_serial_command_async(const std::vector<uint8_t>& command) 
    {
        std::async(std::launch::async, [this, command]() {
            send_serial_command(command);
        });
    }

    void send_serial_command(const std::vector<uint8_t>& command) 
    {
        if (!serial_port_.isOpen()) {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }

        size_t bytes_to_write = command.size();
        size_t bytes_written = serial_port_.write(command.data(), bytes_to_write);
        
        if (bytes_written != bytes_to_write) {
            RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", 
                        bytes_to_write, bytes_written);
        }
    }

    std::vector<uint8_t> create_motor_status_query_command(uint8_t motor_index) {
        std::vector<uint8_t> command = {0x55, 0xAA, 0x03, motor_index, 0x04, 0x00, 0x22};
        uint8_t checkSum = calculateChecksum(command.data() + 2, command.size() - 1);
        command.push_back(checkSum);
        return command;
    }

    uint8_t calculateChecksum(const uint8_t* data, size_t length) {
        uint16_t checksum = 0;
        for (size_t i = 0; i < length; ++i) {
            checksum += data[i];
        }
        return static_cast<uint8_t>(checksum & 0xFF);
    }

    void feedback_listener() 
    {
        std::vector<uint8_t> buffer;
        while (!stop_server_) 
        {
            process_feedback_from_serial(buffer);
        }
    }

    void process_feedback_from_serial(std::vector<uint8_t>& buffer) 
    {
        if (!serial_port_.isOpen()) 
        {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }

        // 增加缓冲区大小
        std::vector<uint8_t> new_data(2048); // 增加缓冲区大小
        size_t bytes_read = serial_port_.read(new_data.data(), new_data.size());
        if (bytes_read > 0) {
            buffer.insert(buffer.end(), new_data.begin(), new_data.begin() + bytes_read);
        }

        bool synchronized = false;
        while (buffer.size() >= 2 && !synchronized) {
            if (buffer[0] == 0xAA && buffer[1] == 0x55) {
                synchronized = true;
            } else {
                buffer.erase(buffer.begin());
            }
        }

        while (buffer.size() >= 4) 
        {
            if (buffer[0] == 0xAA && buffer[1] == 0x55) 
            {
                uint8_t length = buffer[2];

                if (length + 3 + 1 <= buffer.size()) 
                {
                    std::string feedback(reinterpret_cast<char*>(buffer.data()), 3 + length + 1);
                    buffer.erase(buffer.begin(), buffer.begin() + 3 + length + 1);

                    if (is_valid_feedback(feedback)) 
                    {
                        int motor_index = static_cast<unsigned char>(feedback[3]);
                        feedback_queue_.push({motor_index, feedback});
                        std::string deal_feedback = feedback_queue_.pop().second;
                        update_motor_status(deal_feedback);
                    } 
                    else 
                    {
                        // RCLCPP_WARN(this->get_logger(), "Invalid feedback detected and discarded: %s", feedback.c_str());
                    }
                }
                else 
                {
                    break; 
                }
            } 
            else 
            {
                buffer.erase(buffer.begin());
            }
        }
    }

    bool is_valid_feedback(const std::string& feedback) 
    {
        if (feedback.size() < 4) return false;
        uint8_t length = feedback[2];
        if (feedback.size() != 3 + length + 1) return false;

        int motor_id = static_cast<unsigned char>(feedback[3]);
        int current_position = (static_cast<unsigned char>(feedback[10]) << 8) | static_cast<unsigned char>(feedback[9]);
        int target_position = (static_cast<unsigned char>(feedback[8]) << 8) | static_cast<unsigned char>(feedback[7]);
        if (motor_id < 2 || motor_id > 3) return false;
        if (current_position < 0 || current_position > 2000) return false;
        if (target_position < 0 || target_position > 2000) return false;

        return true;
    }


void update_motor_status(const std::string &feedback) 
{
    if (feedback.size() < 8) {
        RCLCPP_ERROR(this->get_logger(), "Invalid feedback length: %zu", feedback.size());
        return;
    }

    int motor_index = static_cast<unsigned char>(feedback[3]);
    int current_position = (static_cast<unsigned char>(feedback[10]) << 8) | static_cast<unsigned char>(feedback[9]);
    int target_position = (static_cast<unsigned char>(feedback[8]) << 8) | static_cast<unsigned char>(feedback[7]);

    std::lock_guard<std::mutex> lock(motor_status_mutex_);

    auto& motors = motor_status_["motor"];
    bool found = false;

    for (auto& motor : motors) {
        if (motor["ID"] == motor_index) {
            motor["currentPosition"] = current_position;
            motor["targetPosition"] = target_position;
            motor["error"] = "No error";
            motor["mode"] = "normal";
            found = true;
            break;
        }
    }

    if (!found) {
        // 如果没有找到，则添加新的电机状态
        motors.push_back({
            {"ID", motor_index},
            {"currentPosition", current_position},
            {"targetPosition", target_position},
            {"error", "No error"},
            {"mode", "normal"}
        });
    }

    // RCLCPP_INFO(this->get_logger(), "Updated motor %d status: current=%d, target=%d", motor_index, current_position, target_position);
}

    FeedbackQueue feedback_queue_;
    rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    serial::Serial serial_port_;
    std::thread http_server_thread_, feedback_thread_, request_processing_thread_, motor_status_query_thread_;
    std::atomic<bool> stop_server_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::mutex command_mutex_;
    std::condition_variable command_cv_;
    std::mutex request_mutex_;
    std::condition_variable request_cv_;
    std::queue<json> request_queue_;
    std::atomic<int> current_motor_index_ = -1;

    std::mutex motor_status_mutex_;
    json motor_status_;
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

