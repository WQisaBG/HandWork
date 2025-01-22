#ifndef SERIAL_COMMUNICATION_MODULE_HPP
#define SERIAL_COMMUNICATION_MODULE_HPP

#include <rclcpp/rclcpp.hpp>
#include <stdexcept>
#include <memory>

#include "serial/serial.h"


namespace Hand {

namespace serialCommunication {


class SerialCommunication 
{
    public:
        SerialCommunication(const std::string& port, int baudrate);
        void write(const std::string& command);
        std::string readLine(int timeout);
        void setBaudRate(int baudrate); // 设置波特率的方法
        
        ~SerialCommunication(); // 定义析构函数

        class Implementation; // 前向声明
    private:
        std::unique_ptr<Implementation> _impl; // 使用unique_ptr管理实现
};

}
}

#endif // SERIAL_COMMUNICATION_MODULE_HPP


