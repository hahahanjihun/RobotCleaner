#include <gtest/gtest.h>

#include "Types.h"
#include "CleanerHandler.h"
#include "MotorHandler.h"
#include "RVCController.h"
#include "ActionController.h"
#include "DustController.h"
#include "ObstacleSensorHandler.h"
#include "DustSensorHandler.h"

// -----------------------------
// MotorHandler Unit Test
// -----------------------------

TEST(MotorHandlerTest, InitialStateIsStopped) {
    MotorHandler motor;

    EXPECT_EQ(motor.getStatus(), DriveSetting::STOPPED);
}

TEST(MotorHandlerTest, MoveForwardChangesState) {
    MotorHandler motor;

    motor.moveForward();

    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
}

TEST(MotorHandlerTest, MoveBackwardChangesState) {
    MotorHandler motor;

    motor.moveBackward();

    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);
}

TEST(MotorHandlerTest, TurnLeftChangesState) {
    MotorHandler motor;

    motor.turnLeft();

    EXPECT_EQ(motor.getStatus(), DriveSetting::LEFT);
}

TEST(MotorHandlerTest, TurnRightChangesState) {
    MotorHandler motor;

    motor.turnRight();

    EXPECT_EQ(motor.getStatus(), DriveSetting::RIGHT);
}

TEST(MotorHandlerTest, StopMotorChangesState) {
    MotorHandler motor;

    motor.moveForward();
    motor.stopMotor();

    EXPECT_EQ(motor.getStatus(), DriveSetting::STOPPED);
}

// -----------------------------
// CleanerHandler Unit Test
// -----------------------------

TEST(CleanerHandlerTest, InitialPowerIsOff) {
    CleanerHandler cleaner;

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

TEST(CleanerHandlerTest, ActivateCleanerSetsNormal) {
    CleanerHandler cleaner;

    cleaner.activateCleaner();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

TEST(CleanerHandlerTest, DeactivateCleanerSetsOff) {
    CleanerHandler cleaner;

    cleaner.activateCleaner();
    cleaner.deactivateCleaner();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

TEST(CleanerHandlerTest, BoostPowerSetsBoost) {
    CleanerHandler cleaner;
    cleaner.activateCleaner();
    cleaner.boostPower();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::BOOST);
}

TEST(CleanerHandlerTest, NormalizePowerSetsNormal) {
    CleanerHandler cleaner;
    cleaner.activateCleaner();
    cleaner.boostPower();
    cleaner.normalizePower();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// -----------------------------
// RVCController Unit Test
// -----------------------------

TEST(RVCControllerTest, InitialStateIsIdle) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    EXPECT_EQ(rvc.getSystemState(), SystemState::IDLE);
}

TEST(RVCControllerTest, StartCleaningChangesToCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    rvc.startCleaning();

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
}

TEST(RVCControllerTest, StartCleaningTurnsOnCleanerAndMotor) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    rvc.startCleaning();

    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
}

TEST(RVCControllerTest, ChangeStateToAvoiding) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    rvc.changeState(SystemState::AVOIDING);

    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
}

TEST(RVCControllerTest, ResumeCleaningReturnsToCleaningAndForward) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    rvc.changeState(SystemState::AVOIDING);
    motor.moveBackward();

    rvc.resumeCleaning();

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
}

// -----------------------------
// ActionController Unit Test
// -----------------------------

TEST(ActionControllerTest, NoObstacleKeepsCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData loc = {false, false};
    action.obstacleStatus(loc);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

TEST(ActionControllerTest, FrontObstacleStartsAvoidingAndTurnsRight) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect;
    firstDetect.frontSensorData = true;
    firstDetect.leftSensorData = false;

    action.obstacleStatus(firstDetect);

    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::RIGHT);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

TEST(ActionControllerTest, FrontObstacleThenLeftFreeAvoidsLeftAndResumesCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect = {true, false};
    action.obstacleStatus(firstDetect);

    SensorData rightCheck = {false, false};
    action.obstacleStatus(rightCheck);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

TEST(ActionControllerTest, FrontObstacleThenRightFreeAvoidsRightAndResumesCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect = {true, true};
    action.obstacleStatus(firstDetect);

    SensorData rightCheck = {false, false};
    action.obstacleStatus(rightCheck);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

TEST(ActionControllerTest, BothSidesBlockedMovesBackward) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect = {true, true};
    action.obstacleStatus(firstDetect);

    SensorData rightCheck = {true, true};
    action.obstacleStatus(rightCheck);

    EXPECT_EQ(rvc.getSystemState(), SystemState::AVOIDING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::BACKWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

TEST(ActionControllerTest, BackwardThenLeftFreeReturnsToCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect = {true, true};
    action.obstacleStatus(firstDetect);

    SensorData bothBlocked = {true, true};
    action.obstacleStatus(bothBlocked);

    SensorData afterBackward = {true, false};
    action.obstacleStatus(afterBackward);

    SensorData rightCheck = {true, false};
    action.obstacleStatus(rightCheck);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

TEST(ActionControllerTest, BackwardThenRightFreeReturnsToCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    rvc.startCleaning();

    SensorData firstDetect = {true, true};
    action.obstacleStatus(firstDetect);

    SensorData bothBlocked = {true, true};
    action.obstacleStatus(bothBlocked);

    SensorData afterBackward = {true, true};
    action.obstacleStatus(afterBackward);

    SensorData rightCheck = {false, false};
    action.obstacleStatus(rightCheck);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(motor.getStatus(), DriveSetting::FORWARD);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}

// -----------------------------
// DustController Unit Test
// -----------------------------

TEST(DustControllerTest, LowDustDoesNothing) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    DustController dust(&cleaner, &rvc);

    dust.dustStatus(30.0f);

    EXPECT_EQ(rvc.getSystemState(), SystemState::IDLE);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::OFF);
}

TEST(DustControllerTest, HighDustBoostsAndReturnsToCleaning) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    DustController dust(&cleaner, &rvc);
    cleaner.activateCleaner();
    dust.dustStatus(80.0f);

    EXPECT_EQ(rvc.getSystemState(), SystemState::CLEANING);
    EXPECT_EQ(cleaner.getPowerLevel(), PowerSetting::NORMAL);
}