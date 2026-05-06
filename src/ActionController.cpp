#include "ActionController.h"
#include "MotorHandler.h"
#include "CleanerHandler.h"
#include "RVCController.h"
#include <iostream>

ActionController::ActionController(MotorHandler* mh, CleanerHandler* ch, RVCController* rvc)
    : motorHandler(mh), cleanerHandler(ch), rvcController(rvc) {}

void ActionController::obstacleStatus(Position loc) {

    // NEG-06: Position의 값이 비정상인 경우에 Motor를 정지시키기 위함.
    if (!loc.isFrontBlocked && !loc.isLeftBlocked && !loc.isRightBlocked) {
        std::cout << "[ActionController] 비정상 Position 감지.\n";
        motorHandler->stopMotor();
        return;
    }
    
    // 1. 정상 청소 중 전방 장애물 최초 감지 
    if (!isEvading && loc.isFrontBlocked) {
        std::cout << "[ActionController] 전방 장애물 감지! 회피 시작...\n";
        
        isEvading = true; // 회피 시작
        rvcController->changeState(SystemState::AVOIDING); // 메인 시스템에 보고

        motorHandler->stopMotor();
        cleanerHandler->deactivateCleaner();
    }

    // 2. 회피 기동 중일 때 (UC-01, UC-02 루프 로직)
    if (isEvading) {
        if (!loc.isLeftBlocked) {
            std::cout << "[ActionController] 왼쪽으로 회피...\n";
            motorHandler->turnLeft();
            
            isEvading = false; // 회피 끝
            rvcController->resumeCleaning(); // 메인에 청소 재개 명령
        } 
        else if (!loc.isRightBlocked) {
            std::cout << "[ActionController] 오른쪽으로 회피...\n";
            motorHandler->turnRight();
            
            isEvading = false; // 회피 끝
            rvcController->resumeCleaning(); 
        } 
        else {
            // 양쪽이 모두 막혀있다면 (UC-02 Loop)
            std::cout << "[ActionController] 양쪽 모두가 막힘. 후진 중...\n";
            motorHandler->moveBackward(); 
        }
    }
}