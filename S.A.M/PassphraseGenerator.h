#ifndef PASSPHRASE_GENERATOR_H
#define PASSPHRASE_GENERATOR_H

#include <string>
#include <vector>

namespace SAM {

class PassphraseGenerator {
private:
    std::vector<std::string> wordList;

public:
    PassphraseGenerator();

    std::string generatePassphrase(size_t numWords = 4, bool includeNumber = true, bool includeSymbol = true) const;
    std::string generateComplexPassword(size_t length = 16, bool useUpper = true, bool useLower = true, bool useDigits = true, bool useSymbols = true) const;
};

} // namespace SAM

#endif // PASSPHRASE_GENERATOR_H
