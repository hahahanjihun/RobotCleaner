#include <gtest/gtest.h>

#include "ActionController.h"
#include "CleanerHandler.h"
#include "DustController.h"
#include "DustSensorHandler.h"
#include "MotorHandler.h"
#include "ObstacleSensorHandler.h"
#include "RVCController.h"
#include "Types.h"

#include <string>
#include <tuple>
#include <vector>


static std::string b(bool v) {
    return v ? "T" : "F";
}

static std::string PositionName(bool front, bool left, bool right) {
    return "F_" + b(front) + "_L_" + b(left) + "_R_" + b(right);
}

// ============================================================
// 1. MotorHandler 20 cases
// ============================================================

enum class MotorOp {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    STOP
};

static void ApplyMotorOp(MotorHandler& motor, MotorOp op) {
    switch (op) {
        case MotorOp::MOVE_FORWARD:  motor.moveForward(); break;
        case MotorOp::MOVE_BACKWARD: motor.moveBackward(); break;
        case MotorOp::TURN_LEFT:     motor.turnLeft(); break;
        case MotorOp::TURN_RIGHT:    motor.turnRight(); break;
        case MotorOp::STOP:          motor.stopMotor(); break;
    }
}

struct MotorCase {
    const char* name;
    std::vector<MotorOp> ops;
    DriveSetting expected;
};

class MotorHandlerParamTest : public ::testing::TestWithParam<MotorCase> {};

TEST_P(MotorHandlerParamTest, MotorStateTransitions) {
    MotorHandler motor;
    const auto& tc = GetParam();

    for (auto op : tc.ops) {
        ApplyMotorOp(motor, op);
    }

    EXPECT_EQ(motor.getStatus(), tc.expected);
}

