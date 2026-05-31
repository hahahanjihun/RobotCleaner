#pragma once
#include "Types.h"

class ActionController; 

class ObstacleSensorHandler {
private:
    // 삭제
    // Position location;

    // 추가, 좌측 및 전방 (실제론 좌측 및 전방 + 전방 및 우측 탐지까지 진행할 수 있음)
    SensorData location; // 현재 감지된 장애물 상태

    ActionController* actionController; // 데이터를 전달할 두뇌 포인터

public:
    explicit ObstacleSensorHandler(ActionController* ac);
    // 삭제 메서드
    // void notifyFrontObstacle(Position loc); // 전방 장애물 발생 알림
    // void notifySideObstacle(Position loc);  // 측면 장애물 발생 알림

    // 추가 메서드
    void notifyObstacle(SensorData loc);  // 전방 및 좌측 장애물 발생 알림
};