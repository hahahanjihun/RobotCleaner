#include "ObstacleSensorHandler.h"
#include "ActionController.h"

ObstacleSensorHandler::ObstacleSensorHandler(ActionController* ac) 
    : actionController(ac) {}

// 삭제 메서드
// void ObstacleSensorHandler::notifyFrontObstacle(Position loc) {
//     location = loc; // 센서 데이터 업데이트
//     actionController->obstacleStatus(location); // 판단을 위해 컨트롤러로 전달
// }

// 삭제 메서드
// void ObstacleSensorHandler::notifySideObstacle(Position loc) {
//     location = loc; // 센서 데이터 업데이트
//     actionController->obstacleStatus(location); // 판단을 위해 컨트롤러로 전달
// }


// 추가된 메서드
void ObstacleSensorHandler::notifyObstacle(SensorData loc) {
    location = loc;
    actionController->obstacleStatus(location);
}