#ifndef MOTOR_CONTROL_MODULE_HPP
#define MOTOR_CONTROL_MODULE_HPP

#include "motor_control_v2/SerialCommunication.hpp"
#include <string>
#include <memory>

namespace Hand {

namespace motorControl {

class MotorControl 
{
    public:
        MotorControl(serialCommunication::SerialCommunication& comm_module);
        void executeMotorCommand(int motor_index, int target_position);
        void updateMotorConfig(int motor_index, const std::string& config);
        void stopMotor(int motor_index);
        std::string queryMotorStatus(int motor_index);  

        class Implementation; 
    private:
        std::unique_ptr<Implementation> _impl; 

};


class MotorControl::Implementation 
{
    public:
        Implementation(serialCommunication::SerialCommunication& comm_module);
        void executeMotorCommand(int motor_index, int target_position);
        void updateMotorConfig(int motor_index, const std::string& config);
        void stopMotor(int motor_index);
        std::string queryMotorStatus(int motor_index);  // 查询电机状态
        std::string transform_command(int motor_index, int target_position);
        std::string decimalToHex(int decimalNumber);
        std::string formatHexToPairs(const std::string& hexStr);
        std::string reverseFormattedHexPairs(const std::string& formattedHex);

    private:
        serialCommunication::SerialCommunication& comm_module_;

};

}

}

#endif // SERIAL_COMMUNICATION_MODULE_HPP
