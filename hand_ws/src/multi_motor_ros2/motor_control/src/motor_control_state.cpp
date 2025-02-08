// // #include <chrono>
// // #include <ctime>
// // #include <iomanip>
// // #include <iostream>
// // #include <cstring>
// // #include <sstream>
// // #include <thread>
// // #include <memory>
// // #include <queue>
// // #include <mutex>
// // #include <atomic>
// // #include <condition_variable>
// // #include <nlohmann/json.hpp>
// // #include <rclcpp/rclcpp.hpp>
// // #include <rclcpp/executor.hpp>
// // #include <std_msgs/msg/string.hpp>
// // #include "httplib.h"
// // #include "serial/serial.h"
// // #include "motor_control_command_msgs/msg/motor_control_command.hpp"
// // #include "motor_control_command_msgs/msg/motor.hpp"

// // using json = nlohmann::json;
// // using namespace std;

// // class FeedbackQueue 
// // {
// //     public:
// //         void push(const std::pair<int, std::string> &feedback) 
// //         {
// //             {
// //                 std::lock_guard<std::mutex> lock(mutex_);
// //                 feedbacks_.push(feedback);
// //             }
// //             cv_.notify_one(); 
// //         }

// //         std::pair<int, std::string> pop() 
// //         {
// //             std::unique_lock<std::mutex> lock(mutex_);
// //             cv_.wait(lock, [this] { return !feedbacks_.empty(); });
// //             std::pair<int, std::string> feedback = feedbacks_.front();
// //             feedbacks_.pop();
// //             return feedback;
// //         }

// //         bool is_empty() 
// //         {
// //             std::lock_guard<std::mutex> lock(mutex_);
// //             return feedbacks_.empty();
// //         }

// //     private:
// //         std::queue<std::pair<int, std::string>> feedbacks_;
// //         std::mutex mutex_;
// //         std::condition_variable cv_;
// // };

// // class HttpServerNode : public rclcpp::Node {
// // public:
// //     HttpServerNode()
// //         : Node("http_server_node"), 
// //           stop_server_(false),
// //           serial_port_("/dev/ttyUSB0", 921600, serial::Timeout::simpleTimeout(1000)) 
// //     {
// //         pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);
// //         status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

// //         RCLCPP_INFO(this->get_logger(), "HTTP server starting on http://127.0.0.1:10088/motor/task");
        
// //         http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);  //开启服务

// //         if (!serial_port_.isOpen()) 
// //         {
// //             RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
// //             rclcpp::shutdown();
// //             return;
// //         }

// //         RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");

// //         // 状态发布定时器
// //         timer_ = this->create_wall_timer(std::chrono::seconds(1), 
// //             std::bind(&HttpServerNode::publish_motor_status, this));

// //         request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);  // 启动POST请求处理线程

// //         // 添加: 启动电机状态查询线程
// //         motor_status_query_thread_ = std::thread(&HttpServerNode::periodic_motor_status_query_thread, this);
// //         feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);

// //     }

// //     ~HttpServerNode() 
// //     {
// //         stop_server_ = true; 
// //         if (http_server_thread_.joinable()) 
// //         {
// //             http_server_thread_.join();
// //         }
// //         if (feedback_thread_.joinable()) 
// //         {
// //             feedback_thread_.join();
// //         }
// //         if (request_processing_thread_.joinable())  // 停止请求处理线程
// //         {
// //             request_processing_thread_.join();
// //         }
// //         // 添加: 停止电机状态查询线程
// //         if (motor_status_query_thread_.joinable()) 
// //         {
// //             motor_status_query_thread_.join();
// //         }
// //         if (serial_port_.isOpen()) 
// //         {
// //             serial_port_.close(); 
// //         }
// //     }

// // private:
// //     void start_http_server() 
// //     {
// //         httplib::Server server; 

// //         server.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
// //             handle_post(req, res);
// //         });

// //         server.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) {
// //             process_feedback();
// //             json status = query_motor_status();
// //             res.set_content(status.dump(), "application/json");
// //         });

// //         while (!stop_server_) 
// //         {
// //             server.listen("127.0.0.1", 10088);
// //         }
// //     }

// //     void handle_post(const httplib::Request &req, httplib::Response &res) 
// //     {
// //         std::lock_guard<std::mutex> lock(request_mutex_); 
// //         try 
// //         {
// //             auto json_data = json::parse(req.body);
// //             RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

// //             if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) 
// //             {
// //                 request_queue_.push(json_data);  // 将请求放入队列
// //                 request_cv_.notify_one();  // 通知请求处理线程
// //                 res.set_content("{\"status\":\"received\"}", "application/json");
// //             } 
// //             else {
// //                 throw std::invalid_argument("Invalid JSON structure");
// //             }
// //         } 
// //         catch (const std::exception &ex) {
// //             RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
// //             res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
// //         }
// //     }

// //     void process_requests() 
// //     {
// //         while (!stop_server_) 
// //         {
// //             std::unique_lock<std::mutex> lock(request_mutex_);
// //             request_cv_.wait(lock, [this] { return !request_queue_.empty() || stop_server_; });
// //             if (stop_server_) break;

