#include "ActionController.h"
#include "MotorHandler.h"
#include "CleanerHandler.h"
#include "RVCController.h"
#include "Types.h"
#include <iostream>

ActionController::ActionController(MotorHandler* mh, CleanerHandler* ch, RVCController* rvc)
    : motorHandler(mh), cleanerHandler(ch), rvcController(rvc){}

void ActionController::obstacleStatus(SensorData loc) {

    //  ====================================== 초기 및 측면 회피 로직 ======================================
    // 1-1(번호 수정). 정상 청소 중 전방 장애물 최초 감지 
    if (!isEvading && loc.frontSensorData && !isMovingBackward){
        std::cout << "[ActionController] 전방 장애물 감지! 회피 시작...\n";
        
        isEvading = true; // 회피 시작.
        checkingRight = true; // 추가된 로직, 오른쪽 장애물 확인을 위함.

        location.isLeftBlocked = loc.leftSensorData;
        location.isFrontBlocked = loc.frontSensorData;

        rvcController->changeState(SystemState::AVOIDING); // 메인 시스템에 보고

        motorHandler->stopMotor();
        cleanerHandler->deactivateCleaner();

        std::cout << "[ActionController] 오른쪽 장애물 유무를 위한 turn right...\n"; // 추가된 cout
        motorHandler->stopMotor();
        motorHandler->turnRight(); // 오른쪽 확인 들어감

        return;
    }

    // 1-2. 정상 청소 중 전방 장애물 최초 감지 후 오른쪽 확인 + 회피 로직을 위한 추가 (추가된 조건문)
    if (isEvading && checkingRight && !isMovingBackward) {
        checkingRight = false;

        location.isFrontBlocked=loc.leftSensorData;
        location.isRightBlocked=loc.frontSensorData;

        std::cout << "[ActionController] 오른쪽 장애물 유무를 확인 후 방향 복구 turn left...\n"; // 추가된 cout
        motorHandler->stopMotor();
        motorHandler->turnLeft();     // 방향 복구
        
        // (조건문 위치 수정) 1-3. 회피 기동 중일 때 (UC-01 회피 로직)

        if (!location.isLeftBlocked) {
            std::cout << "[ActionController] 왼쪽으로 회피...\n";
            motorHandler->turnLeft();
            
            isEvading = false; // 회피 끝
            rvcController->resumeCleaning(); // 메인에 청소 재개 명령
        }
        else if (!location.isRightBlocked) {
            std::cout << "[ActionController] 오른쪽으로 회피...\n";
            motorHandler->turnRight();
            
            isEvading = false; // 회피 끝
            rvcController->resumeCleaning(); 
        } 
        else {
            // 양쪽이 모두 막혀있다면 (UC-02 이동)
            std::cout << "[ActionController] 양쪽 모두가 막힘. 후진 중...\n";
            isMovingBackward = true;
            motorHandler->moveBackward(); 
            return;
        }
    }

    //  ====================================== 후진 로직 ======================================
    // 2-1 후진 중 센서 수신
    if (isEvading && isMovingBackward && !checkingRight){
        checkingRight = true; // 추가된 로직, 오른쪽 장애물 확인을 위함.

        location.isLeftBlocked = loc.leftSensorData;
        location.isFrontBlocked = loc.frontSensorData;

        std::cout << "[ActionController] 오른쪽 장애물 유무를 위한 turn right...\n"; // 추가된 cout
        motorHandler->stopMotor();
        motorHandler->turnRight();
        return;
    }

    // 2-2 후진 중 오른쪽 확인 후 회피 로직 수행
    if (isEvading && checkingRight && isMovingBackward){
        checkingRight = false;

        location.isFrontBlocked=loc.leftSensorData;
        location.isRightBlocked=loc.frontSensorData;

        std::cout << "[ActionController] 오른쪽 장애물 유무를 확인 후 방향 복구 turn left...\n"; // 추가된 cout
        motorHandler->stopMotor();
        motorHandler->turnLeft();     // 방향 복구

        if (!location.isLeftBlocked) {
            std::cout << "[ActionController] 왼쪽으로 회피...\n";
            motorHandler->turnLeft();
            
            isEvading = false; // 회피 끝
            isMovingBackward = false; // 후진 끝
            rvcController->resumeCleaning(); // 메인에 청소 재개 명령
            }
        else if (!location.isRightBlocked) {
            std::cout << "[ActionController] 오른쪽으로 회피...\n";
            motorHandler->turnRight();
            
            isEvading = false; // 회피 끝
            isMovingBackward = false; // 후진 끝
            rvcController->resumeCleaning(); 
        } 
        else {
            // 양쪽이 모두 막혀있다면 (UC-02 이동)
            std::cout << "[ActionController] 양쪽 모두가 막힘. 후진 중...\n";
            isMovingBackward=true;
            motorHandler->moveBackward(); 
            return;
        }
    }
}
