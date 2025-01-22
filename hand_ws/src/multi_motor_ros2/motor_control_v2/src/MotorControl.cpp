#include "motor_control_v2/MotorControl.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

namespace Hand {

namespace motorControl {


//===============================================================================================================

MotorControl::Implementation::Implementation(serialCommunication::SerialCommunication& comm_module)
    : comm_module_(comm_module) 
{

    // doing noting

}
//===============================================================================================================

void MotorControl::Implementation::executeMotorCommand(int motor_index, int target_position) 
{
    std::string command = transform_command(motor_index, target_position);
    comm_module_.write(command);
}
//===============================================================================================================

void MotorControl::Implementation::updateMotorConfig(int motor_index, const std::string& config) 
{
        std::string command = "CONFIGURE " + std::to_string(motor_index) + " " + config; // 示例配置命令
        comm_module_.write(command);
}
//===============================================================================================================
void MotorControl::Implementation::stopMotor(int motor_index)
{
    std::string command = "this is " +std::to_string(motor_index);
    comm_module_.write(command);

}
//===============================================================================================================

std::string  MotorControl::Implementation::queryMotorStatus(int motor_index)
{
    return comm_module_.readLine(motor_index);
}

std::string MotorControl::Implementation::transform_command(int motor_index, int target_position) 
{
    std::string frame_data = "55 AA 04";
    std::string motor_id = decimalToHex(motor_index);
    std::string control_mode = "21";
    std::string target_pose = decimalToHex(target_position);
    std::string target_pose_command = reverseFormattedHexPairs(target_pose);
    std::string check_data = "76";
    return frame_data + " " + motor_id + " " + control_mode + " " + target_pose_command + " " + check_data; 
}
//===============================================================================================================

std::string MotorControl::Implementation::decimalToHex(int decimalNumber)
{
    std::string hexStr; 
    while (decimalNumber > 0) 
    {
        int remainder = decimalNumber % 16; 
        hexStr = (remainder < 10 ? char(remainder + '0') : char(remainder - 10 + 'A')) + hexStr;
            decimalNumber /= 16; 
    }
    return formatHexToPairs(hexStr); 
}
//===============================================================================================================

std::string MotorControl::Implementation::formatHexToPairs(const std::string& hexStr) 
{
    std::ostringstream formattedHex;
    for (size_t i = 0; i < hexStr.length(); i += 2) 
    {
        if (i > 0) 
        {
            formattedHex << " ";
        }
            formattedHex << hexStr.substr(i, 2);
    }
    return formattedHex.str();
}

std::string MotorControl::Implementation::reverseFormattedHexPairs(const std::string& formattedHex) 
{
    std::istringstream iss(formattedHex);
    std::vector<std::string> pairs;
    std::string pair;
    while (iss >> pair)
    {
        pairs.push_back(pair);
    }
    std::reverse(pairs.begin(), pairs.end());
    std::ostringstream reversedHex;
    for (size_t i = 0; i < pairs.size(); ++i) 
    {
        if (i > 0) {
            reversedHex << " ";
        }
            reversedHex << pairs[i];
    }
    return reversedHex.str();
}

//===============================================================================================================
MotorControl::MotorControl(serialCommunication::SerialCommunication& comm_module) 
    : _impl(std::make_unique<Implementation>(comm_module)) 
{
    //doing nothing 

}
//===============================================================================================================

void MotorControl::executeMotorCommand(int motor_index, int target_position) 
{
    _impl->executeMotorCommand(motor_index, target_position);
}
//===============================================================================================================

void MotorControl::updateMotorConfig(int motor_index, const std::string& config) 
{
    _impl->updateMotorConfig(motor_index, config);
}

//===============================================================================================================
void MotorControl::stopMotor(int motor_index)
{
    _impl ->stopMotor(motor_index);
}

//===============================================================================================================
std::string MotorControl::queryMotorStatus(int motor_index)
{
    return _impl ->queryMotorStatus(motor_index);
}


}

}