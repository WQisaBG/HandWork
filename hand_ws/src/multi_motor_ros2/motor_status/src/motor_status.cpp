#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <nlohmann/json.hpp>
#include "serial/serial.h"
#include "httplib.h"
#include <thread>

using json = nlohmann::json;

class MotorStatusNode : public rclcpp::Node {
public:
    MotorStatusNode() : Node("motor_status_node") {
        status_publisher_ = this->create_publisher<std_msgs::msg::String>("motor_status", 10);

        // 设置串口参数
        serial_port_.setPort("/dev/ttyV20");           // 替换为您的串口设备
        serial_port_.setBaudrate(115200);                 // 设置波特率
        serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
        serial_port_.setTimeout(timeout);

        try {
            serial_port_.open();
        } catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "Unable to open serial port: %s", e.what());
            return;
        }

        // 启动周期性定时器
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500), std::bind(&MotorStatusNode::publish_motor_status, this));

        // 启动 HTTP 服务器
        start_http_server();
    }

    ~MotorStatusNode() {
        if (serial_port_.isOpen()) {
            serial_port_.close();
        }
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_publisher_;
    serial::Serial serial_port_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::string last_motor_status_;

void publish_motor_status() {
    if (serial_port_.isOpen()) {
        std::string result = serial_port_.readline(100); // 可以增加更大的超时，以确保读取数据
        RCLCPP_INFO(this->get_logger(), "Read from serial: '%s'", result.c_str()); // 添加调试语句
        if (!result.empty()) {
            json response = parse_motor_status(result);
            last_motor_status_ = response.dump(); // 保存最新状态
            std_msgs::msg::String msg;
            msg.data = last_motor_status_;
            status_publisher_->publish(msg);
        } else {
            RCLCPP_WARN(this->get_logger(), "No data received from serial");
        }
    } else {
        RCLCPP_WARN(this->get_logger(), "Serial port is not open");
    }
}

    json parse_motor_status(const std::string &data) {
        json response;
        response["id"] = "1"; // 示例，您可以实现任务 ID 的累加逻辑
        response["timestamp"] = get_current_time();

        std::vector<json> motors;

        // 假定串口返回的格式为 "index,currentPosition,targetPosition,error,mode"
        std::istringstream ss(data);
        std::string item;
        unsigned int index = 0;

        while (std::getline(ss, item, ',')) {
            json motor;
            motor["index"] = index++;
            motor["currentPosition"] = std::stoi(item); // 假定数据可以转换为整数
            motor["targetPosition"] = motor["currentPosition"].get<int>() + 10; // 示例逻辑
            motor["error"] = "No error"; // 假定无错误
            motor["mode"] = "normal"; // 假定普通模式
            motors.push_back(motor);
        }

        response["motor"] = motors;
        return response;
    }

    std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }

    void start_http_server() {
        std::thread([this]() {
            httplib::Server svr;

            // 定义 HTTP GET 请求路由
            svr.Get("/motor/state", [this](const httplib::Request &req, httplib::Response &res) {
                res.set_content(last_motor_status_, "application/json");
            });

            // 启动 HTTP 服务器并在 5000 端口上监听
            svr.listen("127.0.0.1", 10088);
        }).detach();  // 启动服务器的线程分离
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorStatusNode>());
    rclcpp::shutdown();
    return 0;
}