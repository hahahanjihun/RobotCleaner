#include <gtest/gtest.h>

// 아주 간단한 테스트: 1과 1이 같은지 확인
TEST(BasicTest, CheckOneOne) {
    EXPECT_EQ(1, 1);
}