// gtest의 FRIEND_TEST 사용
// 이것들은 내가 그냥 "이런 식으로 하면 된다~" 느낌으로 
// 넣어둔 코드니까 상훈이 형이 고치거나 더 추가해서 하면 될듯

#include <gtest/gtest.h>
#include "CleanerHandler.h"
#include "Types.h"

class CleanerHandlerTest : public ::testing::Test {
protected:
    CleanerHandler cleaner;

    // 부모 클래스(CleanerHandlerTest)가 자식(테스트 코드)을 위해 비공개 멤버를 반환해줌
    auto getPowerLevel() -> PowerSetting { return cleaner.powerLevel; }
};

// 1. 청소기 가동 및 중지 테스트
TEST_F(CleanerHandlerTest, ActivationTest) {
    cleaner.activateCleaner();
    // cleaner.powerLevel 대신 징검다리 함수 호출!
    EXPECT_NE(getPowerLevel(), PowerSetting::OFF); 

    cleaner.deactivateCleaner();
    EXPECT_EQ(getPowerLevel(), PowerSetting::OFF);
}

// 2. 파워 모드 변경 테스트
TEST_F(CleanerHandlerTest, PowerModeTest) {
    cleaner.activateCleaner();
    cleaner.boostPower();
    
    EXPECT_EQ(getPowerLevel(), PowerSetting::BOOST);
    
    cleaner.normalizePower();
    EXPECT_EQ(getPowerLevel(), PowerSetting::NORMAL);
}