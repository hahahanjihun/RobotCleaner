#include "RVCController.h"
#include "CleanerHandler.h"
#include "MotorHandler.h"
#include <iostream>

RVCController::RVCController(CleanerHandler* ch, MotorHandler* mh) 
    : cleanerHandler(ch), motorHandler(mh) {}

void RVCController::changeState(SystemState newState) {
    systemState = newState; // 전달받은 상태로 시스템 상태 업데이트
    std::cout << "[System] State 변경\n";
}

SystemState RVCController::getSystemState() const {
    return systemState; // 현재 시스템 상태 반환 (테스트용)
}

void RVCController::startCleaning() {
    changeState(SystemState::CLEANING); // 상태를 CLEANING으로 전환
    std::cout << "--- System: 자동 청소 시작 ---\n";
    cleanerHandler->activateCleaner(); // 흡입 시작
    motorHandler->moveForward();       // 전진 시작
}

void RVCController::resumeCleaning() {
    changeState(SystemState::CLEANING); // 회피가 끝났으므로 다시 CLEANING으로 복귀
    std::cout << "--- System: 자동 청소 복귀 ---\n";
    motorHandler->moveForward(); // 다시 전진
}

void RVCController::resumeNormalCleaning() {
    changeState(SystemState::CLEANING); // 집중 청소가 끝났으므로 복귀
    std::cout << "--- System: 자동 청소 복귀 (집중 청소 종료) ---\n";
}