// //             auto json_data = request_queue_.front();
// //             request_queue_.pop();  // 从队列中移除请求

// //             lock.unlock();  // 尽量减少锁的持有时间
// //             process_request(json_data);  // 处理请求
// //         }
// //     }

// //     void process_request(const json& json_data) 
// //     {
// //         send_request_topic(json_data);
// //         send_request_motors(json_data);
// //     }

// //     void send_request_topic(const json& json_data)
// //     {
// //         motor_control_command_msgs::msg::MotorControlCommand msg;

// //         msg.id = json_data["id"];
// //         msg.timestamp = json_data["timestamp"];
// //         const auto& motors = json_data["motor"];
// //         for (const auto& motor : motors) 
// //         {
// //             motor_control_command_msgs::msg::Motor motor_msg;
// //             motor_msg.index = motor["index"];
// //             motor_msg.target_position = motor["targetPosition"];
// //             msg.motors.push_back(motor_msg);
// //         }

// //         pub_->publish(msg);    //将解析后的信息  发布到主题
// //         RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu",
// //                                         msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
// //     }

// //     void send_request_motors(const json& json_data)
// //     {
// //         std::vector<uint8_t> trans_cmd;
// //         trans_cmd.reserve(256); // 预留空间以避免多次分配

// //         const auto& motors = json_data["motor"];
// //         size_t motor_num = motors.size();

// //         trans_cmd.push_back(0x55);
// //         trans_cmd.push_back(0xAA);
// //         trans_cmd.push_back((motor_num * 3 + 1) & 0xFF);
// //         trans_cmd.push_back(0xFF);
// //         trans_cmd.push_back(0xF2);

// //         for (const auto& motor : motors)
// //         {
// //             uint8_t motor_index = motor["index"];
// //             uint16_t target_position = motor["targetPosition"];
// //             trans_cmd.push_back(motor_index);
// //             trans_cmd.push_back(target_position & 0xFF);
// //             trans_cmd.push_back((target_position >> 8) & 0xFF);
// //         }

// //         uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
// //         trans_cmd.push_back(checkSum);


// //         size_t bytes_to_write = trans_cmd.size();
// //         RCLCPP_INFO(this->get_logger(), "Writing %zu bytes to serial port", bytes_to_write);

// //         // 发送命令
// //         size_t bytes_written = serial_port_.write(trans_cmd.data(), bytes_to_write);
// //         if (bytes_written != bytes_to_write) 
// //         {
// //             RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", bytes_to_write, bytes_written);
// //         } 
// //         else 
// //         {
// //             RCLCPP_INFO(this->get_logger(), "Successfully wrote %zu bytes to serial port", bytes_written);
// //             printCommand(trans_cmd.data(), trans_cmd.size());

// //         }

// //     }

// //     // uint8_t calculateChecksum(const uint8_t* data, size_t length) 
// //     // {
// //     //     uint16_t checksum = 0;  
// //     //     for (size_t i = 0; i < length; ++i) 
// //     //     {
// //     //         checksum += data[i];
// //     //     }
// //     //     return static_cast<uint8_t>(checksum & 0xFF); 
// //     // }


// //     json query_motor_status() {
// //         std::int32_t decimalValue = 0;
// //         if (serial_port_.isOpen()) {
// //             std::string result = "aa 55 11 02 21 37 f8 fd d5 df e3 37 ea e8 03 e6 47 be 35 17 bd d0 a2 08 a3 08 2a";
// //             try {
// //                 std::string combinedHex = extractAndCombineHexValues(result);
// //                 decimalValue = parseHexToDecimal(combinedHex);
// //                 RCLCPP_INFO(this->get_logger(), "Combined Hex value: %s, Decimal value: %d", combinedHex.c_str(), decimalValue);
// //             } catch (const std::invalid_argument& e) {
// //                 RCLCPP_ERROR(this->get_logger(), "Error: %s", e.what());
// //             }

// //             json motor_status;
// //             motor_status["timestamp"] = get_current_time();

// //             std::vector<json> motors;
// //             std::int32_t motor_num = 5; // Hypothetical motor count
// //             for (std::int32_t i = 0; i < motor_num; i++) {
// //                 json motor;
// //                 motor["index"] = i;
// //                 motor["currentPosition"] = decimalValue; // 从电缸获取
// //                 motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 从电缸获取
// //                 motor["error"] = "No error"; 
// //                 motor["mode"] = "normal"; // 从电缸获取
// //                 motors.push_back(motor);
// //             }
// //             motor_status["motor"] = motors;
// //             last_motor_status_ = motor_status.dump(); // Store last status
// //             return motor_status;
// //         } 
// //         else 
// //         {
// //             RCLCPP_WARN(this->get_logger(), "Serial port is not open");
// //             return json{};
// //         }
// //     }

// //     std::string extractAndCombineHexValues(const std::string& hexString) 
// //     {
// //         std::istringstream ss(hexString);
// //         std::vector<std::string> hexValues;
// //         std::string item;
// //         while (ss >> item) 
// //         {
// //             hexValues.push_back(item);
// //         }
// //         if (hexValues.size() < 8) 
// //         {
// //             throw std::invalid_argument("Not enough hex values in the string.");
// //         }
// //         return hexValues[7] + hexValues[6]; // Combine specific hex values
// //     }

