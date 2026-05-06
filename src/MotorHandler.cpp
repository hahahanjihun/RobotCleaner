#include "MotorHandler.h"
#include <iostream>

// 초기 상태: 정지
MotorHandler::MotorHandler() = default;

void MotorHandler::moveForward() {
    if (status == DriveSetting::FORWARD) {
        std::cout << "[Motor] 이미 전진 중입니다\n";
        return;
    }

    status = DriveSetting::FORWARD;
    std::cout << "[Motor] 전진\n";
}

void MotorHandler::moveBackward() {
    if (status == DriveSetting::BACKWARD) {
        std::cout << "[Motor] 이미 후진 중입니다\n";
        return;
    }

    status = DriveSetting::BACKWARD;
    std::cout << "[Motor] 후진\n";
}

void MotorHandler::turnLeft() {
    if (status == DriveSetting::LEFT) {
        std::cout << "[Motor] 이미 좌회전 중입니다\n";
        return;
    }

    if (status != DriveSetting::STOPPED) {
        std::cout << "[Motor] 회전은 정지 상태에서만 가능합니다\n";
        return;
    }

    status = DriveSetting::LEFT;
    std::cout << "[Motor] 좌회전\n";
}

void MotorHandler::turnRight() {
    if (status == DriveSetting::RIGHT) {
        std::cout << "[Motor] 이미 우회전 중입니다\n";
        return;
    }

    if (status != DriveSetting::STOPPED) {
        std::cout << "[Motor] 회전은 정지 상태에서만 가능합니다\n";
        return;
    }

    status = DriveSetting::RIGHT;
    std::cout << "[Motor] 우회전\n";
}

void MotorHandler::stopMotor() {
    if (status == DriveSetting::STOPPED) {
        std::cout << "[Motor] 이미 정지 상태 입니다\n";
        return;
    }
    status = DriveSetting::STOPPED;
    std::cout << "[Motor] 정지\n";
}

DriveSetting MotorHandler::getStatus() const{
    /*
    unit test 전용 함수이며 실제 구현코드에는 사용하지 않습니다.
    */
    return status;
}