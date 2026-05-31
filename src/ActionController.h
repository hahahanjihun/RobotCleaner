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

    bool isMovingBackward = false; // 추가 된 필드 - 현재 뒤로 후진 중인지

    bool checkingRight = false; // 추가된 필드

    // 추가 - 현재 장애물의 유무를 저장 및 회피 로직에 사용하지 위함
    Position location; // 현재까지 감지된 장애물 상태

public:
    ActionController(MotorHandler* mh, CleanerHandler* ch, RVCController* rvc);

    // Position에서 SensorData로 변경
    void obstacleStatus(SensorData location); // 장애물 위치에 따른 판단 및 명령
};
