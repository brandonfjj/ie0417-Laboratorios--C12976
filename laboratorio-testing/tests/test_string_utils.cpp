#include <gtest/gtest.h>
#include "string_utils.h"

TEST(StringUtilsTest, ConvertTextToUppercase) {
    EXPECT_EQ(to_uppercase("hola"), "HOLA");
}

TEST(StringUtilsTest, ConvertMixedTextToUppercase) {
    EXPECT_EQ(to_uppercase("HoLa123"), "HOLA123");
}

TEST(StringUtilsTest, DetectSimplePalindrome) {
    EXPECT_TRUE(is_palindrome("radar"));
}

TEST(StringUtilsTest, DetectPalindromeWithSpaces) {
    EXPECT_TRUE(is_palindrome("anita lava la tina"));
}

TEST(StringUtilsTest, DetectNonPalindrome) {
    EXPECT_FALSE(is_palindrome("software"));
}

TEST(StringUtilsTest, CountVowels) {
    EXPECT_EQ(count_vowels("murcielago"), 5);
}

TEST(StringUtilsTest, CountVowelsInTextWithoutVowels) {
    EXPECT_EQ(count_vowels("xyz"), 0);
}

TEST(StringUtilsTest, EmptyStringToUppercase) {
    EXPECT_EQ(to_uppercase(""), "");
}

TEST(StringUtilsTest, EmptyStringIsPalindrome) {
    EXPECT_TRUE(is_palindrome(""));
}

TEST(StringUtilsTest, SingleLetterIsPalindrome) {
    EXPECT_TRUE(is_palindrome("a"));
}
