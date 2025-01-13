#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp> // 使用字符串作为命令

#include "serial/serial.h" 
#include "motor_control_command_msgs/msg/motor_control_command.hpp" 
#include "motor_control_command_msgs/msg/motor.hpp" 

class MotorControlNode : public rclcpp::Node 
{
    public:
        MotorControlNode()
            : Node("motor_control_node"), serial_port("/dev/ttyV20", 115200, serial::Timeout::simpleTimeout(1000)) 
        {
            // 创建订阅者
            subscription_ = this->create_subscription<motor_control_command_msgs::msg::MotorControlCommand>(
                "motor_command", 10, std::bind(&MotorControlNode::on_message, this, std::placeholders::_1));

            if (!serial_port.isOpen()) 
            {
                RCLCPP_ERROR(this->get_logger(), "Failed to open serial port!");
                rclcpp::shutdown();
                return;
            }

            RCLCPP_INFO(this->get_logger(), "Serial port opened successfully.");
        }
    private:
        void on_message(const motor_control_command_msgs::msg::MotorControlCommand::SharedPtr msg) 
        {
            RCLCPP_INFO(this->get_logger(), "Received MotorCommand: id=%s, timestamp=%s", 
                                                msg->id.c_str(), msg->timestamp.c_str());
            
            for(const auto &motor : msg->motors) 
            {
                RCLCPP_INFO(this->get_logger(), " Motor index: %d, Target Position: %d", 
                                                    motor.index, motor.target_position);
                std::int32_t motor_index = motor.index;
                std::string motor_command = transform_command(motor_index,motor.target_position); //将target_position转化成指令
                send_command_to_motor(motor_index, motor_command);
            }
        }

        std::string transform_command(std::int32_t motor_index,std::int32_t target_position)
        {
            std::string command = "0X1111111" ;
            return command;    //包括 电缸的id target_position
        }

        void send_command_to_motor(std::int32_t index ,std::string command) 
        {
            // 发送指令到电机
            std::string cmd_to_send = command + "\n"; // 以换行符结束命令
            serial_port.write(cmd_to_send); // 通过串口发送命令
            RCLCPP_INFO(this->get_logger(), "Sent command to motor: '%s'", cmd_to_send.c_str());
        }



        

        rclcpp::Subscription<motor_control_command_msgs::msg::MotorControlCommand>::SharedPtr subscription_;
        serial::Serial serial_port;  // 串口对象
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorControlNode>());
    rclcpp::shutdown();
    return 0;
}