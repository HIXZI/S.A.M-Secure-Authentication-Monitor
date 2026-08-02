#ifndef SECURITY_EDUCATION_H
#define SECURITY_EDUCATION_H

#include <string>
#include <vector>

namespace SAM {

class SecurityEducation {
public:
    static void displayNistGuidelines();
    static void displayEntropyExplanation();
    static void displayAttackVectors();
    static void displayBestPractices();
    static void displayEducationMenu();
};

} // namespace SAM

#endif // SECURITY_EDUCATION_H
