#include "motor_control_v2/HttpServer.hpp"

namespace Hand {


namespace httpServer {

HttpServer::HttpServer(int port) 
{
    server_.Post("/motor/control", [this](const httplib::Request& req, httplib::Response& res) {
        handleMotorControl(req, res);
    });

    server_.Post("/motor/stop", [this](const httplib::Request& req, httplib::Response& res) {
        handleMotorStop(req, res);
    });

    server_.Get("/motor/status", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetMotorStatus(req, res);
    });
}

void HttpServer::start(int port) 
{
    server_.listen("0.0.0.0", port);
}

void HttpServer::stop() 
{
    server_.stop();
}

void HttpServer::setMotorControlCallback(std::function<void(int, int)> control_callback) 
{
    motor_control_callback_ = control_callback;
}

void HttpServer::setMotorStopCallback(std::function<void(int)> stop_callback) 
{
    motor_stop_callback_ = stop_callback;
}

void HttpServer::setMotorStatusCallback(std::function<std::string(int)> status_callback) 
{
    motor_status_callback_ = status_callback;
}

void HttpServer::handleMotorControl(const httplib::Request& req, httplib::Response& res) 
{
    try 
    {
        auto json_data = nlohmann::json::parse(req.body);
        int motor_index = json_data["motor_index"];
        int target_position = json_data["target_position"];
        
        if (motor_control_callback_) 
        {
            motor_control_callback_(motor_index, target_position);
        }
        
        res.set_content("{\"status\":\"motor command received\"}", "application/json");
    } 
    catch (const std::exception &ex) 
    {
        res.set_content("{\"status\":\"error\",\"message\":\"Failed to parse JSON\"}", "application/json");
    }
}

void HttpServer::handleMotorStop(const httplib::Request& req, httplib::Response& res) 
{
    try 
    {
        auto json_data = nlohmann::json::parse(req.body);
        int motor_index = json_data["motor_index"];
        
        if (motor_stop_callback_) 
        {
            motor_stop_callback_(motor_index);
        }
        
        res.set_content("{\"status\":\"motor stopped\"}", "application/json");
    } 
    catch (const std::exception &ex) 
    {
        res.set_content("{\"status\":\"error\",\"message\":\"Failed to parse JSON\"}", "application/json");
    }
}

void HttpServer::handleGetMotorStatus(const httplib::Request& req, httplib::Response& res) 
{
    try 
    {
        int motor_index = std::stoi(req.get_param_value("motor_index"));
        
        if (motor_status_callback_) 
        {
            std::string status = motor_status_callback_(motor_index);
            res.set_content(status, "application/json");
        } 
        else 
        {
            res.set_content("{\"status\":\"error\",\"message\":\"Motor status callback not set\"}", "application/json");
        }
    } 
    catch (const std::exception &ex) 
    {
        res.set_content("{\"status\":\"error\",\"message\":\"Invalid motor index\"}", "application/json");
    }
}

}

}