#include "string_utils.h"
#include <cctype>
#include <algorithm>

std::string to_uppercase(const std::string& text) {
    std::string result = text;

    for (char& c : result) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    return result;
}

bool is_palindrome(const std::string& text) {
    std::string normalized;

    for (char c : text) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            normalized += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }

    std::string reversed = normalized;
    std::reverse(reversed.begin(), reversed.end());

    return normalized == reversed;
}

int count_vowels(const std::string& text) {
    int count = 0;

    for (char c : text) {
        char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
            count++;
        }
    }

    return count;
}