// //     std::int32_t parseHexToDecimal(const std::string& hexValue) 
// //     {
// //         return std::stoi(hexValue, nullptr, 16);
// //     }

// //     std::string get_current_time() 
// //     {
// //         auto now = std::chrono::system_clock::now();
// //         auto now_time = std::chrono::system_clock::to_time_t(now);
// //         auto utc_tm = *std::gmtime(&now_time); 
// //         utc_tm.tm_hour += 8;

// //         if (utc_tm.tm_hour >= 24) 
// //         {
// //             utc_tm.tm_hour -= 24;
// //             utc_tm.tm_mday += 1; 
// //         }

// //         std::ostringstream oss;
// //         oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%S") << "CST";
// //         return oss.str();
// //     }

// //     void publish_motor_status() 
// //     {
// //         std_msgs::msg::String msg;
// //         msg.data = last_motor_status_;
// //         status_publisher_->publish(msg);
// //     }


// //     // 添加: 定期查询电机状态的线程函数
// //     void periodic_motor_status_query_thread() 
// //     {
// //         while (!stop_server_) 
// //         {
// //             for (uint8_t motor_index = 2; motor_index < 4; motor_index++) // 修改电机索引范围
// //             {
// //                 std::vector<uint8_t> command;
// //                 command.push_back(0x55);
// //                 command.push_back(0xAA);
// //                 command.push_back(0x03);
// //                 command.push_back(motor_index);
// //                 command.push_back(0x04);
// //                 command.push_back(0x00);
// //                 command.push_back(0x22);
// //                 uint8_t checkSum = calculateChecksum(command.data() + 2, command.size() - 1);
// //                 command.push_back(checkSum);
// //                 send_command_to_motor(command, motor_index); // 传递电机索引
// //                 command.clear();
// //             }
// //             std::this_thread::sleep_for(std::chrono::seconds(1));
// //         }
// //     }


// //     // void periodic_motor_status_query() 
// //     // {
// //     //     uint8_t motor_num = 2;
// //     //     for (uint8_t motor_index = 2; motor_index < motor_num+2; motor_index++) 
// //     //     {
// //     //         std::vector<uint8_t> command;
// //     //         command.push_back(0x55);
// //     //         command.push_back(0xAA);
// //     //         command.push_back(0x03);
// //     //         command.push_back(motor_index);
// //     //         command.push_back(0x04);
// //     //         command.push_back(0x00);
// //     //         command.push_back(0x22);
// //     //         uint8_t checkSum = calculateChecksum(command.data() + 2, command.size() - 1);
// //     //         command.push_back(checkSum);
// //     //         send_command_to_motor(command);
// //     //         command.clear();
// //     //     }
// //     // }


// //     uint8_t calculateChecksum(const uint8_t* data, size_t length) 
// //     {
// //         uint16_t checksum = 0;  
// //         for (size_t i = 0; i < length; ++i) 
// //         {
// //             checksum += data[i];
// //         }
// //         return static_cast<uint8_t>(checksum & 0xFF); 
// //     }

// //     void send_command_to_motor(const std::vector<uint8_t>& command, uint8_t motor_index) 
// //     {
// //         // 检查串口是否打开
// //         if (!serial_port_.isOpen()) {
// //             RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
// //             return;
// //         }

// //         // 检查命令是否为空
// //         if (command.empty()) {
// //             RCLCPP_ERROR(this->get_logger(), "Command vector is empty");
// //             return;
// //         }

// //         // 发送命令
// //         size_t bytes_to_write = command.size();
// //         size_t bytes_written = serial_port_.write(command.data(), bytes_to_write);
// //         if (bytes_written != bytes_to_write) 
// //         {
// //             RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", bytes_to_write, bytes_written);
// //             return;
// //         }

// //         printCommand(command.data(), bytes_to_write);

// //         // 通知 feedback_listener 开始读取反馈
// //         {
// //             std::lock_guard<std::mutex> lock(command_mutex_); 
// //             command_sent_ = true;
// //             current_motor_index_ = motor_index; // 设置当前电机索引
// //         }
// //         command_cv_.notify_one();
// //         RCLCPP_INFO(this->get_logger(), "Command sent for motor %d, notifying feedback listener", motor_index);
// //     }

// //     void printCommand(const uint8_t* command, size_t length) 
// //     {
// //         std::cout << "Command: ";
// //         for (size_t i = 0; i < length; ++i) 
// //         {
// //             std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " "; 
// //         }
// //         std::cout << std::dec << std::endl; 
// //     }

// //     void feedback_listener() 
// //     {
// //         std::vector<uint8_t> buffer;
// //         while (!stop_server_) 
// //         {
// //             std::unique_lock<std::mutex> lock(command_mutex_);
// //             command_cv_.wait_for(lock, std::chrono::seconds(1), [this] { return command_sent_ || stop_server_; });
// //             if (stop_server_) break;

