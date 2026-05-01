cmake_minimum_required(VERSION 3.14)
project(RobotCleaner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(app src/main.cpp)

add_executable(robot_tests tests/my_test.cpp src/main.cpp)
target_link_libraries(robot_tests gtest_main)

add_test(NAME MyTest COMMAND robot_tests)