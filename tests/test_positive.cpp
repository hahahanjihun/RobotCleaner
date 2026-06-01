#include "test_common.h"
#include "test_positive.h"

// ==========================================
// TC-POS-01
// UC-00 -> UC-01 연속 흐름
// startCleaning() -> 전방 장애물 감지 -> 좌측 회피 -> resumeCleaning()
// SystemState: CLEANING -> AVOIDING -> CLEANING
// ==========================================
bool TC_POS_01() {
    TestContext t;
    t.rvc.startCleaning();

    SensorData firstDetect = {false, true};
    t.obstacleSensor.notifyObstacle(firstDetect);

    SensorData rightBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(rightBlocked);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-02
// UC-00 -> UC-03 연속 흐름
// startCleaning() -> 고먼지 감지 -> boostPower() -> normalizePower() -> resumeNormalCleaning()
// SystemState: CLEANING -> BOOSTING -> CLEANING
// CleanerHandler: NORMAL -> BOOST -> NORMAL
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
// UC-00 -> UC-01 -> UC-02 연속 흐름
// startCleaning() -> 전방 장애물 -> 후진 -> 측면 재감지 -> 좌측 탈출
// MotorHandler: FORWARD -> BACKWARD -> BACKWARD(loop) -> FORWARD
// ==========================================
bool TC_POS_03() {
    TestContext t;
    t.rvc.startCleaning();

    SensorData allBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(allBlocked);

    SensorData stillBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(stillBlocked);

    SensorData leftClear = {false, false};
    t.obstacleSensor.notifyObstacle(leftClear);

    return t.motor.getStatus() == DriveSetting::FORWARD &&
            t.rvc.getSystemState() == SystemState::CLEANING;
}

// ==========================================
// TC-POS-04
// UC-00 -> UC-01 -> UC-02 연속 흐름
// startCleaning() -> 전방 장애물 -> 후진 -> 좌측 막힘 확인 -> 우측 탈출
// MotorHandler: FORWARD -> BACKWARD -> RIGHT -> FORWARD
// SystemState: CLEANING -> AVOIDING -> CLEANING
// ==========================================
bool TC_POS_04() {
     TestContext t;
    t.rvc.startCleaning();

    SensorData allBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(allBlocked);

    SensorData rightClear = {true, false};
    t.obstacleSensor.notifyObstacle(rightClear);

    return t.motor.getStatus() == DriveSetting::FORWARD &&
            t.rvc.getSystemState() == SystemState::CLEANING;
}

// ==========================================
// TC-POS-05
// UC-03 -> UC-01 연속 흐름
// 집중 청소 복귀 -> 즉시 전방 장애물 발생 -> 회피 완료
// ==========================================
bool TC_POS_05() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(80.0f);

    SensorData allBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(allBlocked);

    SensorData rightClear = {true, false};
    t.obstacleSensor.notifyObstacle(rightClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-06
// 2회 연속 전방 장애물 회피
// isEvading 플래그가 true -> false -> true -> false 로 정상 전환되는지 확인
// ==========================================
bool TC_POS_06() {
     TestContext t;
    t.rvc.startCleaning();

    SensorData leftClear = {false, true};
    SensorData rightBlocked = {true, true};

    t.obstacleSensor.notifyObstacle(leftClear);
    t.obstacleSensor.notifyObstacle(rightBlocked);

    SensorData allBlocked = {true, true};
    SensorData rightClear = {true, false};

    t.obstacleSensor.notifyObstacle(allBlocked);
    t.obstacleSensor.notifyObstacle(rightClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-07
// UC-03 2회 연속 집중 청소 수행
// BOOSTING -> CLEANING -> BOOSTING -> CLEANING 독립 수행 확인
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
// UC-00 -> UC-03 미진입 흐름
// startCleaning() -> 낮은 먼지 감지 -> boost 조건 미충족 -> 일반 청소 유지
// SystemState: CLEANING 유지
// CleanerHandler: NORMAL 유지
// MotorHandler: FORWARD 유지
// ==========================================
bool TC_POS_08() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(30.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD &&
            t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-POS-09
// UC-00 -> UC-03 -> UC-01 -> UC-02 순서로 4개 UC 전체 순차 수행
// 각 UC 전환마다 SystemState, MotorHandler, CleanerHandler 상태 검증
// ==========================================
bool TC_POS_09() {
    TestContext t;
    t.rvc.startCleaning();

    t.dustSensor.notifyDust(75.0f);

    SensorData allBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(allBlocked);

    SensorData stillBlocked = {true, true};
    t.obstacleSensor.notifyObstacle(stillBlocked);

    SensorData leftClear = {false, false};
    t.obstacleSensor.notifyObstacle(leftClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-POS-10
// UC-00 단일 흐름 (부분 장애물)
// startCleaning() -> 좌/우 장애물 감지 -> 전방 비어있음 -> 회피 없이 직진 유지
// SystemState: CLEANING 유지 (AVOIDING 미진입)
// MotorHandler: FORWARD 유지
// CleanerHandler: NORMAL 유지
// ==========================================
bool TC_POS_10() {
    TestContext t;
    t.rvc.startCleaning();

    SensorData frontClear = {true, false};
    t.obstacleSensor.notifyObstacle(frontClear);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD &&
            t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}