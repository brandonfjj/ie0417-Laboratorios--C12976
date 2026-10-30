#include "grade_utils.h"
#include <stdexcept>

double average(const std::vector<int>& grades) {
    if (grades.empty()) {
        throw std::invalid_argument("La lista de notas no puede estar vacía");
    }

    int sum = 0;

    for (int grade : grades) {
        sum += grade;
    }

    return static_cast<double>(sum) / grades.size();
}

bool is_passing(int grade) {
    return grade >= 70;
}

char letter_grade(int grade) {
    if (grade < 0 || grade > 100) {
        throw std::invalid_argument("La nota debe estar entre 0 y 100");
    }

    if (grade >= 90) {
        return 'A';
    }

    if (grade >= 80) {
        return 'B';
    }

    if (grade >= 70) {
        return 'C';
    }

    if (grade >= 60) {
        return 'D';
    }

    return 'F';
}
