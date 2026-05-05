#include <gtest/gtest.h>
#include <limits>
#include "../src/Types.h"
#include "../src/CleanerHandler.h"
#include "../src/MotorHandler.h"
#include "../src/RVCController.h"
#include "../src/ActionController.h"
#include "../src/DustController.h"
#include "../src/ObstacleSensorHandler.h"
#include "../src/DustSensorHandler.h"

// ==========================================
// 테스트 픽스처: NegativeTest
// ==========================================
class NegativeTest : public ::testing::Test {
protected:
    CleanerHandler cleaner;
    MotorHandler   motor;
    RVCController  rvc;
    ActionController  actionCtrl;
    DustController    dustCtrl;
    ObstacleSensorHandler obstacleSensor;
    DustSensorHandler     dustSensor;

    NegativeTest()
        : rvc(&cleaner, &motor),
          actionCtrl(&motor, &cleaner, &rvc),
          dustCtrl(&cleaner, &rvc),
          obstacleSensor(&actionCtrl),
          dustSensor(&dustCtrl) {}
};

// ==========================================
// TC-NEG-01
// 이미 CLEANING 중일 때 startCleaning() 중복 호출
// 예상: 추가 동작 없이 CLEANING 상태 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_01_StartCleaning_DuplicateCall) {
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // 중복 호출
    rvc.startCleaning();

    // 상태 변화 없어야 함
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),       DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-NEG-02
// RVCController가 이미 CLEANING 상태인데 resumeCleaning() 호출
// 예상: if(state == CLEANING) return; 으로 상태 변화 없이 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_02_ResumeCleaning_AlreadyCleaning) {
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // 이미 CLEANING 상태에서 resumeCleaning() 호출
    rvc.resumeCleaning();

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
}

// ==========================================
// TC-NEG-03
// 정의되지 않은 유효하지 않은 값으로 changeState() 시도
// 예상: 요청 거부, 기존 SystemState 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_03_ChangeState_InvalidValue) {
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // 정의되지 않은 상태값으로 강제 변경 시도
    rvc.changeState(static_cast<SystemState>(999));

    // 기존 상태 유지 (현재 코드에는 검증 로직 없으므로 실패 → 버그 탐지)
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

// ==========================================
// TC-NEG-04
// AVOIDING 중 startCleaning() 호출
// 예상: 회피 동작 우선, 새 청소 명령 무시 → AVOIDING 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_04_StartCleaning_DuringAvoiding) {
    rvc.startCleaning();

    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // 회피 중 startCleaning() 재호출
    rvc.startCleaning();

    // AVOIDING 유지 (현재 코드에는 방어 로직 없으므로 실패 → 버그 탐지)
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
}

// ==========================================
// TC-NEG-05
// changeState() 호출 시 현재와 완전히 동일한 상태로 변경 요청
// 예상: 불필요한 상태 전환 로직 수행 안 함, 상태 그대로 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_05_ChangeState_SameState) {
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // 동일한 상태로 변경 요청
    rvc.changeState(SystemState::CLEANING);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

// ==========================================
// TC-NEG-06
// Position 데이터가 비정상 형식일 때 (모든 필드 false)
// 예상: ActionController가 이를 무시하고 stopMotor() 호출
// ==========================================
TEST_F(NegativeTest, TC_NEG_06_Position_InvalidData) {
    rvc.startCleaning();
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);

    // 비정상 Position 데이터 (모든 값 false) 전달
    Position emptyPos = {false, false, false};
    obstacleSensor.notifyFrontObstacle(emptyPos);

    // 안전을 위해 stopMotor() 호출되어야 함 (현재 코드에 방어 로직 없으므로 실패 → 버그 탐지)
    EXPECT_EQ(motor.getStatus(), DriveSetting::STOPPED);
}

// ==========================================
// TC-NEG-07
// isEvading 중 다시 전방 장애물 발생
// 예상: 기존 회피 동작 그대로 진행 (이중 진입 방지)
// ==========================================
TEST_F(NegativeTest, TC_NEG_07_FrontObstacle_DuringEvading) {
    rvc.startCleaning();

    // 1차 전방 장애물 → AVOIDING 진입
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);

    // 2차 전방 장애물 (isEvading=true 상태에서 재호출)
    obstacleSensor.notifyFrontObstacle(allBlocked);

    // 기존 회피 동작 유지, 이중 처리 없음
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

// ==========================================
// TC-NEG-08
// MotorHandler 상태가 이미 FORWARD인데 다시 moveForward() 호출
// 예상: 불필요한 신호 발생 차단, FORWARD 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_08_MoveForward_AlreadyForward) {
    rvc.startCleaning();
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);

    motor.moveForward();

    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
}

// ==========================================
// TC-NEG-09
// MotorHandler 상태가 이미 BACKWARD인데 다시 moveBackward() 호출
// 예상: 불필요한 신호 발생 차단, BACKWARD 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_09_MoveBackward_AlreadyBackward) {
    rvc.startCleaning();

    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);

    motor.moveBackward();

    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);
}

