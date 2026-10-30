#ifndef GRADE_UTILS_H
#define GRADE_UTILS_H

#include <vector>

double average(const std::vector<int>& grades);
bool is_passing(int grade);
char letter_grade(int grade);
bool is_valid_grade(int grade);

#endif