// //             try {
// //                 // 读取数据到缓冲区
// //                 std::vector<uint8_t> new_data(1024); // 假设每次最多读取1024字节
// //                 size_t bytes_read = serial_port_.read(new_data.data(), new_data.size());
// //                 if (bytes_read > 0) {
// //                     buffer.insert(buffer.end(), new_data.begin(), new_data.begin() + bytes_read);
// //                 }

// //                 // 处理缓冲区中的数据
// //                 while (buffer.size() >= 4) { // 至少需要4个字节来解析头部和长度
// //                     // 检查数据包头
// //                     if (buffer[0] == 0xAA && buffer[1] == 0x55) {
// //                         // 读取数据体长度
// //                         uint8_t length = buffer[2];

// //                         // 检查是否有足够的数据来解析整个数据包
// //                         if (buffer.size() >= 3 + length + 1) { // 3个字节头部 + length个字节数据 + 1个字节校验和
// //                             // 构建完整的反馈信息
// //                             std::string feedback(buffer.begin(), buffer.begin() + 3 + length + 1);

// //                             // 移除已处理的数据
// //                             buffer.erase(buffer.begin(), buffer.begin() + 3 + length + 1);

// //                             // 解析反馈
// //                             int motor_index = current_motor_index_; // 获取当前电机索引
// //                             feedback_queue_.push({motor_index, feedback}); // 使用当前电机索引
// //                             parse_feedback(feedback);
// //                         } else {
// //                             // 数据不足，等待更多数据
// //                             break;
// //                         }
// //                     } else {
// //                         // 数据包头不匹配，移除第一个字节并继续检查
// //                         buffer.erase(buffer.begin());
// //                     }
// //                 }

// //             } catch (const std::exception &e) {
// //                 RCLCPP_ERROR(this->get_logger(), "Error reading from serial port: %s", e.what());
// //                 // 尝试重新连接或重试读取
// //                 std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 短暂等待后重试
// //             }

// //             // 重置命令发送标志
// //             command_sent_ = false;
// //         }
// //     }
    
// //     void process_feedback() 
// //     {
// //         if (!feedback_queue_.is_empty()) 
// //         {
// //             auto feedback_pair = feedback_queue_.pop();
// //             int motor_index = feedback_pair.first;
// //             std::string feedback = feedback_pair.second;
// //             parse_feedback(feedback); 
// //             RCLCPP_INFO(this->get_logger(), "Processing feedback for motor %d: %s", motor_index, feedback.c_str());
// //         }
// //     }

// //     void parse_feedback(const std::string &feedback) 
// //     {
// //         std::cout << "Parsing feedback: ";
        
// //         // 遍历反馈字符串的每个字符，并以十六进制输出
// //         for (unsigned char c : feedback) 
// //         {
// //             std::cout << std::hex << std::setw(2) << std::setfill('0') 
// //                     << static_cast<int>(c) << " "; // 格式化输出为十六进制
// //         }

// //         std::cout <<"\n" << std::endl; // 恢复为十进制输出格式
// //         // 这里可以添加其他解析逻辑
// //     }

// //     FeedbackQueue feedback_queue_; 
// //     rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
// //     rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
// //     serial::Serial serial_port_;
// //     std::string last_motor_status_ = "Motor status initialized";
// //     std::thread http_server_thread_;
// //     std::thread feedback_thread_;
// //     std::atomic<bool> stop_server_;
// //     rclcpp::TimerBase::SharedPtr timer_;

// //     std::mutex command_mutex_;
// //     std::condition_variable command_cv_;
// //     std::atomic<bool> command_sent_ = false;
// //     std::atomic<int> current_motor_index_ = -1;
// //     std::mutex feedback_mutex_;

// //     std::queue<json> request_queue_;  // 请求队列
// //     std::mutex request_mutex_;
// //     std::condition_variable request_cv_;
// //     std::thread request_processing_thread_;  // 请求处理线程

// //     // 添加: 电机状态查询线程
// //     std::thread motor_status_query_thread_;
// // };

// // int main(int argc, char *argv[]) {
// //     rclcpp::init(argc, argv);
// //     rclcpp::executors::MultiThreadedExecutor executor;
// //     auto http_server_node = std::make_shared<HttpServerNode>();
// //     executor.add_node(http_server_node);
// //     executor.spin();
// //     rclcpp::shutdown();
// //     return 0;
// // }


// #include <chrono>
// #include <ctime>
// #include <iomanip>
// #include <iostream>
// #include <cstring>
// #include <sstream>
// #include <thread>
// #include <memory>
// #include <queue>
// #include <mutex>
// #include <atomic>
// #include <condition_variable>
// #include <nlohmann/json.hpp>
// #include <rclcpp/rclcpp.hpp>
// #include <rclcpp/executor.hpp>
// #include <std_msgs/msg/string.hpp>
// #include "httplib.h"
// #include "serial/serial.h"
// #include "motor_control_command_msgs/msg/motor_control_command.hpp"
// #include "motor_control_command_msgs/msg/motor.hpp"

// using json = nlohmann::json;

// class FeedbackQueue 
// {
//     public:
//         void push(const std::pair<int, std::string> &feedback) 
//         {
//             {
//                 std::lock_guard<std::mutex> lock(mutex_);
//                 feedbacks_.push(feedback);
//             }
//             cv_.notify_one(); 
//         }

