#pragma once

// 시스템의 현재 동작 상태 (System State)
enum class SystemState{ 
    IDLE,       // 대기 상태
    CLEANING,   // 청소 중
    AVOIDING,   // 회피 중
    BOOSTING    // 강화 청소 중
};

// 모터의 구동 설정 (Drive Setting)
enum class DriveSetting { 
    STOPPED,    // 정지
    FORWARD,    // 전진
    BACKWARD,   // 후진
    LEFT,       // 좌회전
    RIGHT       // 우회전
};

// 흡입기 전원 설정 (Power Setting)
enum class PowerSetting { 
    OFF,        // 꺼짐
    NORMAL,     // 일반 모드
    BOOST       // 강력 모드
};

// 전/좌/우 방향의 장애물 감지 여부 (Position)
struct Position {
    bool isFrontBlocked = false; // 전방 장애물 유무 (true: 있음, false: 없음)
    bool isLeftBlocked = false;  // 좌측 장애물 유무
    bool isRightBlocked = false; // 우측 장애물 유무
};

// 추가 코드 - ObstacleSensorHandler가 센서데이터 값을 받을 때 사용하기 위함.
struct SensorData{
    bool leftSensorData = false; // 좌측 장애물 유무 (true: 있음, false: 없음), 회피 중 turn right 상태일 때는 전방 장애물 유무  기능
    bool frontSensorData = false; // 전방 장애물 유무 (true: 있음, false: 없음), 회피 중 turn right 상태일 때는 우측 장애물 유무 판단 기능
};