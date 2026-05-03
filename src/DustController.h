#pragma once
#include "Types.h"

class CleanerHandler; 
class RVCController;

class DustController {
private:
    CleanerHandler* cleanerHandler;
    RVCController* rvcController; // 메인 시스템 상태 보고용 포인터

public:
    DustController(CleanerHandler* ch, RVCController* rvc);
    void dustStatus(float dustLevel); // 먼지량에 따른 집중 청소 판단
};