#include <gtest/gtest.h>
#include "../src/Types.h"
#include "../src/CleanerHandler.h"
#include "../src/MotorHandler.h"
#include "../src/RVCController.h"
#include "../src/ActionController.h"
#include "../src/DustController.h"
#include "../src/ObstacleSensorHandler.h"
#include "../src/DustSensorHandler.h"

// ==========================================
// 테스트 픽스처: PositiveTest
// ==========================================
class PositiveTest : public ::testing::Test {
protected:
    CleanerHandler cleaner;
    MotorHandler   motor;
    RVCController  rvc;
    ActionController  actionCtrl;
    DustController    dustCtrl;
    ObstacleSensorHandler obstacleSensor;
    DustSensorHandler     dustSensor;

    PositiveTest()
        : rvc(&cleaner, &motor),
          actionCtrl(&motor, &cleaner, &rvc),
          dustCtrl(&cleaner, &rvc),
          obstacleSensor(&actionCtrl),
          dustSensor(&dustCtrl) {}
};

// ==========================================
// TC-POS-01
// UC-00 → UC-01 연속 흐름
// startCleaning() → 전방 장애물 감지 → 좌측 회피 → resumeCleaning()
// SystemState: CLEANING → AVOIDING → CLEANING
// ==========================================
TEST_F(PositiveTest, TC_POS_01_UC00_UC01_FullFlow) {
    // Step 1: 청소 시작
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // Step 2: 전방 장애물 감지 (양측 막힘 → 후진)
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);

    // Step 3: 측면 재감지 → 좌측 CLEAR → 좌회전 후 resumeCleaning()
    Position leftClear = {false, false, true};
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
}

