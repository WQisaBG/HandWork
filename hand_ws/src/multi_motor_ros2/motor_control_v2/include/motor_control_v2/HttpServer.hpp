#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "httplib.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <functional>

namespace Hand {

namespace httpServer {

class HttpServer 
{
    public:
        HttpServer(int port);
        void start(int port);
        void stop();

        void setMotorControlCallback(std::function<void(int, int)> control_callback);
        void setMotorStopCallback(std::function<void(int)> stop_callback);
        void setMotorStatusCallback(std::function<std::string(int)> status_callback); // 设置电机状态回调

    private:
        void handleMotorControl(const httplib::Request& req, httplib::Response& res);
        void handleMotorStop(const httplib::Request& req, httplib::Response& res);
        void handleGetMotorStatus(const httplib::Request& req, httplib::Response& res); // 处理 GET 请求

        httplib::Server server_;
        std::function<void(int, int)> motor_control_callback_;
        std::function<void(int)> motor_stop_callback_;
        std::function<std::string(int)> motor_status_callback_; 
};

}  // HttpServer

}   //MotorControl

#endif // HTTP_SERVER_HPP