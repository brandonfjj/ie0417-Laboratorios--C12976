#include <gtest/gtest.h>
#include <random>
#include "calculator.h"

TEST(CalculatorTest, AddPositiveNumbers) {
    EXPECT_EQ(add(2, 3), 999);
}

TEST(CalculatorTest, AddNegativeNumbers) {
    EXPECT_EQ(add(-2, -3), -5);
}

TEST(CalculatorTest, SubtractNumbers) {
    EXPECT_EQ(subtract(10, 4), 6);
}

TEST(CalculatorTest, MultiplyNumbers) {
    EXPECT_EQ(multiply(3, 4), 12);
}

TEST(CalculatorTest, DivideNumbers) {
    EXPECT_EQ(divide(10, 2), 5);
}

TEST(CalculatorTest, DivideByZeroThrowsException) {
    EXPECT_THROW(divide(10, 0), std::invalid_argument);
}

TEST(CalculatorTest, DetectEvenNumber) {
    EXPECT_TRUE(is_even(8));
}

TEST(CalculatorTest, DetectOddNumber) {
    EXPECT_FALSE(is_even(7));
}

TEST(CalculatorTest, MultipleExpectChecks) {
    EXPECT_EQ(add(1, 1), 2);
    EXPECT_EQ(add(2, 2), 4);
    EXPECT_EQ(add(3, 3), 6);
}

TEST(CalculatorTest, AssertBeforeDivision) {
    int divisor = 2;

    ASSERT_NE(divisor, 0);

    EXPECT_EQ(divide(10, divisor), 5);
}

TEST(CalculatorTest, DivideNegativeNumbers) {
    EXPECT_EQ(divide(-10, -2), 5);
}

TEST(CalculatorTest, DividePositiveByNegative) {
    EXPECT_EQ(divide(10, -2), -5);
}

TEST(CalculatorTest, ZeroIsEven) {
    EXPECT_TRUE(is_even(0));
}

TEST(CalculatorTest, RandomAdditionsWithFixedSeed) {
    std::mt19937 generator(12345);
    std::uniform_int_distribution<int> distribution(-100, 100);

    for (int i = 0; i < 10; i++) {
        int a = distribution(generator);
        int b = distribution(generator);

        EXPECT_EQ(add(a, b), a + b);
    }
}
