#include "DustSensorHandler.h"
#include "DustController.h"

DustSensorHandler::DustSensorHandler(DustController* dc) 
    : dustController(dc) {}

void DustSensorHandler::notifyDust(float level) {
    dustLevel = level; // 센서 데이터 업데이트
    dustController->dustStatus(dustLevel); // 판단을 위해 컨트롤러로 전달
}