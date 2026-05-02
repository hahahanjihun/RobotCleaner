#include "MotorHandler.h"
#include <iostream>

// 초기 상태: 정지
MotorHandler::MotorHandler() = default;

void MotorHandler::moveForward() {
    status = DriveSetting::FORWARD;
    std::cout << "[Motor] 전진\n";
}

void MotorHandler::moveBackward() {
    status = DriveSetting::BACKWARD;
    std::cout << "[Motor] 후진\n";
}

void MotorHandler::turnLeft() {
    status = DriveSetting::LEFT;
    std::cout << "[Motor] 좌회전\n";
}

void MotorHandler::turnRight() {
    status = DriveSetting::RIGHT;
    std::cout << "[Motor] 우회전\n" ;
}

void MotorHandler::stopMotor() {
    status = DriveSetting::STOPPED;
    std::cout << "[Motor] 정지\n";
}