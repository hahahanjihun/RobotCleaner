#pragma once
#include "Types.h"

// 구동 모터 제어 핸들러
class MotorHandler {
private:
    DriveSetting status = DriveSetting::STOPPED; // 현재 모터 상태

public:
    MotorHandler();
    
    // 모터 동작 명령
    void moveForward();  // 전진
    void moveBackward(); // 후진
    void turnLeft();     // 좌회전
    void turnRight();    // 우회전
    void stopMotor();    // 정지

     // GTest에서 상태 검증을 위한 Getter
    DriveSetting getStatus() const;    
};