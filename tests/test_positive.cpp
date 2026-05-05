#include "test_common.h"
#include "test_positive.h"

// ==========================================
// TC-POS-01
// UC-00 → UC-01 연속 흐름
// startCleaning() → 전방 장애물 감지 → 좌측 회피 → resumeCleaning()
// SystemState: CLEANING → AVOIDING → CLEANING
// ==========================================
bool TC_POS_01() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    Position leftClear = {false, false, true};
    t.obstacleSensor.notifySideObstacle(leftClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-02
// UC-00 → UC-03 연속 흐름
// startCleaning() → 고먼지 감지 → boostPower() → normalizePower() → resumeNormalCleaning()
// SystemState: CLEANING → BOOSTING → CLEANING
// CleanerHandler: NORMAL → BOOST → NORMAL
// (주의: DustController 내부 5초 딜레이로 인해 약 5초 소요)
// ==========================================
bool TC_POS_02() {
    TestContext t;
    t.rvc.startCleaning();
    t.dustSensor.notifyDust(80.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-POS-03
// UC-00 → UC-01 → UC-02 연속 흐름
// startCleaning() → 전방 장애물 → 후진 → 측면 재감지 → 좌측 탈출
// MotorHandler: FORWARD → BACKWARD → BACKWARD(loop) → FORWARD
// ==========================================
bool TC_POS_03() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    Position stillBlocked = {false, true, true};
    t.obstacleSensor.notifySideObstacle(stillBlocked);

    Position leftClear = {false, false, true};
    t.obstacleSensor.notifySideObstacle(leftClear);

    return t.motor.getStatus() == DriveSetting::FORWARD &&
           t.rvc.getSystemState() == SystemState::CLEANING;
}

// ==========================================
// TC-POS-04
// UC-01 → UC-03 연속 흐름
// 회피 완료 → 즉시 고먼지 감지 → 집중 청소 → 복귀
// (주의: 약 5초 소요)
// ==========================================
bool TC_POS_04() {
    TestContext t;

    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    if (t.rvc.getSystemState() != SystemState::AVOIDING) return false;

    Position leftClear = {false, false, true};
    t.obstacleSensor.notifySideObstacle(leftClear);
    if (t.rvc.getSystemState() != SystemState::CLEANING) return false;

    t.dustSensor.notifyDust(75.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-POS-05
// UC-03 → UC-01 연속 흐름
// 집중 청소 복귀 → 즉시 전방 장애물 발생 → 회피 완료
// (주의: 약 5초 소요)
// ==========================================
bool TC_POS_05() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(80.0f);

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    Position rightClear = {false, true, false};
    t.obstacleSensor.notifySideObstacle(rightClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-06
// 2회 연속 전방 장애물 회피
// isEvading 플래그가 true → false → true → false 로 정상 전환되는지 확인
// ==========================================
bool TC_POS_06() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    Position leftClear = {false, false, true};
    Position rightClear = {false, true, false};

    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    t.obstacleSensor.notifySideObstacle(leftClear);

    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    t.obstacleSensor.notifySideObstacle(rightClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-07
// UC-03 2회 연속 집중 청소 수행
// BOOSTING → CLEANING → BOOSTING → CLEANING 독립 수행 확인
// (주의: 약 10초 소요)
// ==========================================
bool TC_POS_07() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(75.0f);
    bool first = t.rvc.getSystemState() == SystemState::CLEANING &&
                 t.cleaner.getPowerLevel() == PowerSetting::NORMAL;

    t.dustSensor.notifyDust(75.0f);
    bool second = t.rvc.getSystemState() == SystemState::CLEANING &&
                  t.cleaner.getPowerLevel() == PowerSetting::NORMAL;

    return first && second;
}

// ==========================================
// TC-POS-08
// UC-02 loop → UC-03 연속 흐름
// 후진 2회 반복 → 우측 탈출 → 청소 재개 → 고먼지 발생
// (주의: 약 5초 소요)
// ==========================================
bool TC_POS_08() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    if (t.motor.getStatus() != DriveSetting::BACKWARD) return false;
    if (t.rvc.getSystemState() != SystemState::AVOIDING) return false;

    Position stillBlocked = {false, true, true};
    t.obstacleSensor.notifySideObstacle(stillBlocked);

    if (t.motor.getStatus() != DriveSetting::BACKWARD) return false;

    Position rightClear = {false, true, false};
    t.obstacleSensor.notifySideObstacle(rightClear);

    if (t.rvc.getSystemState() != SystemState::CLEANING) return false;
    if (t.motor.getStatus() != DriveSetting::FORWARD) return false;

    t.dustSensor.notifyDust(75.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-POS-09
// UC-00 → UC-03 → UC-01 → UC-02 순서로 4개 UC 전체 순차 수행
// 각 UC 전환마다 SystemState, MotorHandler, CleanerHandler 상태 검증
// (주의: 약 5초 소요)
// ==========================================
bool TC_POS_09() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(75.0f);

    Position allBlocked = {true, true, true};
    Position stillBlocked = {false, true, true};
    Position leftClear = {false, false, true};

    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    t.obstacleSensor.notifySideObstacle(stillBlocked);
    t.obstacleSensor.notifySideObstacle(leftClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-10
// 전체 시나리오 반복 수행
// UC-00 시작 후 UC-01, UC-02, UC-03 을 각 2회씩 교차 수행
// 매 사이클마다 상태 오염 없이 정상 복귀 확인
// (주의: 약 10초 소요)
// ==========================================
bool TC_POS_10() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    Position leftClear  = {false, false, true};

    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    if (t.rvc.getSystemState() != SystemState::AVOIDING) return false;

    t.obstacleSensor.notifySideObstacle(leftClear);
    if (t.rvc.getSystemState() != SystemState::CLEANING) return false;

    t.dustSensor.notifyDust(75.0f);

    if (t.rvc.getSystemState() != SystemState::CLEANING) return false;
    if (t.cleaner.getPowerLevel() != PowerSetting::NORMAL) return false;

    Position stillBlocked = {false, true, true};
    Position rightClear   = {false, true, false};

    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    if (t.rvc.getSystemState() != SystemState::AVOIDING) return false;

    t.obstacleSensor.notifySideObstacle(stillBlocked);
    if (t.motor.getStatus() != DriveSetting::BACKWARD) return false;

    t.obstacleSensor.notifySideObstacle(rightClear);
    if (t.rvc.getSystemState() != SystemState::CLEANING) return false;
    if (t.motor.getStatus() != DriveSetting::FORWARD) return false;

    t.dustSensor.notifyDust(75.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
           t.cleaner.getPowerLevel() == PowerSetting::NORMAL &&
           t.motor.getStatus() == DriveSetting::FORWARD;
}