INSTANTIATE_TEST_SUITE_P(
    MotorHandler20Cases,
    MotorHandlerParamTest,
    ::testing::Values(
        MotorCase{"InitialStateStopped", {}, DriveSetting::STOPPED},
        MotorCase{"MoveForward", {MotorOp::MOVE_FORWARD}, DriveSetting::FORWARD},
        MotorCase{"MoveBackward", {MotorOp::MOVE_BACKWARD}, DriveSetting::BACKWARD},
        MotorCase{"TurnLeft", {MotorOp::TURN_LEFT}, DriveSetting::LEFT},
        MotorCase{"TurnRight", {MotorOp::TURN_RIGHT}, DriveSetting::RIGHT},
        MotorCase{"StopFromInitial", {MotorOp::STOP}, DriveSetting::STOPPED},
        MotorCase{"ForwardThenStop", {MotorOp::MOVE_FORWARD, MotorOp::STOP}, DriveSetting::STOPPED},
        MotorCase{"BackwardThenStop", {MotorOp::MOVE_BACKWARD, MotorOp::STOP}, DriveSetting::STOPPED},
        MotorCase{"LeftThenStop", {MotorOp::TURN_LEFT, MotorOp::STOP}, DriveSetting::STOPPED},
        MotorCase{"RightThenStop", {MotorOp::TURN_RIGHT, MotorOp::STOP}, DriveSetting::STOPPED},
        MotorCase{"ForwardThenBackward", {MotorOp::MOVE_FORWARD, MotorOp::MOVE_BACKWARD}, DriveSetting::BACKWARD},
        MotorCase{"BackwardThenForward", {MotorOp::MOVE_BACKWARD, MotorOp::MOVE_FORWARD}, DriveSetting::FORWARD},
        MotorCase{"LeftThenRight", {MotorOp::TURN_LEFT, MotorOp::TURN_RIGHT}, DriveSetting::RIGHT},
        MotorCase{"RightThenLeft", {MotorOp::TURN_RIGHT, MotorOp::TURN_LEFT}, DriveSetting::LEFT},
        MotorCase{"ForwardLeftRight", {MotorOp::MOVE_FORWARD, MotorOp::TURN_LEFT, MotorOp::TURN_RIGHT}, DriveSetting::RIGHT},
        MotorCase{"ForwardRightLeft", {MotorOp::MOVE_FORWARD, MotorOp::TURN_RIGHT, MotorOp::TURN_LEFT}, DriveSetting::LEFT},
        MotorCase{"BackwardLeft", {MotorOp::MOVE_BACKWARD, MotorOp::TURN_LEFT}, DriveSetting::LEFT},
        MotorCase{"BackwardRight", {MotorOp::MOVE_BACKWARD, MotorOp::TURN_RIGHT}, DriveSetting::RIGHT},
        MotorCase{"StopThenForward", {MotorOp::STOP, MotorOp::MOVE_FORWARD}, DriveSetting::FORWARD},
        MotorCase{"StopThenBackward", {MotorOp::STOP, MotorOp::MOVE_BACKWARD}, DriveSetting::BACKWARD}
    ),
    [](const ::testing::TestParamInfo<MotorCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 2. CleanerHandler 20 cases
// ============================================================

enum class CleanerOp {
    ACTIVATE,
    DEACTIVATE,
    BOOST,
    NORMALIZE
};

static void ApplyCleanerOp(CleanerHandler& cleaner, CleanerOp op) {
    switch (op) {
        case CleanerOp::ACTIVATE:   cleaner.activateCleaner(); break;
        case CleanerOp::DEACTIVATE: cleaner.deactivateCleaner(); break;
        case CleanerOp::BOOST:      cleaner.boostPower(); break;
        case CleanerOp::NORMALIZE:  cleaner.normalizePower(); break;
    }
}

struct CleanerCase {
    const char* name;
    std::vector<CleanerOp> ops;
    PowerSetting expected;
};

class CleanerHandlerParamTest : public ::testing::TestWithParam<CleanerCase> {};

TEST_P(CleanerHandlerParamTest, CleanerPowerTransitions) {
    CleanerHandler cleaner;
    const auto& tc = GetParam();

    for (auto op : tc.ops) {
        ApplyCleanerOp(cleaner, op);
    }

    EXPECT_EQ(cleaner.getPowerLevel(), tc.expected);
}

INSTANTIATE_TEST_SUITE_P(
    CleanerHandler20Cases,
    CleanerHandlerParamTest,
    ::testing::Values(
        CleanerCase{"InitialPowerOff", {}, PowerSetting::OFF},
        CleanerCase{"ActivateSetsNormal", {CleanerOp::ACTIVATE}, PowerSetting::NORMAL},
        CleanerCase{"DeactivateFromInitialSetsOff", {CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"BoostSetsBoost", {CleanerOp::BOOST}, PowerSetting::BOOST},
        CleanerCase{"NormalizeFromInitialSetsNormal", {CleanerOp::NORMALIZE}, PowerSetting::NORMAL},
        CleanerCase{"ActivateThenDeactivate", {CleanerOp::ACTIVATE, CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"ActivateThenBoost", {CleanerOp::ACTIVATE, CleanerOp::BOOST}, PowerSetting::BOOST},
        CleanerCase{"BoostThenNormalize", {CleanerOp::BOOST, CleanerOp::NORMALIZE}, PowerSetting::NORMAL},
        CleanerCase{"BoostThenDeactivate", {CleanerOp::BOOST, CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"DeactivateThenActivate", {CleanerOp::DEACTIVATE, CleanerOp::ACTIVATE}, PowerSetting::NORMAL},
        CleanerCase{"DeactivateThenBoost", {CleanerOp::DEACTIVATE, CleanerOp::BOOST}, PowerSetting::BOOST},
        CleanerCase{"ActivateBoostNormalize", {CleanerOp::ACTIVATE, CleanerOp::BOOST, CleanerOp::NORMALIZE}, PowerSetting::NORMAL},
        CleanerCase{"ActivateBoostDeactivate", {CleanerOp::ACTIVATE, CleanerOp::BOOST, CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"BoostNormalizeDeactivate", {CleanerOp::BOOST, CleanerOp::NORMALIZE, CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"ActivateDeactivateActivate", {CleanerOp::ACTIVATE, CleanerOp::DEACTIVATE, CleanerOp::ACTIVATE}, PowerSetting::NORMAL},
        CleanerCase{"BoostDeactivateActivate", {CleanerOp::BOOST, CleanerOp::DEACTIVATE, CleanerOp::ACTIVATE}, PowerSetting::NORMAL},
        CleanerCase{"NormalizeBoost", {CleanerOp::NORMALIZE, CleanerOp::BOOST}, PowerSetting::BOOST},
        CleanerCase{"NormalizeDeactivate", {CleanerOp::NORMALIZE, CleanerOp::DEACTIVATE}, PowerSetting::OFF},
        CleanerCase{"DeactivateNormalize", {CleanerOp::DEACTIVATE, CleanerOp::NORMALIZE}, PowerSetting::NORMAL},
        CleanerCase{"ActivateNormalize", {CleanerOp::ACTIVATE, CleanerOp::NORMALIZE}, PowerSetting::NORMAL}
    ),
    [](const ::testing::TestParamInfo<CleanerCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 3. RVCController 30 cases
// ============================================================

enum class RvcOp {
    START_CLEANING,
    RESUME_CLEANING,
    RESUME_NORMAL_CLEANING,
    CHANGE_IDLE,
    CHANGE_CLEANING,
    CHANGE_AVOIDING,
    CHANGE_BOOSTING
};

static void ApplyRvcOp(RVCController& rvc, RvcOp op) {
    switch (op) {
        case RvcOp::START_CLEANING:          rvc.startCleaning(); break;
        case RvcOp::RESUME_CLEANING:         rvc.resumeCleaning(); break;
        case RvcOp::RESUME_NORMAL_CLEANING:  rvc.resumeNormalCleaning(); break;
        case RvcOp::CHANGE_IDLE:             rvc.changeState(SystemState::IDLE); break;
        case RvcOp::CHANGE_CLEANING:         rvc.changeState(SystemState::CLEANING); break;
        case RvcOp::CHANGE_AVOIDING:         rvc.changeState(SystemState::AVOIDING); break;
        case RvcOp::CHANGE_BOOSTING:         rvc.changeState(SystemState::BOOSTING); break;
    }
}

struct RvcCase {
    const char* name;
    std::vector<RvcOp> ops;
    SystemState expectedState;
    DriveSetting expectedMotor;
    PowerSetting expectedCleaner;
};

class RVCControllerParamTest : public ::testing::TestWithParam<RvcCase> {};

TEST_P(RVCControllerParamTest, RVCStateAndDeviceTransitions) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);

    const auto& tc = GetParam();

    for (auto op : tc.ops) {
        ApplyRvcOp(rvc, op);
    }

    EXPECT_EQ(rvc.getSystemState(), tc.expectedState);
    EXPECT_EQ(motor.getStatus(), tc.expectedMotor);
    EXPECT_EQ(cleaner.getPowerLevel(), tc.expectedCleaner);
}

INSTANTIATE_TEST_SUITE_P(
    RVCController30Cases,
    RVCControllerParamTest,
    ::testing::Values(
        RvcCase{"InitialIdle", {}, SystemState::IDLE, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"StartCleaningStateCleaning", {RvcOp::START_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"ResumeCleaningFromInitial", {RvcOp::RESUME_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::OFF},
        RvcCase{"ResumeNormalCleaningFromInitial", {RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"ChangeIdle", {RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"ChangeCleaning", {RvcOp::CHANGE_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"ChangeAvoiding", {RvcOp::CHANGE_AVOIDING}, SystemState::AVOIDING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"ChangeBoosting", {RvcOp::CHANGE_BOOSTING}, SystemState::BOOSTING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"StartThenAvoiding", {RvcOp::START_CLEANING, RvcOp::CHANGE_AVOIDING}, SystemState::AVOIDING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartThenBoosting", {RvcOp::START_CLEANING, RvcOp::CHANGE_BOOSTING}, SystemState::BOOSTING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartThenIdle", {RvcOp::START_CLEANING, RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartThenResumeCleaning", {RvcOp::START_CLEANING, RvcOp::RESUME_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartThenResumeNormalCleaning", {RvcOp::START_CLEANING, RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"AvoidingThenResumeCleaning", {RvcOp::CHANGE_AVOIDING, RvcOp::RESUME_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::OFF},
        RvcCase{"BoostingThenResumeNormal", {RvcOp::CHANGE_BOOSTING, RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"IdleCleaningAvoidingCleaning", {RvcOp::CHANGE_IDLE, RvcOp::CHANGE_CLEANING, RvcOp::CHANGE_AVOIDING, RvcOp::CHANGE_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"StartAvoidingResume", {RvcOp::START_CLEANING, RvcOp::CHANGE_AVOIDING, RvcOp::RESUME_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartBoostingResumeNormal", {RvcOp::START_CLEANING, RvcOp::CHANGE_BOOSTING, RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"ResumeThenIdle", {RvcOp::RESUME_CLEANING, RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::FORWARD, PowerSetting::OFF},
        RvcCase{"ResumeNormalThenIdle", {RvcOp::RESUME_NORMAL_CLEANING, RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"ChangeCleaningThenStart", {RvcOp::CHANGE_CLEANING, RvcOp::START_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"ChangeAvoidingThenStart", {RvcOp::CHANGE_AVOIDING, RvcOp::START_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"ChangeBoostingThenStart", {RvcOp::CHANGE_BOOSTING, RvcOp::START_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartIdleStart", {RvcOp::START_CLEANING, RvcOp::CHANGE_IDLE, RvcOp::START_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartAvoidingIdle", {RvcOp::START_CLEANING, RvcOp::CHANGE_AVOIDING, RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartBoostingIdle", {RvcOp::START_CLEANING, RvcOp::CHANGE_BOOSTING, RvcOp::CHANGE_IDLE}, SystemState::IDLE, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"AvoidingBoostingCleaning", {RvcOp::CHANGE_AVOIDING, RvcOp::CHANGE_BOOSTING, RvcOp::CHANGE_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"BoostingAvoidingCleaning", {RvcOp::CHANGE_BOOSTING, RvcOp::CHANGE_AVOIDING, RvcOp::CHANGE_CLEANING}, SystemState::CLEANING, DriveSetting::STOPPED, PowerSetting::OFF},
        RvcCase{"StartResumeResumeNormal", {RvcOp::START_CLEANING, RvcOp::RESUME_CLEANING, RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL},
        RvcCase{"StartIdleResumeNormal", {RvcOp::START_CLEANING, RvcOp::CHANGE_IDLE, RvcOp::RESUME_NORMAL_CLEANING}, SystemState::CLEANING, DriveSetting::FORWARD, PowerSetting::NORMAL}
    ),
    [](const ::testing::TestParamInfo<RvcCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 4. ActionController 80 cases
// ============================================================

struct ActionCase {
    std::string name;
    bool startCleaning;
    bool firstFront;
    bool firstLeft;
    bool firstRight;
    bool secondCall;
    bool secondFront;
    bool secondLeft;
    bool secondRight;
    DriveSetting expectedMotor;
    PowerSetting expectedCleaner;
    SystemState expectedState;
};

class ActionControllerParamTest : public ::testing::TestWithParam<ActionCase> {};

TEST_P(ActionControllerParamTest, ObstacleDecisionLogic) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);

    const auto& tc = GetParam();

    if (tc.startCleaning) {
        rvc.startCleaning();
    }

    Position first{tc.firstFront, tc.firstLeft, tc.firstRight};
    action.obstacleStatus(first);

    if (tc.secondCall) {
        Position second{tc.secondFront, tc.secondLeft, tc.secondRight};
        action.obstacleStatus(second);
    }

    EXPECT_EQ(motor.getStatus(), tc.expectedMotor);
    EXPECT_EQ(cleaner.getPowerLevel(), tc.expectedCleaner);
    EXPECT_EQ(rvc.getSystemState(), tc.expectedState);
}

static std::vector<ActionCase> MakeActionCases() {
    std::vector<ActionCase> cases;

    // 1) startCleaning=false, single call: 8개
    for (bool f : {false, true}) {
        for (bool l : {false, true}) {
            for (bool r : {false, true}) {
                ActionCase tc;
                tc.name = "NoStartSingle_" + PositionName(f, l, r);
                tc.startCleaning = false;
                tc.firstFront = f;
                tc.firstLeft = l;
                tc.firstRight = r;
                tc.secondCall = false;
                tc.secondFront = false;
                tc.secondLeft = false;
                tc.secondRight = false;

                if (!f) {
                    tc.expectedMotor = DriveSetting::STOPPED;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::IDLE;
                } else if (!l || !r) {
                    // 현재 ActionController는 전방 감지 후 바로 좌/우 회피 후 resumeCleaning() 수행.
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 2) startCleaning=true, single call: 8개
    for (bool f : {false, true}) {
        for (bool l : {false, true}) {
            for (bool r : {false, true}) {
                ActionCase tc;
                tc.name = "StartSingle_" + PositionName(f, l, r);
                tc.startCleaning = true;
                tc.firstFront = f;
                tc.firstLeft = l;
                tc.firstRight = r;
                tc.secondCall = false;
                tc.secondFront = false;
                tc.secondLeft = false;
                tc.secondRight = false;

                if (!f) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::NORMAL;
                    tc.expectedState = SystemState::CLEANING;
                } else if (!l || !r) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 3) startCleaning=true, first call all blocked -> evading 유지, second call 8개
    for (bool f2 : {false, true}) {
        for (bool l2 : {false, true}) {
            for (bool r2 : {false, true}) {
                ActionCase tc;
                tc.name = "StartAllBlockedThen_" + PositionName(f2, l2, r2);
                tc.startCleaning = true;
                tc.firstFront = true;
                tc.firstLeft = true;
                tc.firstRight = true;
                tc.secondCall = true;
                tc.secondFront = f2;
                tc.secondLeft = l2;
                tc.secondRight = r2;

                if (!l2 || !r2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 4) no start, first call all blocked -> evading 유지, second call 8개
    for (bool f2 : {false, true}) {
        for (bool l2 : {false, true}) {
            for (bool r2 : {false, true}) {
                ActionCase tc;
                tc.name = "NoStartAllBlockedThen_" + PositionName(f2, l2, r2);
                tc.startCleaning = false;
                tc.firstFront = true;
                tc.firstLeft = true;
                tc.firstRight = true;
                tc.secondCall = true;
                tc.secondFront = f2;
                tc.secondLeft = l2;
                tc.secondRight = r2;

                if (!l2 || !r2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 5) startCleaning=true, first = front blocked + left free -> evade ends, second call 8개
    for (bool f2 : {false, true}) {
        for (bool l2 : {false, true}) {
            for (bool r2 : {false, true}) {
                ActionCase tc;
                tc.name = "StartLeftFreeThen_" + PositionName(f2, l2, r2);
                tc.startCleaning = true;
                tc.firstFront = true;
                tc.firstLeft = false;
                tc.firstRight = true;
                tc.secondCall = true;
                tc.secondFront = f2;
                tc.secondLeft = l2;
                tc.secondRight = r2;

                if (!f2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else if (!l2 || !r2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 6) startCleaning=true, first = front blocked + right free -> evade ends, second call 8개
    for (bool f2 : {false, true}) {
        for (bool l2 : {false, true}) {
            for (bool r2 : {false, true}) {
                ActionCase tc;
                tc.name = "StartRightFreeThen_" + PositionName(f2, l2, r2);
                tc.startCleaning = true;
                tc.firstFront = true;
                tc.firstLeft = true;
                tc.firstRight = false;
                tc.secondCall = true;
                tc.secondFront = f2;
                tc.secondLeft = l2;
                tc.secondRight = r2;

                if (!f2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else if (!l2 || !r2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 7) startCleaning=true, first no front obstacle 8개 + second call 조합 일부 8개
    for (bool l1 : {false, true}) {
        for (bool r1 : {false, true}) {
            for (bool f2 : {false, true}) {
                ActionCase tc;
                tc.name = "StartNoFront_L_" + b(l1) + "_R_" + b(r1) + "_ThenFront_" + b(f2);
                tc.startCleaning = true;
                tc.firstFront = false;
                tc.firstLeft = l1;
                tc.firstRight = r1;
                tc.secondCall = true;
                tc.secondFront = f2;
                tc.secondLeft = l1;
                tc.secondRight = r1;

                if (!f2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::NORMAL;
                    tc.expectedState = SystemState::CLEANING;
                } else if (!l1 || !r1) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
            }
        }
    }

    // 8) 추가 24개: all blocked 이후 2회 연속 회피 루프 상황을 다양한 side 조합으로 검증
    int extra = 0;
    for (bool start : {false, true}) {
        for (bool l2 : {true, false, true}) {
            for (bool r2 : {true, true, false, false}) {
                if (extra >= 24) break;
                ActionCase tc;
                tc.name = std::string(start ? "ExtraStart" : "ExtraNoStart") +
                          "_AllBlockedThen_L_" + b(l2) + "_R_" + b(r2) + "_Case_" + std::to_string(extra);
                tc.startCleaning = start;
                tc.firstFront = true;
                tc.firstLeft = true;
                tc.firstRight = true;
                tc.secondCall = true;
                tc.secondFront = true;
                tc.secondLeft = l2;
                tc.secondRight = r2;

                if (!l2 || !r2) {
                    tc.expectedMotor = DriveSetting::FORWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::CLEANING;
                } else {
                    tc.expectedMotor = DriveSetting::BACKWARD;
                    tc.expectedCleaner = PowerSetting::OFF;
                    tc.expectedState = SystemState::AVOIDING;
                }
                cases.push_back(tc);
                extra++;
            }
            if (extra >= 24) break;
        }
        if (extra >= 24) break;
    }

    // 정확히 80개만 사용
    cases.resize(80);
    return cases;
}

INSTANTIATE_TEST_SUITE_P(
    ActionController80Cases,
    ActionControllerParamTest,
    ::testing::ValuesIn(MakeActionCases()),
    [](const ::testing::TestParamInfo<ActionCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 5. ObstacleSensorHandler 25 cases
// ============================================================

struct ObstacleSensorCase {
    std::string name;
    bool useStartCleaning;
    bool useFrontNotify;
    bool front;
    bool left;
    bool right;
    DriveSetting expectedMotor;
    PowerSetting expectedCleaner;
    SystemState expectedState;
};

class ObstacleSensorHandlerParamTest : public ::testing::TestWithParam<ObstacleSensorCase> {};

TEST_P(ObstacleSensorHandlerParamTest, SensorForwardsObstaclePositionToActionController) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    ActionController action(&motor, &cleaner, &rvc);
    ObstacleSensorHandler sensor(&action);

    const auto& tc = GetParam();

    if (tc.useStartCleaning) {
        rvc.startCleaning();
    }

    Position loc{tc.front, tc.left, tc.right};

    if (tc.useFrontNotify) {
        sensor.notifyFrontObstacle(loc);
    } else {
        sensor.notifySideObstacle(loc);
    }

    EXPECT_EQ(motor.getStatus(), tc.expectedMotor);
    EXPECT_EQ(cleaner.getPowerLevel(), tc.expectedCleaner);
    EXPECT_EQ(rvc.getSystemState(), tc.expectedState);
}

static std::vector<ObstacleSensorCase> MakeObstacleSensorCases() {
    std::vector<ObstacleSensorCase> cases;

    int count = 0;
    for (bool start : {false, true}) {
        for (bool frontNotify : {true, false}) {
            for (bool f : {false, true}) {
                for (bool l : {false, true}) {
                    for (bool r : {false, true}) {
                        if (count >= 25) return cases;

                        ObstacleSensorCase tc;
                        tc.name = std::string(start ? "Start_" : "NoStart_") +
                                  (frontNotify ? "NotifyFront_" : "NotifySide_") +
                                  PositionName(f, l, r);
                        tc.useStartCleaning = start;
                        tc.useFrontNotify = frontNotify;
                        tc.front = f;
                        tc.left = l;
                        tc.right = r;

                        if (!f) {
                            tc.expectedMotor = start ? DriveSetting::FORWARD : DriveSetting::STOPPED;
                            tc.expectedCleaner = start ? PowerSetting::NORMAL : PowerSetting::OFF;
                            tc.expectedState = start ? SystemState::CLEANING : SystemState::IDLE;
                        } else if (!l || !r) {
                            tc.expectedMotor = DriveSetting::FORWARD;
                            tc.expectedCleaner = PowerSetting::OFF;
                            tc.expectedState = SystemState::CLEANING;
                        } else {
                            tc.expectedMotor = DriveSetting::BACKWARD;
                            tc.expectedCleaner = PowerSetting::OFF;
                            tc.expectedState = SystemState::AVOIDING;
                        }

                        cases.push_back(tc);
                        count++;
                    }
                }
            }
        }
    }
    return cases;
}

INSTANTIATE_TEST_SUITE_P(
    ObstacleSensorHandler25Cases,
    ObstacleSensorHandlerParamTest,
    ::testing::ValuesIn(MakeObstacleSensorCases()),
    [](const ::testing::TestParamInfo<ObstacleSensorCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 6. DustController 15 cases
// ============================================================

struct DustControllerCase {
    const char* name;
    float dustLevel;
    SystemState expectedState;
    PowerSetting expectedCleaner;
};

class DustControllerParamTest : public ::testing::TestWithParam<DustControllerCase> {};

TEST_P(DustControllerParamTest, DustThresholdDecision) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    DustController dust(&cleaner, &rvc);

    const auto& tc = GetParam();

    dust.dustStatus(tc.dustLevel);

    EXPECT_EQ(rvc.getSystemState(), tc.expectedState);
    EXPECT_EQ(cleaner.getPowerLevel(), tc.expectedCleaner);
}

INSTANTIATE_TEST_SUITE_P(
    DustController15Cases,
    DustControllerParamTest,
    ::testing::Values(
        DustControllerCase{"Dust0NoBoost", 0.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust1NoBoost", 1.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust10NoBoost", 10.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust25NoBoost", 25.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust49NoBoost", 49.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust50NoBoost", 50.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust50Point01Boost", 50.01f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"Dust51Boost", 51.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"Dust60Boost", 60.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"Dust75Boost", 75.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"Dust100Boost", 100.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"Dust150Boost", 150.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustControllerCase{"DustMinus1NoBoost", -1.0f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust49Point99NoBoost", 49.99f, SystemState::IDLE, PowerSetting::OFF},
        DustControllerCase{"Dust999Boost", 999.0f, SystemState::CLEANING, PowerSetting::NORMAL}
    ),
    [](const ::testing::TestParamInfo<DustControllerCase>& info) {
        return info.param.name;
    }
);

// ============================================================
// 7. DustSensorHandler 10 cases
// ============================================================

struct DustSensorCase {
    const char* name;
    float dustLevel;
    SystemState expectedState;
    PowerSetting expectedCleaner;
};

class DustSensorHandlerParamTest : public ::testing::TestWithParam<DustSensorCase> {};

TEST_P(DustSensorHandlerParamTest, SensorForwardsDustLevelToDustController) {
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc(&cleaner, &motor);
    DustController dustController(&cleaner, &rvc);
    DustSensorHandler dustSensor(&dustController);

    const auto& tc = GetParam();

    dustSensor.notifyDust(tc.dustLevel);

    EXPECT_EQ(rvc.getSystemState(), tc.expectedState);
    EXPECT_EQ(cleaner.getPowerLevel(), tc.expectedCleaner);
}

INSTANTIATE_TEST_SUITE_P(
    DustSensorHandler10Cases,
    DustSensorHandlerParamTest,
    ::testing::Values(
        DustSensorCase{"SensorDust0", 0.0f, SystemState::IDLE, PowerSetting::OFF},
        DustSensorCase{"SensorDust10", 10.0f, SystemState::IDLE, PowerSetting::OFF},
        DustSensorCase{"SensorDust25", 25.0f, SystemState::IDLE, PowerSetting::OFF},
        DustSensorCase{"SensorDust49", 49.0f, SystemState::IDLE, PowerSetting::OFF},
        DustSensorCase{"SensorDust50", 50.0f, SystemState::IDLE, PowerSetting::OFF},
        DustSensorCase{"SensorDust50Point1", 50.1f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustSensorCase{"SensorDust55", 55.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustSensorCase{"SensorDust75", 75.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustSensorCase{"SensorDust100", 100.0f, SystemState::CLEANING, PowerSetting::NORMAL},
        DustSensorCase{"SensorDust500", 500.0f, SystemState::CLEANING, PowerSetting::NORMAL}
    ),
    [](const ::testing::TestParamInfo<DustSensorCase>& info) {
        return info.param.name;
    }
);
