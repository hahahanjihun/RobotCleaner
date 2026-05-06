#include "CleanerHandler.h"
#include <iostream>

// 초기 상태: 꺼짐
CleanerHandler::CleanerHandler() = default;

void CleanerHandler::activateCleaner() {
    if (powerLevel == PowerSetting::NORMAL) {
        std::cout << "[Cleaner] 이미 NORMAL 상태입니다\n";
        return;  // NEG-16
    }
    powerLevel = PowerSetting::NORMAL;
    std::cout << "[Cleaner] 청소기 흡입 ON\n";
}
void CleanerHandler::deactivateCleaner() {
    if (powerLevel == PowerSetting::OFF) {
        std::cout << "[Cleaner] 이미 OFF 상태입니다\n";
        return;  // NEG-15
    }
    powerLevel = PowerSetting::OFF;
    std::cout << "[Cleaner] 청소기 흡입 OFF\n";
}

void CleanerHandler::boostPower() {
    if (powerLevel == PowerSetting::BOOST) {
        std::cout << "[Cleaner] 이미 BOOST 상태입니다\n";
        return;  // NEG-17
    }

    // NEG-20: OFF 상태에서 전력 조절 차단
    if (powerLevel == PowerSetting::OFF) {
        std::cout << "[Cleaner] 비활성 상태에서 전력 조절 불가\n";
        return;
    }

    powerLevel = PowerSetting::BOOST;
    std::cout << "[Cleaner] 강력 흡입 모드\n";
}

void CleanerHandler::normalizePower() {
    if (powerLevel == PowerSetting::NORMAL) {
        std::cout << "[Cleaner] 이미 NORMAL 상태입니다\n";
        return;  // NEG-18
    }

    if (powerLevel == PowerSetting::NORMAL) return;  // NEG-18

    // NEG-19: OFF 상태에서 전력 조절 차단
    if (powerLevel == PowerSetting::OFF) {
        std::cout << "[Cleaner] 비활성 상태에서 전력 조절 불가\n";
        return;
    }

    powerLevel = PowerSetting::NORMAL;
    std::cout << "[Cleaner] 일반 모드 복구\n";
}

PowerSetting CleanerHandler::getPowerLevel() const {
    /*
    unit test 전용 함수이며 실제 구현코드에는 사용하지 않습니다.
    */
    return powerLevel;
}