// ==========================================
// TC-NEG-10
// MotorHandler에서 turnLeft() 호출 시 이미 LEFT 상태인 경우
// 예상: 회전 명령 중복 처리 방지, LEFT 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_10_TurnLeft_AlreadyLeft) {
    motor.turnLeft();
    EXPECT_EQ(motor.getStatus(), DriveSetting::LEFT);

    motor.turnLeft();

    EXPECT_EQ(motor.getStatus(), DriveSetting::LEFT);
}

// ==========================================
// TC-NEG-11
// MotorHandler에서 turnRight() 호출 시 이미 RIGHT 상태인 경우
// 예상: 회전 명령 중복 처리 방지, RIGHT 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_11_TurnRight_AlreadyRight) {
    motor.turnRight();
    EXPECT_EQ(motor.getStatus(), DriveSetting::RIGHT);

    motor.turnRight();

    EXPECT_EQ(motor.getStatus(), DriveSetting::RIGHT);
}

// ==========================================
// TC-NEG-12
// stopMotor() 호출 시 이미 STOPPED 상태인 경우
// 예상: 아무 동작 하지 않음, STOPPED 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_12_StopMotor_AlreadyStopped) {
    // 초기 상태는 STOPPED
    EXPECT_EQ(motor.getStatus(), DriveSetting::STOPPED);

    motor.stopMotor();

    EXPECT_EQ(motor.getStatus(), DriveSetting::STOPPED);
}

// ==========================================
// TC-NEG-13
// DustLevel에 음수 값 입력
// 예상: DustController 유효성 검사 후 명령 전달 차단, 상태 변화 없음
// ==========================================
TEST_F(NegativeTest, TC_NEG_13_DustLevel_Negative) {
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    dustSensor.notifyDust(-1.0f);

    // 음수 입력이므로 BOOSTING 전환 없어야 함
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-NEG-14
// DustLevel에 float 범위를 초과하는 매우 큰 값 입력
// 예상: 최대 PowerSetting(BOOST)까지만 상승, 크래시 없이 정상 복귀
// (주의: 약 5초 소요)
// ==========================================
TEST_F(NegativeTest, TC_NEG_14_DustLevel_VeryLargeValue) {
    rvc.startCleaning();

    // 매우 큰 float 값 입력
    dustSensor.notifyDust(std::numeric_limits<float>::max());

    // 5초 후 정상 복귀, 크래시 없음
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-NEG-15
// CleanerHandler가 이미 OFF인데 deactivateCleaner() 호출
// 예상: 중복 명령 무시, OFF 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_15_DeactivateCleaner_AlreadyOff) {
    // 초기 상태는 OFF
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);

    cleaner.deactivateCleaner();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

// ==========================================
// TC-NEG-16
// CleanerHandler가 이미 NORMAL(ON)인데 activateCleaner() 호출
// 예상: 중복 명령 무시, NORMAL 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_16_ActivateCleaner_AlreadyOn) {
    cleaner.activateCleaner();
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    cleaner.activateCleaner();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-NEG-17
// boostPower() 중 다시 boostPower() 요청
// 예상: 이미 최고 출력이므로 중복 명령 무시, BOOST 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_17_BoostPower_DuplicateCall) {
    cleaner.activateCleaner();
    cleaner.boostPower();
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::BOOST);

    cleaner.boostPower();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::BOOST);
}

// ==========================================
// TC-NEG-18
// normalizePower() 중 다시 normalizePower() 요청
// 예상: 상태 변화 없이 NORMAL 유지
// ==========================================
TEST_F(NegativeTest, TC_NEG_18_NormalizePower_DuplicateCall) {
    cleaner.activateCleaner();
    cleaner.normalizePower();
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    cleaner.normalizePower();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-NEG-19
// Cleaner가 비활성화(OFF) 상태에서 normalizePower() 호출
// 예상: activate() 전에는 전력 조절 명령을 수행하지 않음, OFF 유지
// (현재 코드에 방어 로직 없으므로 실패 → 버그 탐지)
// ==========================================
TEST_F(NegativeTest, TC_NEG_19_NormalizePower_WhileCleanerOff) {
    // 초기 상태는 OFF (activate 호출 없음)
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);

    cleaner.normalizePower();

    // OFF 상태에서 normalizePower()는 수행되지 않아야 함
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

// ==========================================
// TC-NEG-20
// Cleaner가 비활성화(OFF) 상태에서 boostPower() 호출
// 예상: activate() 전에는 전력 조절 명령을 수행하지 않음, OFF 유지
// (현재 코드에 방어 로직 없으므로 실패 → 버그 탐지)
// ==========================================
TEST_F(NegativeTest, TC_NEG_20_BoostPower_WhileCleanerOff) {
    // 초기 상태는 OFF (activate 호출 없음)
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);

    cleaner.boostPower();

    // OFF 상태에서 boostPower()는 수행되지 않아야 함
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}
