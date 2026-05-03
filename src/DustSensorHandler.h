#pragma once
#include "Types.h"

class DustController; // 전방 선언

class DustSensorHandler {
private:
    float dustLevel = 0.0f; // 현재 감지된 먼지량 (기본 0)
    DustController* dustController; // 데이터를 전달할 두뇌 포인터

public:
    DustSensorHandler(DustController* dc);
    void notifyDust(float level); // 새로운 먼지량 감지 알림
};