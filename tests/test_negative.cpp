#include "test_common.h"
#include "test_negative.h"

// ==========================================
// TC-NEG-01
// 이미 CLEANING 중일 때 startCleaning() 중복 호출
// 예상: 추가 동작 없이 CLEANING 상태 유지
// ==========================================
bool TC_NEG_01() {
    TestContext t;
    t.rvc.startCleaning();
    t.rvc.startCleaning();

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD &&
            t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-NEG-02
// RVCController가 이미 CLEANING 상태인데 resumeCleaning() 호출
// 예상: if(state == CLEANING) return; 으로 상태 변화 없이 유지
// ==========================================
bool TC_NEG_02() {
    TestContext t;
    t.rvc.startCleaning();
    t.rvc.resumeCleaning();

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-NEG-03
// 정의되지 않은 유효하지 않은 값으로 changeState() 시도
// 예상: 요청 거부, 기존 SystemState 유지
// ==========================================
bool TC_NEG_03() {
    TestContext t;
    t.rvc.startCleaning();
    t.rvc.changeState(static_cast<SystemState>(999));

    return t.rvc.getSystemState() == SystemState::CLEANING;
}

// ==========================================
// TC-NEG-04
// AVOIDING 중 startCleaning() 호출
// 예상: 회피 동작 우선, 새 청소 명령 무시 -> AVOIDING 유지
// ==========================================
bool TC_NEG_04() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    t.rvc.startCleaning();

    return t.rvc.getSystemState() == SystemState::AVOIDING &&
            t.motor.getStatus() == DriveSetting::BACKWARD;
}

// ==========================================
// TC-NEG-05
// changeState() 호출 시 현재와 완전히 동일한 상태로 변경 요청
// 예상: 불필요한 상태 전환 로직 수행 안 함, 상태 그대로 유지
// ==========================================
bool TC_NEG_05() {
    TestContext t;
    t.rvc.startCleaning();
    t.rvc.changeState(SystemState::CLEANING);

    return t.rvc.getSystemState() == SystemState::CLEANING;
}

// ==========================================
// TC-NEG-06
// moveForward 중 정지 단계가 생략된 급격한 방향 전환(turnLeft(), turnLigit() 호출)
// 예상: 비정상적인 상태로 인지하고, 현재 구동 상태를 유지
// ==========================================
bool TC_NEG_06() {
    TestContext t;
    t.rvc.startCleaning();
    t.motor.turnLeft();
    t.motor.turnRight();

    return t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-NEG-07
// isEvading 중 다시 전방 장애물 발생
// 예상: 기존 회피 동작 그대로 진행 (이중 진입 방지)
// ==========================================
bool TC_NEG_07() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    return t.rvc.getSystemState() == SystemState::AVOIDING &&
            t.motor.getStatus() == DriveSetting::BACKWARD &&
            t.cleaner.getPowerLevel() == PowerSetting::OFF;
}

// ==========================================
// TC-NEG-08
// MotorHandler 상태가 이미 FORWARD인데 다시 moveForward() 호출
// 예상: 불필요한 신호 발생 차단, FORWARD 유지
// ==========================================
bool TC_NEG_08() {
    TestContext t;
    t.rvc.startCleaning();
    t.motor.moveForward();

    return t.motor.getStatus() == DriveSetting::FORWARD;
}

// ==========================================
// TC-NEG-09
// MotorHandler 상태가 이미 BACKWARD인데 다시 moveBackward() 호출
// 예상: 불필요한 신호 발생 차단, BACKWARD 유지
// ==========================================
bool TC_NEG_09() {
    TestContext t;
    t.rvc.startCleaning();

    Position allBlocked = {true, true, true};
    t.obstacleSensor.notifyFrontObstacle(allBlocked);

    t.motor.moveBackward();

    return t.motor.getStatus() == DriveSetting::BACKWARD;
}

// ==========================================
// TC-NEG-10
// MotorHandler에서 turnLeft() 호출 시 이미 LEFT 상태인 경우
// 예상: 회전 명령 중복 처리 방지, LEFT 유지
// ==========================================
bool TC_NEG_10() {
    TestContext t;
    t.rvc.startCleaning();

    t.motor.stopMotor();

    t.motor.turnLeft();
    t.motor.turnLeft();

    return t.motor.getStatus() == DriveSetting::LEFT;
}

// ==========================================
// TC-NEG-11
// MotorHandler에서 turnRight() 호출 시 이미 RIGHT 상태인 경우
// 예상: 회전 명령 중복 처리 방지, RIGHT 유지
// ==========================================
bool TC_NEG_11() {
    TestContext t;
    t.rvc.startCleaning();

    t.motor.stopMotor();

    t.motor.turnRight();
    t.motor.turnRight();

    return t.motor.getStatus() == DriveSetting::RIGHT;
}

// ==========================================
// TC-NEG-12
// stopMotor() 호출 시 이미 STOPPED 상태인 경우
// 예상: 아무 동작 하지 않음, STOPPED 유지
// ==========================================
bool TC_NEG_12() {
    TestContext t;
    t.rvc.startCleaning();

    t.motor.stopMotor();
    t.motor.stopMotor();

    return t.motor.getStatus() == DriveSetting::STOPPED;
}

// ==========================================
// TC-NEG-13
// DustLevel에 음수 값 입력
// 예상: DustController 유효성 검사 후 명령 전달 차단, 상태 변화 없음
// ==========================================
bool TC_NEG_13() {
    TestContext t;
    t.rvc.startCleaning();
    t.dustSensor.notifyDust(-1.0f);

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-NEG-14
// DustLevel에 float 범위를 초과하는 매우 큰 값 입력
// 예상: 최대 PowerSetting(BOOST)까지만 상승, 크래시 없이 정상 복귀
// ==========================================
bool TC_NEG_14() {
    TestContext t;
    t.rvc.startCleaning();
    t.dustSensor.notifyDust(std::numeric_limits<float>::max());

    return t.rvc.getSystemState() == SystemState::CLEANING &&
            t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}


// ==========================================
// TC-NEG-15
// CleanerHandler가 이미 OFF인데 deactivateCleaner() 호출
// 예상: 중복 명령 무시, OFF 유지
// ==========================================
bool TC_NEG_15() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.deactivateCleaner();
    t.cleaner.deactivateCleaner();

    return t.cleaner.getPowerLevel() == PowerSetting::OFF;
}

// ==========================================
// TC-NEG-16
// CleanerHandler가 이미 NORMAL(ON)인데 activateCleaner() 호출
// 예상: 중복 명령 무시, NORMAL 유지
// ==========================================
bool TC_NEG_16() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.activateCleaner();

    return t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-NEG-17
// boostPower() 중 다시 boostPower() 요청
// 예상: 이미 최고 출력이므로 중복 명령 무시, BOOST 유지
// ==========================================
bool TC_NEG_17() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.boostPower();
    t.cleaner.boostPower();

    return t.cleaner.getPowerLevel() == PowerSetting::BOOST;
}

// ==========================================
// TC-NEG-18
// normalizePower() 중 다시 normalizePower() 요청
// 예상: 상태 변화 없이 NORMAL 유지
// ==========================================
bool TC_NEG_18() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.normalizePower();

    return t.cleaner.getPowerLevel() == PowerSetting::NORMAL;
}

// ==========================================
// TC-NEG-19
// Cleaner가 비활성화(OFF) 상태에서 normalizePower() 호출
// 예상: activate() 전에는 전력 조절 명령을 수행하지 않음, OFF 유지
// ==========================================
bool TC_NEG_19() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.deactivateCleaner();
    t.cleaner.normalizePower();

    return t.cleaner.getPowerLevel() == PowerSetting::OFF;
}

// ==========================================
// TC-NEG-20
// Cleaner가 비활성화(OFF) 상태에서 boostPower() 호출
// 예상: activate() 전에는 전력 조절 명령을 수행하지 않음, OFF 유지
// ==========================================
bool TC_NEG_20() {
    TestContext t;
    t.rvc.startCleaning();

    t.cleaner.deactivateCleaner();
    t.cleaner.boostPower();

    return t.cleaner.getPowerLevel() == PowerSetting::OFF;
}