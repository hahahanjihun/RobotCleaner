#include <gtest/gtest.h>
#include "Types.h"
#include "CleanerHandler.h"
#include "MotorHandler.h"
#include "RVCController.h"
#include "ActionController.h"
#include "DustController.h"
#include "ObstacleSensorHandler.h"
#include "DustSensorHandler.h"

// ==========================================
// 테스트 픽스처: RVCIntegrationTest
// ==========================================
class RVCIntegrationTest : public ::testing::Test {
protected:
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc;
    ActionController actionCtrl;
    DustController dustCtrl;
    ObstacleSensorHandler obstacleSensor;
    DustSensorHandler dustSensor;

    RVCIntegrationTest() 
        : rvc(&cleaner, &motor), 
          actionCtrl(&motor, &cleaner, &rvc), 
          dustCtrl(&cleaner, &rvc),
          obstacleSensor(&actionCtrl),
          dustSensor(&dustCtrl) {}
};

// ==========================================
// [UC-00 & UC-01] 전방 장애물 감지 후 한쪽으로 회피
// ==========================================
TEST_F(RVCIntegrationTest, UC00_UC01_EvadeLeftScenario) {
    rvc.startCleaning(); 

    Position fakeObstacle;
    fakeObstacle.isFrontBlocked = true;
    fakeObstacle.isLeftBlocked = false; 
    fakeObstacle.isRightBlocked = true;

    obstacleSensor.notifyFrontObstacle(fakeObstacle);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING); 
}

// ==========================================
// [UC-02] 3면이 막힌 구석에서의 후진 루프
// ==========================================
TEST_F(RVCIntegrationTest, UC02_EvadeBackwardLoopScenario) {
    rvc.startCleaning();

    Position trappedPos = {true, true, true};
    obstacleSensor.notifyFrontObstacle(trappedPos);

   
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    Position stillTrappedPos = {false, true, true};
    obstacleSensor.notifySideObstacle(stillTrappedPos);
    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);

    Position escapedPos = {false, false, true};
    obstacleSensor.notifySideObstacle(escapedPos);
    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

// ==========================================
// [UC-03] 먼지 임계치 초과 시 집중 청소
// ==========================================
TEST_F(RVCIntegrationTest, UC03_DustFocusCleaningScenario) {
    rvc.startCleaning();

    dustSensor.notifyDust(80.5f);


    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

