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
    // NEG-20: OFF 상태에서 전력 조절 차단
    if (powerLevel == PowerSetting::OFF) {
        std::cout << "[Cleaner] 비활성 상태에서 전력 조절 불가\n";
        return;
    }

    powerLevel = PowerSetting::BOOST;
    std::cout << "[Cleaner] 강력 흡입 모드\n";
}

void CleanerHandler::normalizePower() {
    // NEG-19: OFF 상태에서 전력 조절 차단
    if (powerLevel == PowerSetting::OFF) {
        std::cout << "[Cleaner] 비활성 상태에서 전력 조절 불가\n";
        return;
    }

    powerLevel = PowerSetting::NORMAL;
    std::cout << "[Cleaner] 일반 모드 복구\n";
}

PowerSetting CleanerHandler::getPowerLevel() const {
    return powerLevel; // gtest용
}