#include "DustController.h"
#include "CleanerHandler.h"
#include "RVCController.h"
#include <iostream>
#include <thread>
#include <chrono>

DustController::DustController(CleanerHandler* ch, RVCController* rvc)
    : cleanerHandler(ch), rvcController(rvc) {}

void DustController::dustStatus(float dustLevel) {
    // NEG-13
    if (dustLevel < 0.0f) {
        std::cout << "[DustController] 유효하지 않은 먼지량 감지\n";
        return; 
    }

    // NEG-14
    if (dustLevel > 100.0f) {
        std::cout << "[DustController] 감지된 먼지량이 너무 큽니다. 최대값으로 처리\n";
        dustLevel = 100.0f; 
    }

    if (dustLevel > 50.0f) { // 먼지량이 임계치를 초과하면 (UC-03)
        rvcController->changeState(SystemState::BOOSTING); // 메인 시스템에 집중 청소 상태 보고
        std::cout << "[DustController] 먼지 감지! 집중 청소 시작\n";
        
        cleanerHandler->boostPower(); // 흡입력 부스트
        
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 5초간 집중 청소 딜레이
        
        cleanerHandler->normalizePower(); // 흡입력 정상화
        rvcController->resumeNormalCleaning(); // 집중 청소 종료 및 복귀 보고
    }
}