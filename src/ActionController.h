#pragma once
#include "Types.h"

class MotorHandler;   
class CleanerHandler;
class RVCController;

class ActionController {
private:
    MotorHandler* motorHandler;
    CleanerHandler* cleanerHandler;
    RVCController* rvcController; // 메인 시스템 상태 보고용 포인터
    bool isEvading = false; // actioncontroller 스스로 회피 중인지 기억하는 로컬 변수
public:
    ActionController(MotorHandler* mh, CleanerHandler* ch, RVCController* rvc);
    void obstacleStatus(Position location); // 장애물 위치에 따른 판단 및 명령
};