//         std::pair<int, std::string> pop() 
//         {
//             std::unique_lock<std::mutex> lock(mutex_);
//             cv_.wait(lock, [this] { return !feedbacks_.empty(); });
//             std::pair<int, std::string> feedback = feedbacks_.front();
//             feedbacks_.pop();
//             return feedback;
//         }

//         bool is_empty() 
//         {
//             std::lock_guard<std::mutex> lock(mutex_);
//             return feedbacks_.empty();
//         }

//     private:
//         std::queue<std::pair<int, std::string>> feedbacks_;
//         std::mutex mutex_;
//         std::condition_variable cv_;
// };

// class HttpServerNode : public rclcpp::Node 
// {
//     public:
//         HttpServerNode()
//             : Node("http_server_node"), 
//             stop_server_(false),
//             serial_port_("/dev/ttyUSB0", 921600, serial::Timeout::simpleTimeout(1000)) 
//         {
//             pub_ = this->create_publisher<motor_control_command_msgs::msg::MotorControlCommand>("motor_command", 10);
//             status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

//             RCLCPP_INFO(this->get_logger(), "HTTP server starting on http://127.0.0.1:10088/motor/task");

//             // 启动线程
//             http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);
//             feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);
//             request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);
//             motor_status_query_thread_ = std::thread(&HttpServerNode::periodic_motor_status_query_thread, this);

//             if (!serial_port_.isOpen()) 
//             {
//                 RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
//                 rclcpp::shutdown();
//                 return;
//             }

//             RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
//             timer_ = this->create_wall_timer(std::chrono::seconds(1), 
//                 std::bind(&HttpServerNode::publish_motor_status, this));
//         }

//         ~HttpServerNode() 
//         {
//             stop_server_ = true;
//             if (http_server_thread_.joinable()) http_server_thread_.join();
//             if (feedback_thread_.joinable()) feedback_thread_.join();
//             if (request_processing_thread_.joinable()) request_processing_thread_.join();
//             if (motor_status_query_thread_.joinable()) motor_status_query_thread_.join();
//             if (serial_port_.isOpen()) serial_port_.close(); 
//         }

//     private:
//         void start_http_server() 
//         {
//             httplib::Server server; 

//             server.Post("/motor/task", [this](const httplib::Request &req, httplib::Response &res) {
//                 handle_post(req, res);
//             });

//             server.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) {
//                 json status = query_motor_status();
//                 res.set_content(status.dump(), "application/json");
//             });

//             while (!stop_server_) 
//             {
//                 server.listen("127.0.0.1", 10088);
//             }
//         }

//         void handle_post(const httplib::Request &req, httplib::Response &res) 
//         {
//             std::lock_guard<std::mutex> lock(request_mutex_);
//             try 
//             {
//                 auto json_data = json::parse(req.body);
//                 RCLCPP_INFO(this->get_logger(), "Received JSON data: %s", json_data.dump().c_str());

//                 if (json_data.contains("id") && json_data.contains("timestamp") && json_data.contains("motor")) {
//                     request_queue_.push(json_data);
//                     request_cv_.notify_one();
//                     res.set_content("{\"status\":\"received\"}", "application/json");
//                 } else {
//                     throw std::invalid_argument("Invalid JSON structure");
//                 }
//             } 
//             catch (const std::exception &ex) 
//             {
//                 RCLCPP_ERROR(this->get_logger(), "Failed to parse JSON: %s", ex.what());
//                 res.set_content("{\"status\":\"error\",\"message\":\"Invalid JSON\"}", "application/json");
//             }
//         }

//         void process_requests() 
//         {
//             while (!stop_server_) 
//             {
//                 std::unique_lock<std::mutex> lock(request_mutex_);
//                 request_cv_.wait(lock, [this] { return !request_queue_.empty() || stop_server_; });
//                 if (stop_server_) break;

//                 auto json_data = request_queue_.front();
//                 request_queue_.pop();
//                 lock.unlock();
//                 process_request(json_data);
//             }
//         }

//         void process_request(const json& json_data) 
//         {
//             send_request_topic(json_data);
//             send_request_motors(json_data);
//         }

//         void send_request_topic(const json& json_data) 
//         {
//             motor_control_command_msgs::msg::MotorControlCommand msg;
//             msg.id = json_data["id"];
//             msg.timestamp = json_data["timestamp"];
//             const auto& motors = json_data["motor"];
//             for (const auto& motor : motors) 
//             {
//                 motor_control_command_msgs::msg::Motor motor_msg;
//                 motor_msg.index = motor["index"];
//                 motor_msg.target_position = motor["targetPosition"];
//                 msg.motors.push_back(motor_msg);
//             }

//             pub_->publish(msg);
//             RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu", 
//                                                         msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
//         }

//         void send_request_motors(const json& json_data) 
//         {
//             // Prepare command
//             std::vector<uint8_t> trans_cmd = {0x55, 0xAA};
//             const auto& motors = json_data["motor"];
//             size_t motor_num = motors.size();
//             trans_cmd.push_back((motor_num * 3 + 1) & 0xFF);
//             trans_cmd.push_back(0xFF);
//             trans_cmd.push_back(0xF2);

