#include <gtest/gtest.h>
#include <modules/TestFile.hpp>

TEST(Test, DISABLED_Sum)
{
    ASSERT_TRUE(test_any_sources::Sum(1, 2) == 3);
}
