#pragma once
#include "Types.h"

class ActionController; 

class ObstacleSensorHandler {
private:
    Position location; // 현재 감지된 장애물 상태
    ActionController* actionController; // 데이터를 전달할 두뇌 포인터

public:
    ObstacleSensorHandler(ActionController* ac);
    void notifyFrontObstacle(Position loc); // 전방 장애물 발생 알림
    void notifySideObstacle(Position loc);  // 측면 장애물 발생 알림
};