//             for (const auto& motor : motors) 
//             {
//                 uint8_t motor_index = motor["index"];
//                 uint16_t target_position = motor["targetPosition"];
//                 trans_cmd.push_back(motor_index);
//                 trans_cmd.push_back(target_position & 0xFF);
//                 trans_cmd.push_back((target_position >> 8) & 0xFF);
//             }

//             // Calculate checksum and send command
//             uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
//             trans_cmd.push_back(checkSum);
//             send_serial_command(trans_cmd);
//         }

//         void send_serial_command(const std::vector<uint8_t>& command) 
//         {
//             if (!serial_port_.isOpen()) 
//             {
//                 RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
//                 return;
//             }
        
//             size_t bytes_to_write = command.size();
//             size_t bytes_written = serial_port_.write(command.data(), bytes_to_write);
//             if (bytes_written != bytes_to_write) 
//             {
//                 RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", bytes_to_write, bytes_written);
//             } else {
//                 RCLCPP_INFO(this->get_logger(), "Successfully sent command to serial port");
//                 printCommand(command.data(), bytes_to_write);
//             }
        
//             // 设置命令发送标志
//             {
//                 std::lock_guard<std::mutex> lock(command_mutex_);
//                 command_sent_ = true;
//             }
//             command_cv_.notify_one();
//         }

//         void printCommand(const uint8_t* command, size_t length) 
//         {
//             std::ostringstream oss;
//             oss << "Command: ";
//             for (size_t i = 0; i < length; ++i) 
//             {
//                 oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " "; 
//             }
//             oss << std::dec;

//             RCLCPP_INFO(rclcpp::get_logger("motor_control"), "%s", oss.str().c_str());
//         }

//         uint8_t calculateChecksum(const uint8_t* data, size_t length) {
//             uint16_t checksum = 0;  
//             for (size_t i = 0; i < length; ++i) {
//                 checksum += data[i];
//             }
//             return static_cast<uint8_t>(checksum & 0xFF); 
//         }

//         json query_motor_status() 
//         {
//             // 模拟查询中的串行通信以取得电机状态
//             if (!serial_port_.isOpen()) {
//                 RCLCPP_WARN(this->get_logger(), "Serial port is not open");
//                 return json{};
//             }

//             std::vector<json> motors;
//             for (int i = 0; i < 5; ++i) {  // 假设有5个电机
//                 json motor;
//                 motor["index"] = i;
//                 motor["currentPosition"] = i * 10;  // 假设的当前位置
//                 motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 假设目标位置
//                 motor["error"] = "No error"; 
//                 motor["mode"] = "normal"; // 模式
//                 motors.push_back(motor);
//             }

//             json motor_status;
//             motor_status["motor"] = motors;
//             return motor_status;
//         }

//         void publish_motor_status() 
//         {
//             std_msgs::msg::String msg;
//             msg.data = last_motor_status_;
//             status_publisher_->publish(msg);
//         }

//         void periodic_motor_status_query_thread() 
//         {
//             while (!stop_server_) 
//             {
//                 for (uint8_t motor_index = 2; motor_index < 4; motor_index++) 
//                 {
//                     std::vector<uint8_t> command = create_motor_status_query_command(motor_index);
//                     send_serial_command(command);
//                 }
//                 std::this_thread::sleep_for(std::chrono::milliseconds(50));

                
//             }
//         }
        
//         std::vector<uint8_t> create_motor_status_query_command(uint8_t motor_index) 
//         {
//             std::vector<uint8_t> command = {0x55, 0xAA, 0x03, motor_index, 0x04, 0x00, 0x22};
//             uint8_t checkSum = calculateChecksum(command.data() + 2, command.size() - 1);
//             command.push_back(checkSum);
//             return command;
//         }

//         void feedback_listener() 
//         {
//             std::vector<uint8_t> buffer;
//             while (!stop_server_) {
//                 std::unique_lock<std::mutex> lock(command_mutex_);
//                 command_cv_.wait_for(lock, std::chrono::seconds(1), [this] { return command_sent_ || stop_server_; });
//                 if (stop_server_) break;
        
//                 process_feedback_from_serial(buffer);
        
//                 // 重置命令发送标志
//                 command_sent_ = false;
//             }
//         }

//         void process_feedback_from_serial(std::vector<uint8_t>& buffer) 
//         {
//             if (!serial_port_.isOpen()) 
//             {
//                 RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
//                 return;
//             }

//             std::vector<uint8_t> new_data(1024);
//             size_t bytes_read = serial_port_.read(new_data.data(), new_data.size());
//             if (bytes_read > 0) 
//             {
//                 buffer.insert(buffer.end(), new_data.begin(), new_data.begin() + bytes_read);
//             }

//             // 处理缓冲区中的数据
//             while (buffer.size() >= 4) { 
//                 if (buffer[0] == 0xAA && buffer[1] == 0x55) 
//                 {
//                     uint8_t length = buffer[2];

