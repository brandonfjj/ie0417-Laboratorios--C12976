#include <gtest/gtest.h>
#include <vector>
#include "grade_utils.h"

TEST(GradeUtilsTest, CalculateAverage) {
    std::vector<int> grades = {80, 90, 100};
    EXPECT_DOUBLE_EQ(average(grades), 90.0);
}

TEST(GradeUtilsTest, AverageEmptyVectorThrowsException) {
    std::vector<int> grades = {};
    EXPECT_THROW(average(grades), std::invalid_argument);
}

TEST(GradeUtilsTest, PassingGrade) {
    EXPECT_TRUE(is_passing(70));
}

TEST(GradeUtilsTest, FailingGrade) {
    EXPECT_FALSE(is_passing(69));
}

TEST(GradeUtilsTest, LetterGradeA) {
    EXPECT_EQ(letter_grade(95), 'A');
}

TEST(GradeUtilsTest, LetterGradeB) {
    EXPECT_EQ(letter_grade(85), 'B');
}

TEST(GradeUtilsTest, LetterGradeC) {
    EXPECT_EQ(letter_grade(75), 'C');
}

TEST(GradeUtilsTest, LetterGradeD) {
    EXPECT_EQ(letter_grade(65), 'D');
}

TEST(GradeUtilsTest, LetterGradeF) {
    EXPECT_EQ(letter_grade(50), 'F');
}

TEST(GradeUtilsTest, InvalidGradeThrowsException) {
    EXPECT_THROW(letter_grade(120), std::invalid_argument);
}

TEST(GradeUtilsTest, MinimumValidGrade) {
    EXPECT_EQ(letter_grade(0), 'F');
}

TEST(GradeUtilsTest, MaximumValidGrade) {
    EXPECT_EQ(letter_grade(100), 'A');
}

TEST(GradeUtilsTest, NegativeGradeThrowsException) {
    EXPECT_THROW(letter_grade(-1), std::invalid_argument);
}

TEST(GradeUtilsTest, GradeJustAboveMaximumThrowsException) {
    EXPECT_THROW(letter_grade(101), std::invalid_argument);
}

TEST(GradeUtilsTest, GradeBoundaryBetweenFAndD) {
    EXPECT_EQ(letter_grade(59), 'F');
    EXPECT_EQ(letter_grade(60), 'D');
}

TEST(GradeUtilsTest, GradeBoundaryBetweenDAndC) {
    EXPECT_EQ(letter_grade(69), 'D');
    EXPECT_EQ(letter_grade(70), 'C');
}

TEST(GradeUtilsTest, GradeBoundaryBetweenCAndB) {
    EXPECT_EQ(letter_grade(79), 'C');
    EXPECT_EQ(letter_grade(80), 'B');
}

TEST(GradeUtilsTest, GradeBoundaryBetweenBAndA) {
    EXPECT_EQ(letter_grade(89), 'B');
    EXPECT_EQ(letter_grade(90), 'A');
}

// Mini reto: pruebas de is_valid_grade

TEST(GradeUtilsTest, ValidGradeNormalValue) {
    EXPECT_TRUE(is_valid_grade(85));
}

TEST(GradeUtilsTest, InvalidGradeNormalValue) {
    // Valor claramente fuera del rango, no solo cerca de un borde
    EXPECT_FALSE(is_valid_grade(250));
}

TEST(GradeUtilsTest, ValidGradeLowerBound) {
    EXPECT_TRUE(is_valid_grade(0));
}

TEST(GradeUtilsTest, ValidGradeUpperBound) {
    EXPECT_TRUE(is_valid_grade(100));
}

TEST(GradeUtilsTest, GradeJustBelowLowerBoundIsInvalid) {
    EXPECT_FALSE(is_valid_grade(-1));
}

TEST(GradeUtilsTest, GradeJustAboveUpperBoundIsInvalid) {
    EXPECT_FALSE(is_valid_grade(101));
}