// ==========================================
// TC-POS-02
// UC-00 → UC-03 연속 흐름
// startCleaning() → 고먼지 감지 → boostPower() → normalizePower() → resumeNormalCleaning()
// SystemState: CLEANING → BOOSTING → CLEANING
// CleanerHandler: NORMAL → BOOST → NORMAL
// (주의: DustController 내부 5초 딜레이로 인해 약 5초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_02_UC00_UC03_FullFlow) {
    // Step 1: 청소 시작
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // Step 2: 고먼지 감지 → BOOSTING 진입 후 5초 대기 → CLEANING 복귀 (블로킹 호출)
    dustSensor.notifyDust(80.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-POS-03
// UC-00 → UC-01 → UC-02 연속 흐름
// startCleaning() → 전방 장애물 → 후진 → 측면 재감지 → 좌측 탈출
// MotorHandler: FORWARD → BACKWARD → BACKWARD(loop) → FORWARD
// ==========================================
TEST_F(PositiveTest, TC_POS_03_UC00_UC01_UC02_MotorSequence) {
    // Step 1: 청소 시작
    rvc.startCleaning();
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);

    // Step 2: 전방 장애물 감지 → 양측 막힘 → 후진
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // Step 3: 측면 재감지 → 여전히 양측 막힘 → 후진 유지 (loop)
    Position stillBlocked = {false, true, true};
    obstacleSensor.notifySideObstacle(stillBlocked);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // Step 4: 측면 재감지 → 좌측 CLEAR → 좌회전 후 전진 복귀
    Position leftClear = {false, false, true};
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

// ==========================================
// TC-POS-04
// UC-01 → UC-03 연속 흐름
// 회피 완료 → 즉시 고먼지 감지 → 집중 청소 → 복귀
// (주의: 약 5초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_04_UC01_UC03_FullFlow) {
    // Step 1: 청소 시작 + 회피
    rvc.startCleaning();
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // Step 2: 좌측 탈출 → CLEANING 복귀
    Position leftClear = {false, false, true};
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // Step 3: 회피 직후 고먼지 감지 → BOOSTING → 5초 후 CLEANING 복귀
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-POS-05
// UC-03 → UC-01 연속 흐름
// 집중 청소 복귀 → 즉시 전방 장애물 발생 → 회피 완료
// (주의: 약 5초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_05_UC03_UC01_FullFlow) {
    // Step 1: 청소 시작
    rvc.startCleaning();

    // Step 2: 고먼지 감지 → BOOSTING → 5초 후 CLEANING 복귀
    dustSensor.notifyDust(80.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // Step 3: 복귀 직후 전방 장애물 → AVOIDING
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // Step 4: 우측 탈출 → CLEANING 복귀
    Position rightClear = {false, true, false};
    obstacleSensor.notifySideObstacle(rightClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
}

// ==========================================
// TC-POS-06
// 2회 연속 전방 장애물 회피
// isEvading 플래그가 true → false → true → false 로 정상 전환되는지 확인
// ==========================================
TEST_F(PositiveTest, TC_POS_06_TwoConsecutiveAvoidances) {
    rvc.startCleaning();

    // [1차 회피]
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    Position leftClear = {false, false, true};
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // [2차 회피]
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    Position rightClear = {false, true, false};
    obstacleSensor.notifySideObstacle(rightClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
}

// ==========================================
// TC-POS-07
// UC-03 2회 연속 집중 청소 수행
// BOOSTING → CLEANING → BOOSTING → CLEANING 독립 수행 확인
// (주의: 약 10초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_07_TwoConsecutiveBoosting) {
    rvc.startCleaning();

    // [1차 집중 청소]
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // [2차 집중 청소]
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-POS-08
// UC-02 loop → UC-03 연속 흐름
// 후진 2회 반복 → 우측 탈출 → 청소 재개 → 고먼지 발생
// (주의: 약 5초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_08_UC02Loop_UC03_FullFlow) {
    rvc.startCleaning();

    // Step 1: 전방 장애물 → 양측 막힘 → 후진
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    // Step 2: 여전히 양측 막힘 → 후진 반복 (loop)
    Position stillBlocked = {false, true, true};
    obstacleSensor.notifySideObstacle(stillBlocked);
    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);

    // Step 3: 우측 CLEAR → 우회전 + resumeCleaning()
    Position rightClear = {false, true, false};
    obstacleSensor.notifySideObstacle(rightClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);

    // Step 4: 탈출 직후 고먼지 감지 → BOOSTING → 5초 후 CLEANING
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// ==========================================
// TC-POS-09
// UC-00 → UC-03 → UC-01 → UC-02 순서로 4개 UC 전체 순차 수행
// 각 UC 전환마다 SystemState, MotorHandler, CleanerHandler 상태 검증
// (주의: 약 5초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_09_AllFourUC_SequentialFlow) {
    // [UC-00] 청소 시작
    rvc.startCleaning();
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),       DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // [UC-03] 고먼지 감지 → 집중 청소 → 복귀 (5초 블로킹)
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // [UC-01 진입] 전방 장애물 → 양측 막힘 → 후진
    Position allBlocked = {true, true, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::BACKWARD);

    // [UC-02] 측면 재감지 → 여전히 막힘 → 후진 유지
    Position stillBlocked = {false, true, true};
    obstacleSensor.notifySideObstacle(stillBlocked);
    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);

    // [UC-02 탈출] 좌측 CLEAR → 청소 복귀
    Position leftClear = {false, false, true};
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);
}

// ==========================================
// TC-POS-10
// 전체 시나리오 반복 수행
// UC-00 시작 후 UC-01, UC-02, UC-03 을 각 2회씩 교차 수행
// 매 사이클마다 상태 오염 없이 정상 복귀 확인
// (주의: 약 10초 소요)
// ==========================================
TEST_F(PositiveTest, TC_POS_10_FullScenario_TwoCycles) {
    rvc.startCleaning();

    // ── Cycle 1 ──────────────────────────────
    // UC-01: 좌측 회피
    Position allBlocked = {true, true, true};
    Position leftClear  = {false, false, true};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    obstacleSensor.notifySideObstacle(leftClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);

    // UC-03: 집중 청소 (5초 블로킹)
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);

    // ── Cycle 2 ──────────────────────────────
    // UC-02: 후진 loop 후 우측 탈출
    Position stillBlocked = {false, true, true};
    Position rightClear   = {false, true, false};
    obstacleSensor.notifyFrontObstacle(allBlocked);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    obstacleSensor.notifySideObstacle(stillBlocked);
    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);
    obstacleSensor.notifySideObstacle(rightClear);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(),    DriveSetting::FORWARD);

    // UC-03: 집중 청소 (5초 블로킹)
    dustSensor.notifyDust(75.0f);
    EXPECT_EQ(rvc.getSystemState(),    SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
    EXPECT_EQ(motor.getStatus(),       DriveSetting::FORWARD);
}
