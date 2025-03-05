
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
#include "nlohmann/json.hpp"
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/executor.hpp>
#include <std_msgs/msg/string.hpp>
#include "httplib.h"
#include "serial/serial.h"
#include "motor_control_command_msgs/msg/motor_control_command.hpp"
#include "motor_control_command_msgs/msg/motor.hpp"

using json = nlohmann::json;


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

//=====================================================================================================================


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

        // 1、初始化电机状态
        initialize_motor_status();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // 2、启动线程
        http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);   //http server thread
        feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);
        request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);
        motor_status_query_thread_ = std::thread(&HttpServerNode::heartbeat_motor_status_query_thread, this); // 使用心跳机制

        if (!serial_port_.isOpen()) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
        timer_ = this->create_wall_timer(std::chrono::milliseconds(100),
            std::bind(&HttpServerNode::publish_motor_status, this));   //发布电机状态到主题

        control_timer_ = this->create_wall_timer(std::chrono::milliseconds(50),
            std::bind(&HttpServerNode::control_timer_callback, this));

        
    }

    ~HttpServerNode() 
    {
        stop_server_ = true;
        if (http_server_thread_.joinable()) http_server_thread_.join();
        if (feedback_thread_.joinable()) feedback_thread_.join();
        if (request_processing_thread_.joinable()) request_processing_thread_.join();
        if (motor_status_query_thread_.joinable()) motor_status_query_thread_.join();
        if (serial_port_.isOpen()) serial_port_.close();
    }

    json get_motor_status()    //获取电机状态 用于http_get请求
    {
        std::lock_guard<std::mutex> lock(motor_status_mutex_);
        return motor_status_;
    }

private:
    void start_http_server() 
    {
        httplib::Server server;

        server.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
            handle_post(req, res);
        });

        server.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) 
        {
            json status = get_motor_status();
            res.set_content(status.dump(), "application/json");
        });

        while (!stop_server_) 
        {
            server.listen("127.0.0.1", 10088);
        }
    }
//======================================================================================================================
    void initialize_motor_status() 
    {
        // 初始化电机状态
        motor_status_["motor"] = json::array(); // 确保 "motor" 是一个数组

        for (uint8_t motor_index = 1; motor_index < 4; motor_index++) 
        {
            std::vector<uint8_t> command = create_motor_status_query_command(motor_index);
            send_serial_command(command);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待响应
        }
        RCLCPP_INFO(this->get_logger(), "Motor status initialization completed.");
    } 
