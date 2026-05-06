#include "RVCController.h"
#include "CleanerHandler.h"
#include "MotorHandler.h"
#include <iostream>

RVCController::RVCController(CleanerHandler* ch, MotorHandler* mh) 
    : cleanerHandler(ch), motorHandler(mh) {}

void RVCController::changeState(SystemState newState) {
    //NEG-03 / NEG-04 유효성 검사 추가
    if (newState < SystemState::IDLE || newState > SystemState::BOOSTING) {
        std::cout << "[System] 유효하지 않은 상태값 요청 거부\n";
        return;
    }

    // NEG-05: 현재와 동일한 상태로 변경 요청 차단
    if (newState == systemState) {
        std::cout << "[System] 이미 동일한 상태\n";
        return;
    }

    systemState = newState; // 전달받은 상태로 시스템 상태 업데이트
    std::cout << "[System] State 변경\n";
}

SystemState RVCController::getSystemState() const {
    /*
    unit test 전용 함수이며 실제 구현코드에는 사용하지 않습니다.
    */
    return systemState;
}

void RVCController::startCleaning() {
    // NEG-04: AVOIDING 중 재시작 차단 / NEG-01: CLEANING 중복 호출 차단
    if (systemState == SystemState::AVOIDING) {
        std::cout << "[System] 현재 상태에서 청소 시작 불가\n";
        return;
    }

    if (systemState == SystemState::CLEANING) {
        std::cout << "[System] 현재 이미 청소 중\n";
        return;
    }

    changeState(SystemState::CLEANING); // 상태를 CLEANING으로 전환
    std::cout << "--- System: 자동 청소 시작 ---\n";
    cleanerHandler->activateCleaner(); // 흡입 시작
    motorHandler->moveForward();       // 전진 시작
}

void RVCController::resumeCleaning() {
    if (systemState == SystemState::CLEANING) {
        std::cout << "[System] 현재 이미 청소 중\n";
        return;
    }

    changeState(SystemState::CLEANING); // 회피가 끝났으므로 다시 CLEANING으로 복귀
    std::cout << "--- System: 자동 청소 복귀 ---\n";
    motorHandler->moveForward(); // 다시 전진
    cleanerHandler->activateCleaner(); // 다시 흡입 시작
}

void RVCController::resumeNormalCleaning() {
    changeState(SystemState::CLEANING); // 집중 청소가 끝났으므로 복귀
    std::cout << "--- System: 자동 청소 복귀 (집중 청소 종료) ---\n";
}