//                     // 确保有足够的数据
//                     if (buffer.size() >= 3 + length + 1) { 
//                         std::string feedback(buffer.begin(), buffer.begin() + 3 + length + 1);
//                         buffer.erase(buffer.begin(), buffer.begin() + 3 + length + 1);
//                         feedback_queue_.push({current_motor_index_, feedback});
//                         parse_feedback(feedback);
//                     }
//                 } 
//                 else 
//                 {
//                     buffer.erase(buffer.begin());
//                 }
//             }
//         }



//         void parse_feedback(const std::string &feedback) 
//         {
//             std::ostringstream oss;
//             oss << "Parsing feedback: ";
            
//             // 遍历反馈字符串的每个字符，并以十六进制输出
//             for (unsigned char c : feedback) 
//             {
//                 oss << std::hex << std::setw(2) << std::setfill('0') 
//                     << static_cast<int>(c) << " "; // 格式化输出为十六进制
//             }

//             oss << std::dec; // 恢复为十进制输出格式

//             RCLCPP_INFO(rclcpp::get_logger("motor_control"), "%s", oss.str().c_str());
//         }



//         FeedbackQueue feedback_queue_; 
//         rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
//         rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
//         serial::Serial serial_port_;
//         std::string last_motor_status_ = "Motor status initialized";
//         std::thread http_server_thread_, feedback_thread_, request_processing_thread_, motor_status_query_thread_;
//         std::atomic<bool> stop_server_;
//         rclcpp::TimerBase::SharedPtr timer_;

//         std::mutex command_mutex_;
//         std::condition_variable command_cv_;
//         std::atomic<bool> command_sent_ = false;
//         std::atomic<int> current_motor_index_ = -1;
//         std::mutex request_mutex_;
//         std::condition_variable request_cv_;
//         std::queue<json> request_queue_;  
// };

