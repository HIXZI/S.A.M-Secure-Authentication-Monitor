#include "PassphraseGenerator.h"
#include <random>
#include <algorithm>

namespace SAM {

PassphraseGenerator::PassphraseGenerator() {
    wordList = {
        "quantum", "cipher", "matrix", "falcon", "orbit",
        "shield", "crypto", "phoenix", "anchor", "beacon",
        "cobalt", "dragon", "echo", "forest", "glacier",
        "horizon", "island", "jungle", "knight", "lunar",
        "magnet", "nebula", "ocean", "pioneer", "quasar",
        "river", "shadow", "timber", "uptime", "vector",
        "walnut", "xenon", "yellow", "zenith", "aurora",
        "blazer", "canyon", "drift", "ember", "frost"
    };
}

std::string PassphraseGenerator::generatePassphrase(size_t numWords, bool includeNumber, bool includeSymbol) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> wordDist(0, wordList.size() - 1);
    std::uniform_int_distribution<int> numDist(10, 99);

    static const std::string symbols = "!@#$%^&*_-+=?";
    std::uniform_int_distribution<size_t> symDist(0, symbols.length() - 1);

    std::string passphrase = "";
    for (size_t i = 0; i < numWords; ++i) {
        passphrase += wordList[wordDist(gen)];
        if (i < numWords - 1) {
            passphrase += "-";
        }
    }

    if (includeNumber) {
        passphrase += "-" + std::to_string(numDist(gen));
    }
    if (includeSymbol) {
        passphrase += symbols[symDist(gen)];
    }

    return passphrase;
}

std::string PassphraseGenerator::generateComplexPassword(size_t length, bool useUpper, bool useLower, bool useDigits, bool useSymbols) const {
    if (length < 8) length = 8;
    
    std::string upperStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string lowerStr = "abcdefghijklmnopqrstuvwxyz";
    std::string digitStr = "0123456789";
    std::string symbolStr = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    std::string pool = "";
    std::string required = "";

    std::random_device rd;
    std::mt19937 gen(rd());

    if (useUpper) {
        pool += upperStr;
        std::uniform_int_distribution<size_t> d(0, upperStr.length() - 1);
        required += upperStr[d(gen)];
    }
    if (useLower) {
        pool += lowerStr;
        std::uniform_int_distribution<size_t> d(0, lowerStr.length() - 1);
        required += lowerStr[d(gen)];
    }
    if (useDigits) {
        pool += digitStr;
        std::uniform_int_distribution<size_t> d(0, digitStr.length() - 1);
        required += digitStr[d(gen)];
    }
    if (useSymbols) {
        pool += symbolStr;
        std::uniform_int_distribution<size_t> d(0, symbolStr.length() - 1);
        required += symbolStr[d(gen)];
    }

    if (pool.empty()) {
        pool = lowerStr + digitStr;
    }

    std::string password = required;
    std::uniform_int_distribution<size_t> poolDist(0, pool.length() - 1);
    while (password.length() < length) {
        password += pool[poolDist(gen)];
    }

    // Shuffle characters to avoid predictable required character positions
    std::shuffle(password.begin(), password.end(), gen);

    return password;
}

} // namespace SAM
