#include <gtest/gtest.h>

int add(int, int);

TEST(BasicTest, AddTest) {
    EXPECT_EQ(add(1, 1), 2);
}