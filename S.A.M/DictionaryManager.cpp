#include "DictionaryManager.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace SAM {

DictionaryManager::DictionaryManager(const std::string& filePath) 
    : dictionaryFilePath(filePath) {
    if (!loadDictionary()) {
        initializeDefaultDictionary();
        saveDictionary();
    }
}

std::string DictionaryManager::toLower(const std::string& str) const {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

void DictionaryManager::initializeDefaultDictionary() {
    weakPasswords = {
        "password", "123456", "12345678", "123456789", "12345",
        "qwerty", "password123", "admin", "welcome", "letmein",
        "iloveyou", "sunshine", "dragon", "football", "monkey",
        "111111", "master", "princess", "shadow", "superman",
        "pass123", "abc123", "secret", "login", "trustno1",
        "p@ssword", "administrator", "guest", "passcode", "baseball"
    };

    std::sort(weakPasswords.begin(), weakPasswords.end(), [this](const std::string& a, const std::string& b) {
        std::string lowerA = toLower(a);
        std::string lowerB = toLower(b);
        if (lowerA != lowerB) return lowerA < lowerB;
        return a < b;
    });

    dictionarySet.clear();
    for (const auto& pwd : weakPasswords) {
        dictionarySet.insert(toLower(pwd));
    }
}

bool DictionaryManager::loadDictionary() {
    std::ifstream file(dictionaryFilePath);
    if (!file.is_open()) {
        return false;
    }

    weakPasswords.clear();
    dictionarySet.clear();
    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (!line.empty()) {
            weakPasswords.push_back(line);
            dictionarySet.insert(toLower(line));
        }
    }
    file.close();

    // Sort entries in ascending case-insensitive alphanumeric order
    std::sort(weakPasswords.begin(), weakPasswords.end(), [this](const std::string& a, const std::string& b) {
        std::string lowerA = toLower(a);
        std::string lowerB = toLower(b);
        if (lowerA != lowerB) {
            return lowerA < lowerB;
        }
        return a < b;
    });

    return !weakPasswords.empty();
}

bool DictionaryManager::saveDictionary() {
    std::ofstream file(dictionaryFilePath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& pwd : weakPasswords) {
        file << pwd << "\n";
    }
    file.close();
    return true;
}

bool DictionaryManager::addPasswordToDictionary(const std::string& newPassword) {
    std::string lowerPwd = toLower(newPassword);
    if (dictionarySet.find(lowerPwd) != dictionarySet.end()) {
        return false; // Already present
    }

    weakPasswords.push_back(newPassword);
    dictionarySet.insert(lowerPwd);
    
    std::sort(weakPasswords.begin(), weakPasswords.end(), [this](const std::string& a, const std::string& b) {
        std::string lowerA = toLower(a);
        std::string lowerB = toLower(b);
        if (lowerA != lowerB) {
            return lowerA < lowerB;
        }
        return a < b;
    });

    saveDictionary();
    return true;
}

bool DictionaryManager::isExactMatch(const std::string& password) const {
    std::string lowerInput = toLower(password);
    return dictionarySet.find(lowerInput) != dictionarySet.end();
}

std::vector<std::string> DictionaryManager::findSubstringMatches(const std::string& password) const {
    std::string lowerInput = toLower(password);
    std::vector<std::string> matches;

    for (const auto& dictWord : weakPasswords) {
        std::string lowerDict = toLower(dictWord);
        // Only check words of length >= 3 to avoid false positive tiny substrings
        if (lowerDict.length() >= 3 && lowerInput.find(lowerDict) != std::string::npos) {
            matches.push_back(dictWord);
        }
    }
    return matches;
}

const std::vector<std::string>& DictionaryManager::getWeakPasswords() const {
    return weakPasswords;
}

size_t DictionaryManager::getDictionarySize() const {
    return weakPasswords.size();
}

const std::string& DictionaryManager::getFilePath() const {
    return dictionaryFilePath;
}

} // namespace SAM
