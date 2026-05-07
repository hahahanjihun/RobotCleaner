#pragma once
#include "Types.h"

// 먼지 흡입 모터 제어 핸들러
class CleanerHandler {
private:
    PowerSetting powerLevel = PowerSetting::OFF; // 현재 흡입 모터 세기 

public:
    CleanerHandler();
    
    // 흡입 모터 동작 명령
    void activateCleaner();   // 청소기 흡입 켜기
    void deactivateCleaner(); // 청소기 흡입 끄기
    void boostPower();        // 강력 모드로 변경
    void normalizePower();    // 일반 모드로 복구

     // GTest에서 상태 검증을 위한 Getter. unit test 전용 함수이며 실제 구현코드에는 사용하지 않습니다.
    PowerSetting getPowerLevel() const;          
};