// int main(int argc, char *argv[]) 
// {
//     rclcpp::init(argc, argv);
//     rclcpp::executors::MultiThreadedExecutor executor;
//     auto http_server_node = std::make_shared<HttpServerNode>();
//     executor.add_node(http_server_node);
//     executor.spin();
//     rclcpp::shutdown();
//     return 0;
// }



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

        // 启动线程
        http_server_thread_ = std::thread(&HttpServerNode::start_http_server, this);
        feedback_thread_ = std::thread(&HttpServerNode::feedback_listener, this);
        request_processing_thread_ = std::thread(&HttpServerNode::process_requests, this);
        motor_status_query_thread_ = std::thread(&HttpServerNode::periodic_motor_status_query_thread, this);

        if (!serial_port_.isOpen()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
        timer_ = this->create_wall_timer(std::chrono::seconds(1), 
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

    // 获取电机状态的接口
    json get_motor_status() {
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

    void handle_post(const httplib::Request &req, httplib::Response &res) {
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

    void process_request(const json& json_data) {
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
        RCLCPP_INFO(this->get_logger(), "Published MotorControlCommand: id=%s, timestamp=%s, motors_count=%zu", msg.id.c_str(), msg.timestamp.c_str(), msg.motors.size());
    }

    void send_request_motors(const json& json_data) {
        // Prepare command
        std::vector<uint8_t> trans_cmd = {0x55, 0xAA};
        const auto& motors = json_data["motor"];
        size_t motor_num = motors.size();
        trans_cmd.push_back((motor_num * 3 + 1) & 0xFF);
        trans_cmd.push_back(0xFF);
        trans_cmd.push_back(0xF2);

        for (const auto& motor : motors) {
            uint8_t motor_index = motor["index"];
            uint16_t target_position = motor["targetPosition"];
            trans_cmd.push_back(motor_index);
            trans_cmd.push_back(target_position & 0xFF);
            trans_cmd.push_back((target_position >> 8) & 0xFF);
        }

        // Calculate checksum and send command
        uint8_t checkSum = calculateChecksum(trans_cmd.data() + 2, trans_cmd.size() - 1);
        trans_cmd.push_back(checkSum);
        send_serial_command(trans_cmd);
    }

    void send_serial_command(const std::vector<uint8_t>& command) {
        if (!serial_port_.isOpen()) {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }
    
        size_t bytes_to_write = command.size();
        size_t bytes_written = serial_port_.write(command.data(), bytes_to_write);
        if (bytes_written != bytes_to_write) {
            RCLCPP_ERROR(this->get_logger(), "Failed to write all bytes to serial port. Expected %zu, wrote %zu", bytes_to_write, bytes_written);
        } else {
            RCLCPP_INFO(this->get_logger(), "Successfully sent command to serial port");
            printCommand(command.data(), bytes_to_write);
        }
    
        // 设置命令发送标志
        {
            std::lock_guard<std::mutex> lock(command_mutex_);
            command_sent_ = true;
        }
        command_cv_.notify_one();
    }

    void printCommand(const uint8_t* command, size_t length) 
    {
        std::ostringstream oss;
        oss << "Command: ";
        for (size_t i = 0; i < length; ++i) 
        {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " "; 
        }
        oss << std::dec;

        RCLCPP_INFO(rclcpp::get_logger("motor_control"), "%s", oss.str().c_str());
    }

    uint8_t calculateChecksum(const uint8_t* data, size_t length) {
        uint16_t checksum = 0;  
        for (size_t i = 0; i < length; ++i) {
            checksum += data[i];
        }
        return static_cast<uint8_t>(checksum & 0xFF); 
    }

    void publish_motor_status() {
        std_msgs::msg::String msg;
        msg.data = motor_status_.dump();
        status_publisher_->publish(msg);
    }

    void periodic_motor_status_query_thread() {
        while (!stop_server_) {
            for (uint8_t motor_index = 2; motor_index < 4; motor_index++) {
                std::vector<uint8_t> command = create_motor_status_query_command(motor_index);
                send_serial_command(command);
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 修改为500毫秒
            }
        }
    }
    
    std::vector<uint8_t> create_motor_status_query_command(uint8_t motor_index) {
        std::vector<uint8_t> command = {0x55, 0xAA, 0x03, motor_index, 0x04, 0x00, 0x22};
        uint8_t checkSum = calculateChecksum(command.data() + 2, command.size() - 1);
        command.push_back(checkSum);
        return command;
    }

    void feedback_listener() {
        std::vector<uint8_t> buffer;
        while (!stop_server_) {
            std::unique_lock<std::mutex> lock(command_mutex_);
            command_cv_.wait_for(lock, std::chrono::seconds(1), [this] { return command_sent_ || stop_server_; });
            if (stop_server_) break;
    
            process_feedback_from_serial(buffer);
    
            // 重置命令发送标志
            command_sent_ = false;
        }
    }

    void process_feedback_from_serial(std::vector<uint8_t>& buffer) {
        if (!serial_port_.isOpen()) {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }

        std::vector<uint8_t> new_data(1024);
        size_t bytes_read = serial_port_.read(new_data.data(), new_data.size());
        if (bytes_read > 0) {
            buffer.insert(buffer.end(), new_data.begin(), new_data.begin() + bytes_read);
        }

        // 处理缓冲区中的数据
        while (buffer.size() >= 4) { 
            if (buffer[0] == 0xAA && buffer[1] == 0x55) {
                uint8_t length = buffer[2];

                // 确保有足够的数据
                if (buffer.size() >= 3 + length + 1) { 
                    std::string feedback(buffer.begin(), buffer.begin() + 3 + length + 1);
                    buffer.erase(buffer.begin(), buffer.begin() + 3 + length + 1);
                    feedback_queue_.push({current_motor_index_, feedback});
                    parse_feedback(feedback);
                }
            } else {
                buffer.erase(buffer.begin());
            }
        }
    }

    void parse_feedback(const std::string &feedback) 
    {
        std::ostringstream oss;
        oss << "Parsing feedback: ";
        
        // 遍历反馈字符串的每个字符，并以十六进制输出
        for (unsigned char c : feedback) 
        {
            oss << std::hex << std::setw(2) << std::setfill('0') 
                << static_cast<int>(c) << " "; // 格式化输出为十六进制
        }

        oss << std::dec; // 恢复为十进制输出格式

        RCLCPP_INFO(rclcpp::get_logger("motor_control"), "%s", oss.str().c_str());

        // 解析反馈数据并更新电机状态
        update_motor_status(feedback);
    }

    void update_motor_status(const std::string &feedback) {
        // 假设反馈数据格式为 "aa 55 11 02 21 37 f8 fd d5 df e3 37 ea e8 03 e6 47 be 35 17 bd d0 a2 08 a3 08 2a"
        // 这里需要根据实际的反馈数据格式进行解析
        // 例如，假设反馈数据的第4个字节是电机索引，第5-6个字节是当前位置，第7-8个字节是目标位置

        if (feedback.size() < 8) 
        {
            RCLCPP_ERROR(this->get_logger(), "Invalid feedback length: %zu", feedback.size());
            return;
        }

        int motor_index = static_cast<unsigned char>(feedback[3]);
        int current_position = (static_cast<unsigned char>(feedback[10]) << 8) | static_cast<unsigned char>(feedback[9]);
        int target_position = (static_cast<unsigned char>(feedback[8]) << 8) | static_cast<unsigned char>(feedback[7]);

        std::lock_guard<std::mutex> lock(motor_status_mutex_);
        motor_status_["motor"][motor_index]["currentPosition"] = current_position;
        motor_status_["motor"][motor_index]["targetPosition"] = target_position;
        motor_status_["motor"][motor_index]["error"] = "No error";
        motor_status_["motor"][motor_index]["mode"] = "normal";

        RCLCPP_INFO(this->get_logger(), "Updated motor %d status: current=%d, target=%d", motor_index, current_position, target_position);
    }

    FeedbackQueue feedback_queue_; 
    rclcpp::Publisher<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    serial::Serial serial_port_;
    std::string last_motor_status_ = "Motor status initialized";
    std::thread http_server_thread_, feedback_thread_, request_processing_thread_, motor_status_query_thread_;
    std::atomic<bool> stop_server_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::mutex command_mutex_;
    std::condition_variable command_cv_;
    std::atomic<bool> command_sent_ = false;
    std::atomic<int> current_motor_index_ = -1;
    std::mutex request_mutex_;
    std::condition_variable request_cv_;
    std::queue<json> request_queue_;

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
