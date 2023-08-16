#include <modules/TestFile.hpp>

int test_any_sources::Sum(int a, int b)
{
    return a + b;
}

/*
     Google Test (gtest) - Essential Macros and Functions:
    
    1. Basic Assertion Macros for Tests:
    - ASSERT_TRUE(condition);
    - ASSERT_FALSE(condition);
    - ASSERT_EQ(val1, val2);
    - ASSERT_NE(val1, val2);
    - ASSERT_LT(val1, val2);
    - ASSERT_LE(val1, val2);
    - ASSERT_GT(val1, val2);
    - ASSERT_GE(val1, val2);
    
    (If an ASSERT_* assertion fails, the current test terminates.)
    
    2. Basic Expectation Macros for Tests:
    - EXPECT_TRUE(condition);
    - EXPECT_FALSE(condition);
    - EXPECT_EQ(val1, val2);
    - EXPECT_NE(val1, val2);
    - EXPECT_LT(val1, val2);
    - EXPECT_LE(val1, val2);
    - EXPECT_GT(val1, val2);
    - EXPECT_GE(val1, val2);
    
    (Unlike ASSERT_* macros, EXPECT_* continues the current test even if the assertion fails.)
    
    3. Macros for Exception Handling and Floating-Point Comparisons:
    - ASSERT_THROW(statement, exception_type);
    - EXPECT_THROW(statement, exception_type);
    - ASSERT_ANY_THROW(statement);
    - EXPECT_ANY_THROW(statement);
    - ASSERT_NO_THROW(statement);
    - EXPECT_NO_THROW(statement);
    - ASSERT_FLOAT_EQ(val1, val2);
    - EXPECT_FLOAT_EQ(val1, val2);
    - ASSERT_DOUBLE_EQ(val1, val2);
    - EXPECT_DOUBLE_EQ(val1, val2);
    - ASSERT_NEAR(val1, val2, abs_error);
    - EXPECT_NEAR(val1, val2, abs_error);
    
    4. Defining Tests and Test Suites:
    - TEST(TestSuiteName, TestName) {  }
    
    5. Setup and Teardown for Tests and Test Suites :
    -class FooTest : public ::testing::Test {
    protected:
        void SetUp() override {  }
        void TearDown() override {  }
    };
    
    6. Using Parameterized Tests :
    -class FooTest : public ::testing::TestWithParam<T> { };
    -INSTANTIATE_TEST_SUITE_P(InstantiationName, FooTest, ::testing::Values(arg1, arg2, ...));
    -TEST_P(FooTest, TestName) {  }
    
    (Refer to the official Google Test documentation for more detailed information.)

 */