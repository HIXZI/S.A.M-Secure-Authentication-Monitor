#ifndef PASSWORD_ANALYZER_H
#define PASSWORD_ANALYZER_H

#include <string>
#include <vector>
#include "DictionaryManager.h"

namespace SAM {

enum class StrengthLevel {
    WEAK,
    MODERATE,
    STRONG,
    VERY_STRONG
};

struct AnalysisReport {
    std::string candidatePassword;
    size_t length;
    size_t uppercaseCount;
    size_t lowercaseCount;
    size_t digitCount;
    size_t specialCharCount;
    size_t uniqueCharCount;
    
    double entropyBits;
    int score; // 0 to 100
    StrengthLevel level;
    
    bool isDictionaryMatch;
    std::vector<std::string> dictionarySubstrings;
    bool hasSequentialPattern;
    std::vector<std::string> sequentialPatternsFound;
    bool hasRepeatedPattern;
    std::vector<std::string> repeatedPatternsFound;

    std::vector<std::string> recommendations;

    std::string getStrengthString() const;
    std::string getStrengthColor() const;
    std::string getMaskedPassword() const;
};

class PasswordAnalyzer {
private:
    DictionaryManager dictionaryMgr;

    // Helper pattern detectors
    bool checkSequentialAlpha(const std::string& pwd, std::vector<std::string>& found) const;
    bool checkSequentialNumeric(const std::string& pwd, std::vector<std::string>& found) const;
    bool checkKeyboardSpatial(const std::string& pwd, std::vector<std::string>& found) const;
    bool checkRepeatedCharacters(const std::string& pwd, std::vector<std::string>& found) const;

    double calculateEntropy(size_t length, size_t poolSize) const;

public:
    PasswordAnalyzer();
    explicit PasswordAnalyzer(const DictionaryManager& dictManager);

    AnalysisReport analyze(const std::string& password) const;
    
    DictionaryManager& getDictionaryManager();
    const DictionaryManager& getDictionaryManager() const;
};

} // namespace SAM

#endif // PASSWORD_ANALYZER_H