//======================================================================================================================

    void handle_post(const httplib::Request &req, httplib::Response &res) 
    {
        std::lock_guard<std::mutex> lock(request_mutex_);
        try {
            auto json_data = json::parse(req.body);
            RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

            if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) {
                request_queue_.push(json_data);   //将获取的POST请求 放到队列中
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
//======================================================================================================================


    void control_timer_callback() 
    {
        if(! current_request_.empty())
        {
            process_request(current_request_);
        }
    }
    void process_requests() 
    {
        while (!stop_server_) 
        {
            std::unique_lock<std::mutex> lock(request_mutex_);
            request_cv_.wait(lock, [this] { return !request_queue_.empty() || stop_server_; });
            if (stop_server_) break;

            auto json_data = request_queue_.front();    //从队列中取出一个请求  
            request_queue_.pop();                       // 从队列中移除该请求
            lock.unlock();
            current_request_ = json_data; // 将当前请求赋值给current_request_
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

        pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu", 
                    msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
    }

    // void send_request_motors(const json& json_data) 
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
    //         trans_cmd.push_back(motor_index);
    //         trans_cmd.push_back(target_position & 0xFF);
    //         trans_cmd.push_back((target_position >> 8) & 0xFF);
    //     }

    //     uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
    //     trans_cmd.push_back(checkSum);
        
    //     send_serial_command_async(trans_cmd);
    // }


//======================================================================================================================
    void send_request_motors(const json& json_data) 
    {
        // 获取电机列表
        const auto& motors = json_data["motor"];
        size_t motor_num = motors.size();

        // 初始化目标位置映射
        std::unordered_map<uint8_t, uint16_t> target_positions;
        for (const auto& motor : motors) 
        {
            uint8_t motor_index = motor["index"];
            uint16_t target_position = motor["targetPosition"];
            target_positions[motor_index] = target_position;
        }

        // 广播随动模式下发指令
        // while (true) 
        // {
            // 获取当前电机状态
            std::unordered_map<uint8_t, uint16_t> current_positions;
            {
                std::lock_guard<std::mutex> lock(motor_status_mutex_);
                auto& motor_list = motor_status_["motor"];
                for (const auto& motor : motor_list) 
                {
                    uint8_t motor_index = motor["ID"];
                    uint16_t current_position = motor["currentPosition"];
                    current_positions[motor_index] = current_position;
                }
            }

            // 检查是否所有电机都已到达目标位置
            if(are_all_motors_reached(current_positions, target_positions))
            {
                current_request_.clear();
                return;  //所有电机都已到达目标位置，退出循环
            }


            // 构造广播指令
            std::vector<uint8_t> trans_cmd = {0x55, 0xAA};
            trans_cmd.push_back((motor_num * 3 + 1) & 0xFF); // 指令长度
            trans_cmd.push_back(0xFF); // 固定字段
            trans_cmd.push_back(0xF2); // 指令类型

            for (const auto& [motor_index, target_position] : target_positions) 
            {
                uint16_t current_position = current_positions[motor_index];
                // 计算下一步目标位置
                int16_t position_diff = target_position - current_position;
                int step_target = current_position + (position_diff > 0 ? 250 : -205);
                if(step_target > 2000)
                {
                    step_target = 2000;
                }
                if (step_target < 0)
                {
                   step_target = 0;
                }
                
                // 添加电机索引和目标位置
                trans_cmd.push_back(motor_index);
                trans_cmd.push_back(step_target & 0xFF); // 目标位置低字节
                trans_cmd.push_back((step_target >> 8) & 0xFF); // 目标位置高字节
            }
            // 计算校验和
            uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
            trans_cmd.push_back(checkSum);
            // 异步发送广播指令
            send_serial_command_async(trans_cmd);

            // // 延迟一段时间以确保电机能够处理指令
            // std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // }
    }
//=====================================================================================================================

    bool are_all_motors_reached(const std::unordered_map<uint8_t, uint16_t>& target_positions,
                                const std::unordered_map<uint8_t, uint16_t>& current_positions) 
    {
        for (const auto& [motor_index, target_position] : target_positions) 
        {
            // 检查当前电机是否存在
            if (current_positions.find(motor_index) == current_positions.end()) 
            {
                RCLCPP_ERROR(this->get_logger(), "Motor index %d not found in current positions", motor_index);
                return false; // 如果某个电机的状态缺失，直接返回 false
            }

            // 计算位置差
            int16_t position_diff = target_position - current_positions.at(motor_index);
            if (std::abs(position_diff) > 1) 
            {
                return false; // 如果任意一个电机未到达目标位置，返回 false
            }
        }
        return true; // 所有电机都已到达目标位置
    }
//======================================================================================================================
    void publish_motor_status() 
    {
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
            for (uint8_t motor_index = 1; motor_index < 4; motor_index++) 
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
        if (!serial_port_.isOpen()) 
        {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }

        size_t bytes_to_write = command.size();
        size_t bytes_written = serial_port_.write(command.data(), bytes_to_write);
        
        if (bytes_written != bytes_to_write) 
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", 
                        bytes_to_write, bytes_written);
        }
    }

    std::vector<uint8_t> create_motor_status_query_command(uint8_t motor_index) 
    {
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

    void feedback_listener()   //监听反馈的状态
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

        // 读取新数据
        std::vector<uint8_t> new_data(1024);
        size_t bytes_read = serial_port_.read(new_data.data(), new_data.size());
        if (bytes_read == 0) return; // 没有读取到数据，直接返回

        // 查找同步头
        auto sync_header = std::search(new_data.begin(), new_data.begin() + bytes_read, 
                                    std::begin(sync_bytes), std::end(sync_bytes));
        if (sync_header == new_data.begin() + bytes_read) return; // 没有找到同步头，直接返回

        // 处理数据帧
        for (auto it = sync_header; it < new_data.begin() + bytes_read;) 
        {
            if (it + 3 >= new_data.begin() + bytes_read) break; // 数据不足，跳出循环

            uint8_t length = *(it + 2);
            if (it + 3 + length + 1 > new_data.begin() + bytes_read) break; // 数据帧不完整，跳出循环

            // 提取反馈数据
            std::vector<uint8_t> feedback(it, it + 3 + length + 1);
            it += 3 + length + 1;

            if (is_valid_feedback(feedback)) 
            {
                int motor_index = feedback[3];
                feedback_queue_.push({motor_index, std::string(feedback.begin(), feedback.end())});
                std::string deal_feedback = feedback_queue_.pop().second;
                update_motor_status(deal_feedback);
            } 
            else 
            {
                // RCLCPP_WARN(this->get_logger(), "Invalid feedback detected and discarded: %s", feedback.c_str());
            }
        }

        // 更新缓冲区
        buffer.insert(buffer.end(), new_data.begin() + bytes_read, new_data.end());
    }


    bool is_valid_feedback(const std::vector<uint8_t>& feedback) 
    {
        if (feedback.size() < 4) return false;
        uint8_t length = feedback[2];
        if (feedback.size() != 3 + length + 1) return false;

        if (feedback[0] != 0xAA || feedback[1] != 0x55) return false;
    

        int motor_id = static_cast<unsigned char>(feedback[3]);
        int current_position = (static_cast<unsigned char>(feedback[10]) << 8) | static_cast<unsigned char>(feedback[9]);
        int target_position = (static_cast<unsigned char>(feedback[8]) << 8) | static_cast<unsigned char>(feedback[7]);
        if (motor_id < 1 || motor_id > 6) return false;
        if (current_position < 0 || current_position > 2000) return false;
        if (target_position < 0 || target_position > 2000) return false;

        return true;
    }


void update_motor_status(const std::string &feedback) 
{
    if (feedback.size() < 8) 
    {
        RCLCPP_ERROR(this->get_logger(), "Invalid feedback length: %zu", feedback.size());
        return;
    }

    int motor_index = static_cast<unsigned char>(feedback[3]);
    int current_position = (static_cast<unsigned char>(feedback[10]) << 8) | static_cast<unsigned char>(feedback[9]);
    int target_position = (static_cast<unsigned char>(feedback[8]) << 8) | static_cast<unsigned char>(feedback[7]);

    std::lock_guard<std::mutex> lock(motor_status_mutex_);

    auto& motors = motor_status_["motor"];
    bool found = false;

    for (auto& motor : motors) 
    {
        if (motor["ID"] == motor_index) 
        {
            motor["currentPosition"] = current_position;
            motor["targetPosition"] = target_position;
            motor["error"] = "No error";
            motor["mode"] = "normal";
            found = true;
            break;
        }
    }

    if (!found) 
    {
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
    // 同步头定义
    const std::vector<uint8_t> sync_bytes = {0xAA, 0x55};

    std::mutex motor_status_mutex_;
    json motor_status_;

    rclcpp::TimerBase::SharedPtr control_timer_;
    json current_request_;
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

