#pragma once
#include "Types.h"

class CleanerHandler; 
class MotorHandler;

class RVCController {
private:
    SystemState systemState = SystemState::IDLE; // 기본 시스템 상태는 IDLE
    CleanerHandler* cleanerHandler; // 청소기 제어 포인터
    MotorHandler* motorHandler;     // 모터 제어 포인터

public:
    RVCController(CleanerHandler* ch, MotorHandler* mh);

    void startCleaning();        // 자동 청소 시작 (UC-00)
    void resumeCleaning();       // 회피 종료 후 청소 재개 (UC-01, 02)
    void resumeNormalCleaning(); // 집중 청소 종료 후 청소 재개 (UC-03)
    
    void changeState(SystemState newState); // 서브 컨트롤러의 상태 동기화용 메서드
    
    SystemState getSystemState() const; // GTest에서 상태 검증을 위한 Getter. unit test 전용 함수이며 실제 구현코드에는 사용하지 않습니다.
};