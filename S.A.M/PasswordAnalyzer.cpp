#include "PasswordAnalyzer.h"
#include "TerminalUI.h"
#include <cmath>
#include <cctype>
#include <unordered_set>
#include <algorithm>

namespace SAM {

// AnalysisReport Helper Methods
std::string AnalysisReport::getStrengthString() const {
    switch (level) {
        case StrengthLevel::WEAK: return "WEAK";
        case StrengthLevel::MODERATE: return "MODERATE";
        case StrengthLevel::STRONG: return "STRONG";
        case StrengthLevel::VERY_STRONG: return "VERY STRONG";
        default: return "UNKNOWN";
    }
}

std::string AnalysisReport::getStrengthColor() const {
    switch (level) {
        case StrengthLevel::WEAK: return TerminalUI::BRIGHT_RED;
        case StrengthLevel::MODERATE: return TerminalUI::BRIGHT_YELLOW;
        case StrengthLevel::STRONG: return TerminalUI::GREEN;
        case StrengthLevel::VERY_STRONG: return TerminalUI::BRIGHT_GREEN;
        default: return TerminalUI::RESET;
    }
}

std::string AnalysisReport::getMaskedPassword() const {
    if (candidatePassword.empty()) return "";
    if (candidatePassword.length() <= 2) return std::string(candidatePassword.length(), '*');
    
    std::string masked = candidatePassword;
    for (size_t i = 1; i < masked.length() - 1; ++i) {
        masked[i] = '*';
    }
    return masked;
}

// PasswordAnalyzer Implementation
PasswordAnalyzer::PasswordAnalyzer() : dictionaryMgr() {}

PasswordAnalyzer::PasswordAnalyzer(const DictionaryManager& dictManager) 
    : dictionaryMgr(dictManager) {}

DictionaryManager& PasswordAnalyzer::getDictionaryManager() {
    return dictionaryMgr;
}

const DictionaryManager& PasswordAnalyzer::getDictionaryManager() const {
    return dictionaryMgr;
}

double PasswordAnalyzer::calculateEntropy(size_t length, size_t poolSize) const {
    if (length == 0 || poolSize == 0) return 0.0;
    return static_cast<double>(length) * (std::log2(static_cast<double>(poolSize)));
}

bool PasswordAnalyzer::checkSequentialAlpha(const std::string& pwd, std::vector<std::string>& found) const {
    if (pwd.length() < 3) return false;
    bool detected = false;
    std::string lowerPwd = pwd;
    std::transform(lowerPwd.begin(), lowerPwd.end(), lowerPwd.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (size_t i = 0; i <= lowerPwd.length() - 3; ++i) {
        if (std::isalpha(static_cast<unsigned char>(lowerPwd[i])) && 
            std::isalpha(static_cast<unsigned char>(lowerPwd[i+1])) && 
            std::isalpha(static_cast<unsigned char>(lowerPwd[i+2]))) {
            int diff1 = lowerPwd[i+1] - lowerPwd[i];
            int diff2 = lowerPwd[i+2] - lowerPwd[i+1];
            if ((diff1 == 1 && diff2 == 1) || (diff1 == -1 && diff2 == -1)) {
                found.push_back(pwd.substr(i, 3));
                detected = true;
            }
        }
    }
    return detected;
}

bool PasswordAnalyzer::checkSequentialNumeric(const std::string& pwd, std::vector<std::string>& found) const {
    if (pwd.length() < 3) return false;
    bool detected = false;
    for (size_t i = 0; i <= pwd.length() - 3; ++i) {
        if (std::isdigit(static_cast<unsigned char>(pwd[i])) && 
            std::isdigit(static_cast<unsigned char>(pwd[i+1])) && 
            std::isdigit(static_cast<unsigned char>(pwd[i+2]))) {
            int diff1 = pwd[i+1] - pwd[i];
            int diff2 = pwd[i+2] - pwd[i+1];
            if ((diff1 == 1 && diff2 == 1) || (diff1 == -1 && diff2 == -1)) {
                found.push_back(pwd.substr(i, 3));
                detected = true;
            }
        }
    }
    return detected;
}

bool PasswordAnalyzer::checkKeyboardSpatial(const std::string& pwd, std::vector<std::string>& found) const {
    static const std::vector<std::string> spatialRows = {
        "qwertyuiop", "asdfghjkl", "zxcvbnm",
        "1234567890"
    };

    std::string lowerPwd = pwd;
    std::transform(lowerPwd.begin(), lowerPwd.end(), lowerPwd.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    bool detected = false;
    for (const auto& row : spatialRows) {
        for (size_t len = 4; len >= 3; --len) {
            for (size_t i = 0; i <= row.length() - len; ++i) {
                std::string sub = row.substr(i, len);
                if (lowerPwd.find(sub) != std::string::npos) {
                    found.push_back(sub);
                    detected = true;
                }
                // Reverse check
                std::string revSub = sub;
                std::reverse(revSub.begin(), revSub.end());
                if (lowerPwd.find(revSub) != std::string::npos) {
                    found.push_back(revSub);
                    detected = true;
                }
            }
        }
    }
    return detected;
}

bool PasswordAnalyzer::checkRepeatedCharacters(const std::string& pwd, std::vector<std::string>& found) const {
    if (pwd.length() < 3) return false;
    bool detected = false;
    for (size_t i = 0; i <= pwd.length() - 3; ++i) {
        if (pwd[i] == pwd[i+1] && pwd[i+1] == pwd[i+2]) {
            found.push_back(pwd.substr(i, 3));
            detected = true;
        }
    }
    return detected;
}

AnalysisReport PasswordAnalyzer::analyze(const std::string& password) const {
    AnalysisReport report;
    report.candidatePassword = password;
    report.length = password.length();
    report.uppercaseCount = 0;
    report.lowercaseCount = 0;
    report.digitCount = 0;
    report.specialCharCount = 0;
    
    std::unordered_set<char> uniqueSet;
    for (char c : password) {
        uniqueSet.insert(c);
        if (std::isupper(static_cast<unsigned char>(c))) report.uppercaseCount++;
        else if (std::islower(static_cast<unsigned char>(c))) report.lowercaseCount++;
        else if (std::isdigit(static_cast<unsigned char>(c))) report.digitCount++;
        else report.specialCharCount++;
    }
    report.uniqueCharCount = uniqueSet.size();

    // Pool size for entropy
    size_t poolSize = 0;
    if (report.lowercaseCount > 0) poolSize += 26;
    if (report.uppercaseCount > 0) poolSize += 26;
    if (report.digitCount > 0) poolSize += 10;
    if (report.specialCharCount > 0) poolSize += 32;

    report.entropyBits = calculateEntropy(report.length, poolSize);

    // Scoring calculation (0 - 100)
    int score = 0;

    // 1. Length scoring (Max 35 points)
    if (report.length >= 16) score += 35;
    else if (report.length >= 12) score += 28;
    else if (report.length >= 8) score += 18;
    else score += static_cast<int>(report.length) * 2;

    // 2. Character diversity (Max 45 points)
    if (report.lowercaseCount > 0) score += 10;
    if (report.uppercaseCount > 0) score += 10;
    if (report.digitCount > 0) score += 10;
    if (report.specialCharCount > 0) score += 15;

    // Bonus for using all 4 types
    if (report.lowercaseCount > 0 && report.uppercaseCount > 0 && 
        report.digitCount > 0 && report.specialCharCount > 0) {
        score += 10;
    }

    // Uniqueness bonus (Max 10 points)
    if (report.length > 0) {
        double ratio = static_cast<double>(report.uniqueCharCount) / static_cast<double>(report.length);
        score += static_cast<int>(ratio * 10.0);
    }

    // 3. Vulnerability Checks & Deductions
    report.isDictionaryMatch = dictionaryMgr.isExactMatch(password);
    if (report.isDictionaryMatch) {
        score -= 50;
    }

    report.dictionarySubstrings = dictionaryMgr.findSubstringMatches(password);
    
    // Prune dictionary substring matches
    if (!report.dictionarySubstrings.empty()) {
        std::sort(report.dictionarySubstrings.begin(), report.dictionarySubstrings.end(), 
                  [](const std::string& a, const std::string& b) {
                      if (a.length() != b.length()) return a.length() > b.length();
                      return a < b;
                  });
        std::vector<std::string> prunedDict;
        for (const auto& item : report.dictionarySubstrings) {
            bool isSub = false;
            for (const auto& existing : prunedDict) {
                if (existing.find(item) != std::string::npos) {
                    isSub = true;
                    break;
                }
            }
            if (!isSub) prunedDict.push_back(item);
        }
        report.dictionarySubstrings = prunedDict;
        score -= static_cast<int>(report.dictionarySubstrings.size()) * 15;
    }

    std::vector<std::string> seqAlpha, seqNum, seqKey, repeated;
    bool hasAlpha = checkSequentialAlpha(password, seqAlpha);
    bool hasNum = checkSequentialNumeric(password, seqNum);
    bool hasKey = checkKeyboardSpatial(password, seqKey);
    bool hasRep = checkRepeatedCharacters(password, repeated);

    report.hasSequentialPattern = (hasAlpha || hasNum || hasKey);
    report.sequentialPatternsFound.insert(report.sequentialPatternsFound.end(), seqAlpha.begin(), seqAlpha.end());
    report.sequentialPatternsFound.insert(report.sequentialPatternsFound.end(), seqNum.begin(), seqNum.end());
    report.sequentialPatternsFound.insert(report.sequentialPatternsFound.end(), seqKey.begin(), seqKey.end());

    // Prune redundant sub-patterns (e.g. keep 'ghjk' and remove 'ghj'/'hjk')
    if (!report.sequentialPatternsFound.empty()) {
        std::sort(report.sequentialPatternsFound.begin(), report.sequentialPatternsFound.end(), 
                  [](const std::string& a, const std::string& b) {
                      if (a.length() != b.length()) return a.length() > b.length();
                      return a < b;
                  });
        std::vector<std::string> prunedSeq;
        for (const auto& item : report.sequentialPatternsFound) {
            bool isSub = false;
            for (const auto& existing : prunedSeq) {
                if (existing.find(item) != std::string::npos) {
                    isSub = true;
                    break;
                }
            }
            if (!isSub) prunedSeq.push_back(item);
        }
        report.sequentialPatternsFound = prunedSeq;
    }

    if (report.hasSequentialPattern) {
        score -= 20;
    }

    report.hasRepeatedPattern = hasRep;
    report.repeatedPatternsFound = repeated;
    if (!report.repeatedPatternsFound.empty()) {
        std::sort(report.repeatedPatternsFound.begin(), report.repeatedPatternsFound.end(), 
                  [](const std::string& a, const std::string& b) {
                      if (a.length() != b.length()) return a.length() > b.length();
                      return a < b;
                  });
        std::vector<std::string> prunedRep;
        for (const auto& item : report.repeatedPatternsFound) {
            bool isSub = false;
            for (const auto& existing : prunedRep) {
                if (existing.find(item) != std::string::npos) {
                    isSub = true;
                    break;
                }
            }
            if (!isSub) prunedRep.push_back(item);
        }
        report.repeatedPatternsFound = prunedRep;
    }
    if (report.hasRepeatedPattern) {
        score -= 15;
    }

    if (report.length < 8) {
        score -= 20;
    }

    // Clamp score between 0 and 100
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    report.score = score;

    // Classify Strength Level
    if (report.score >= 85) report.level = StrengthLevel::VERY_STRONG;
    else if (report.score >= 65) report.level = StrengthLevel::STRONG;
    else if (report.score >= 40) report.level = StrengthLevel::MODERATE;
    else report.level = StrengthLevel::WEAK;

    // Generate Tailored Security Recommendations
    if (report.length < 12) {
        report.recommendations.push_back("Increase length: Aim for at least 12 to 16 characters for robust protection.");
    }
    if (report.uppercaseCount == 0) {
        report.recommendations.push_back("Add uppercase letters (A-Z) to increase character pool size.");
    }
    if (report.lowercaseCount == 0) {
        report.recommendations.push_back("Add lowercase letters (a-z) to diversify character set.");
    }
    if (report.digitCount == 0) {
        report.recommendations.push_back("Include numeric digits (0-9) to raise password entropy.");
    }
    if (report.specialCharCount == 0) {
        report.recommendations.push_back("Include special symbols (!@#$%^&*) to guard against dictionary attacks.");
    }
    if (report.isDictionaryMatch) {
        report.recommendations.push_back("CRITICAL: Password exists in common weak password dictionaries. Change immediately!");
    }
    if (!report.dictionarySubstrings.empty()) {
        report.recommendations.push_back("Avoid embeding common words or dictionary sub-phrases into your password.");
    }
    if (report.hasSequentialPattern) {
        report.recommendations.push_back("Remove sequential patterns (e.g. '123', 'abc', 'qwerty') as automated tools guess them first.");
    }
    if (report.hasRepeatedPattern) {
        report.recommendations.push_back("Avoid repeating identical consecutive characters (e.g. 'aaa', '111').");
    }
    if (report.recommendations.empty()) {
        report.recommendations.push_back("Excellent password design! Ensure you use a unique password for each online service.");
    }

    return report;
}

} // namespace SAM
