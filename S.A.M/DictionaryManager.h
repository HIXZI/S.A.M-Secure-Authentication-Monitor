#ifndef DICTIONARY_MANAGER_H
#define DICTIONARY_MANAGER_H

#include <string>
#include <vector>
#include <unordered_set>

namespace SAM {

class DictionaryManager {
private:
    std::vector<std::string> weakPasswords;
    std::unordered_set<std::string> dictionarySet;
    std::string dictionaryFilePath;

    void initializeDefaultDictionary();
    std::string toLower(const std::string& str) const;

public:
    explicit DictionaryManager(const std::string& filePath = "rockyou.txt");

    bool loadDictionary();
    bool saveDictionary();
    bool addPasswordToDictionary(const std::string& newPassword);
    
    bool isExactMatch(const std::string& password) const;
    std::vector<std::string> findSubstringMatches(const std::string& password) const;

    // Getters & Setters
    const std::vector<std::string>& getWeakPasswords() const;
    size_t getDictionarySize() const;
    const std::string& getFilePath() const;
};

} // namespace SAM

#endif // DICTIONARY_MANAGER_H
