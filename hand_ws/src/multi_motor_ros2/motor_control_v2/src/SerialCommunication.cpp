#include "motor_control_v2/SerialCommunication.hpp"


namespace Hand {

namespace serialCommunication {


class SerialCommunication::Implementation 
{
    public:
        Implementation(const std::string& port, int baudrate);

        void write(const std::string& command); 
        std::string readLine(int timeout); 
        void setBaudRate(int baudrate);

    private:
        serial::Serial serial_port_;
};
 

SerialCommunication::Implementation::Implementation(const std::string& port, int baudrate)
    : serial_port_(port, baudrate, serial::Timeout::simpleTimeout(1000)) 
{
    if (!serial_port_.isOpen()) 
    {
        throw std::runtime_error("Failed to open serial port!");
    }
    RCLCPP_INFO(rclcpp::get_logger("SerialCommunication"), "Serial port opened successfully.");
}


void SerialCommunication::Implementation::write(const std::string& command) 
{
    serial_port_.write(command + "\n");
    RCLCPP_INFO(rclcpp::get_logger("SerialCommunication"), "Sent command to motor: '%s'", command.c_str());
}

std::string SerialCommunication::Implementation::readLine(int timeout)
{
    return serial_port_.readline(timeout);
} 

void SerialCommunication::Implementation::setBaudRate(int baudrate)
{
    serial_port_.setBaudrate(baudrate);
    RCLCPP_INFO(rclcpp::get_logger("SerialCommunicationModule"), "Baud rate set to: %d", baudrate);
}


SerialCommunication::SerialCommunication(const std::string& port, int baudrate)
    : _impl(std::make_unique<Implementation>(port, baudrate)) 
{
        //doing nothing
}

void SerialCommunication::write(const std::string& command) 
{
    _impl->write(command);
}

std::string SerialCommunication::readLine(int timeout) 
{
    return _impl->readLine(timeout);
}

void SerialCommunication::setBaudRate(int baudrate)
{
    _impl ->setBaudRate(baudrate);
}

SerialCommunication::~SerialCommunication() = default; // 默认实现的析构函数


}

}