
#include <gtest/gtest.h>

TEST(thread_pool_test, thread_pool_create) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}