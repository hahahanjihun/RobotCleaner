cmake_minimum_required(VERSION 3.14)
project(RobotCleaner)

# Google Test를 자동으로 인터넷에서 다운로드하는 설정
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 테스트용 실행 파일 생성 (main.cpp를 테스트 대상으로 포함)
add_executable(run_tests tests/my_test.cpp)
target_link_libraries(run_tests gtest_main)