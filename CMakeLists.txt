cmake_minimum_required(VERSION 3.14)
project(RobotCleaner)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# 실행 파일
add_executable(app src/main.cpp)

# 테스트 파일
add_executable(run_tests tests/my_test.cpp src/main.cpp)
target_link_libraries(run_tests gtest_main)

add_test(NAME MyTest COMMAND run_tests)