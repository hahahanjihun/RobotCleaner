#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <limits>

#include "../src/Types.h"
#include "../src/CleanerHandler.h"
#include "../src/MotorHandler.h"
#include "../src/RVCController.h"
#include "../src/ActionController.h"
#include "../src/DustController.h"
#include "../src/ObstacleSensorHandler.h"
#include "../src/DustSensorHandler.h"

struct TestResult {
    std::string id;
    std::string type;
    std::string name;
    bool passed;
};

class TestContext {
public:
    CleanerHandler cleaner;
    MotorHandler motor;
    RVCController rvc;
    ActionController actionCtrl;
    DustController dustCtrl;
    ObstacleSensorHandler obstacleSensor;
    DustSensorHandler dustSensor;

    TestContext()
        : rvc(&cleaner, &motor),
          actionCtrl(&motor, &cleaner, &rvc),
          dustCtrl(&cleaner, &rvc),
          obstacleSensor(&actionCtrl),
          dustSensor(&dustCtrl) {}
};

inline void addResult(
    std::vector<TestResult>& results,
    const std::string& id,
    const std::string& type,
    const std::string& name,
    const std::function<bool()>& testFunc
) {
    bool passed = false;

    try {
        passed = testFunc();
    } catch (...) {
        passed = false;
    }

    results.push_back({id, type, name, passed});
}