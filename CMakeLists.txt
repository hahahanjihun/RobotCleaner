# ---------------------------------------------------------
# 커버리지 옵션 적용
# ---------------------------------------------------------
add_compile_options(--coverage)
add_link_options(--coverage)
# -----------------------------------------------------------------------------
# 1. 기초 설정 (Basic Settings)
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.14)
project(RobotCleaner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# 2. 정적 분석 도구 설정 
# 사용법: cmake -DENABLE_STATIC_ANALYSIS=ON ..
# -----------------------------------------------------------------------------
option(ENABLE_STATIC_ANALYSIS "Enable cppcheck and clang-tidy analysis" OFF)

if(ENABLE_STATIC_ANALYSIS)
    # Cppcheck: 논리적 오류 및 버그 탐지
    find_program(CPPCHECK_PATH cppcheck)
    if(CPPCHECK_PATH)
        set(CMAKE_CXX_CPPCHECK "${CPPCHECK_PATH};--enable=all;--suppress=missingIncludeSystem")
    endif()

    # Clang-Tidy: 코드 가독성 및 최신 문법 검사
    find_program(CLANG_TIDY_PATH clang-tidy)
    if(CLANG_TIDY_PATH)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_PATH};-checks=-*,readability-*,modernize-*,performance-*")
        # macOS 사용자(Apple Silicon)를 위한 SDK 경로 추가 설정
        if(APPLE)
            execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
            list(APPEND CMAKE_CXX_CLANG_TIDY "--extra-arg=-isysroot" "--extra-arg=${SDK_PATH}")
        endif()
    endif()
endif()

# -----------------------------------------------------------------------------
# 3. 구글 테스트(GTest) 의존성 관리
# -----------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Windows 등 다른 환경에서의 호환성을 위해 shared runtime 설정
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -----------------------------------------------------------------------------
# 4. 타겟(Target) 설정: 로직 라이브러리 및 실행 파일
# -----------------------------------------------------------------------------


# RobotLogic: 실제 로봇 청소기 기능이 구현된 핵심 소스들 
# 실제 앱과 테스트 앱에서 공통으로 사용하므로 따로 묶어서 관리함
add_library(RobotLogic
    src/CleanerHandler.cpp
    src/MotorHandler.cpp
    src/RVCController.cpp
    src/ActionController.cpp
    src/DustController.cpp
    src/ObstacleSensorHandler.cpp
    src/DustSensorHandler.cpp
)

# RobotLogic을 사용하는 모든 타겟이 
# 자동으로 src 폴더의 헤더 파일들을 찾을 수 있게 해주는 설정
target_include_directories(RobotLogic PUBLIC src)

# app: 실제 프로그램을 실행하는 실행 파일
add_executable(app src/main.cpp)
target_link_libraries(app RobotLogic) # 핵심 로직 연결

# robot_tests: 단위 테스트를 수행하는 실행 파일
# 모든 테스트용 .cpp 파일을 여기에 포함시켜야 함
add_executable(robot_tests
    tests/unit_test.cpp
    tests/simulator.cpp
    tests/test_positive.cpp
    tests/test_negative.cpp
)
# GTest의 main() 함수와 핵심 로직을 연결
target_link_libraries(robot_tests
    RobotLogic
    gtest_main
)

# -----------------------------------------------------------------------------
# 5. 테스트 등록 (Test Registration)
# -----------------------------------------------------------------------------
enable_testing()

add_test(NAME MyRobotSystemTest COMMAND robot_tests)

# System Test HTML Report용 실행 파일
add_executable(system_test
    tests/simulator.cpp
    tests/test_positive.cpp
    tests/test_negative.cpp
)

target_link_libraries(system_test
    RobotLogic
)
