#include "CleanerHandler.h"
#include <iostream>

// 초기 상태: 꺼짐
CleanerHandler::CleanerHandler() = default;

void CleanerHandler::activateCleaner() {
    powerLevel = PowerSetting::NORMAL;
    std::cout << "[Cleaner] 청소기 흡입 ON\n";
}
void CleanerHandler::deactivateCleaner() {
    powerLevel = PowerSetting::OFF;
    std::cout << "[Cleaner] 청소기 흡입 OFF\n";
}

void CleanerHandler::boostPower() {
    powerLevel = PowerSetting::BOOST;
    std::cout << "[Cleaner] 강력 흡입 모드\n";
}

void CleanerHandler::normalizePower() {
    powerLevel = PowerSetting::NORMAL;
    std::cout << "[Cleaner] 일반 모드 복구\n";
}