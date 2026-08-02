#include "SecurityEducation.h"
#include "TerminalUI.h"
#include <iostream>

namespace SAM {

void SecurityEducation::displayNistGuidelines() {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("NIST SP 800-63B SECURITY STANDARDS");

    std::vector<std::string> lines = {
        "1. LENGTH OVER COMPLEXITY:",
        "   - Modern standards prioritize length (12-64+ characters) rather than",
        "     forced arbitrary symbol requirements.",
        "",
        "2. ELIMINATE MANDATORY PERIODIC EXPIRATION:",
        "   - Forcing users to change passwords every 30-90 days leads to predictable",
        "     pattern updates (e.g. Winter2024! -> Spring2024!).",
        "",
        "3. DICTIONARY CHECKING AT REGISTRATION:",
        "   - Verifier systems must block passwords appearing in known breach databases,",
        "     dictionary words, repetitive characters, and context-specific terms.",
        "",
        "4. MULTI-FACTOR AUTHENTICATION (MFA):",
        "   - Combine passwords with an authenticator app (TOTP) or FIDO2 hardware key.",
        "   - MFA renders 99% of passive credential theft attacks ineffective."
    };
    TerminalUI::printBox("NIST Guidelines Summary", lines, TerminalUI::BRIGHT_CYAN);
    TerminalUI::pause();
}

void SecurityEducation::displayEntropyExplanation() {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("UNDERSTANDING PASSWORD ENTROPY");

    std::vector<std::string> lines = {
        "WHAT IS ENTROPY?",
        "   - Entropy measures the unpredictable randomness of a password in bits.",
        "   - Formula: E = L * log2(R)",
        "     L = Password Length | R = Size of Character Pool",
        "",
        "ENTROPY THRESHOLDS & RESISTANCE:",
        "   - < 28 Bits  : VERY WEAK. Cracked instantly by automated scripts.",
        "   - 28-35 Bits : WEAK. Cracked in minutes using GPU hash clusters.",
        "   - 36-59 Bits : MODERATE. Vulnerable to offline dictionary/rule attacks.",
        "   - 60-79 Bits : STRONG. Takes decades to crack via brute force.",
        "   - 80+ Bits   : VERY STRONG. Mathematically unfeasible to brute force.",
        "",
        "PASSPHRASE ADVANTAGE:",
        "   - A 4-word passphrase ('correct-horse-battery-staple') provides ~77 bits",
        "     of entropy while being vastly easier for humans to remember!"
    };
    TerminalUI::printBox("Entropy & Mathematical Protection", lines, TerminalUI::BRIGHT_YELLOW);
    TerminalUI::pause();
}

void SecurityEducation::displayAttackVectors() {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("COMMON CREDENTIAL ATTACK VECTORS");

    std::vector<std::string> lines = {
        "1. BRUTE FORCE ATTACKS:",
        "   - Trying every possible character combination until the right one is found.",
        "",
        "2. DICTIONARY ATTACKS:",
        "   - Using automated lists of millions of leaked & common words/passwords.",
        "",
        "3. CREDENTIAL STUFFING:",
        "   - Automated bots testing stolen username/password pairs across hundreds of websites.",
        "",
        "4. RAINBOW TABLE ATTACKS:",
        "   - Precomputed tables of cryptographic hashes used to reverse unsalted password hashes.",
        "",
        "5. SOCIAL ENGINEERING & PHISHING:",
        "   - Tricking users into revealing credentials via fake login portals or emails."
    };
    TerminalUI::printBox("Threat Matrix Overview", lines, TerminalUI::BRIGHT_RED);
    TerminalUI::pause();
}

void SecurityEducation::displayBestPractices() {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("SECURITY BEST PRACTICES & ACTION PLAN");

    std::vector<std::string> lines = {
        "DO'S:",
        "  [+] Use a zero-knowledge Password Manager (e.g. Bitwarden, 1Password).",
        "  [+] Use unique, long passwords (16+ chars) or 4-word random passphrases for every site.",
        "  [+] Enable Hardware/App Multi-Factor Authentication (2FA) on all critical accounts.",
        "  [+] Check HaveIBeenPwned.com to verify if your email/passwords were leaked in data breaches.",
        "",
        "DONT'S:",
        "  [-] Never reuse the same password across multiple accounts or services.",
        "  [-] Never write passwords in plaintext files, notes, or sticky notes.",
        "  [-] Never share passwords via email, SMS, or unencrypted messaging apps.",
        "  [-] Avoid simple letter-to-number substitutions (e.g. P@ssw0rd1) - bots easily bypass them."
    };
    TerminalUI::printBox("Actionable Recommendations", lines, TerminalUI::GREEN);
    TerminalUI::pause();
}

void SecurityEducation::displayEducationMenu() {
    while (true) {
        TerminalUI::clearScreen();
        TerminalUI::printBanner();
        TerminalUI::printHeader("MODULE 3: SECURITY EDUCATION & AWARENESS CENTER");

        std::cout << TerminalUI::BRIGHT_CYAN << "  Select an educational topic to review:\n\n" << TerminalUI::RESET;
        std::cout << TerminalUI::CYAN << "  [1] " << TerminalUI::BOLD << "NIST SP 800-63B Modern Password Standards\n" << TerminalUI::RESET;
        std::cout << TerminalUI::CYAN << "  [2] " << TerminalUI::BOLD << "Understanding Password Entropy & Mathematics\n" << TerminalUI::RESET;
        std::cout << TerminalUI::CYAN << "  [3] " << TerminalUI::BOLD << "Common Attack Vectors (Brute Force, Dictionary, Stuffing)\n" << TerminalUI::RESET;
        std::cout << TerminalUI::CYAN << "  [4] " << TerminalUI::BOLD << "Security Best Practices & Actionable Recommendations\n" << TerminalUI::RESET;
        std::cout << TerminalUI::CYAN << "  [5] " << TerminalUI::BOLD << "Return to Main Menu\n\n" << TerminalUI::RESET;

        int choice = TerminalUI::getIntInput("  Enter choice (1-5): ", 1, 5);
        if (choice == 1) displayNistGuidelines();
        else if (choice == 2) displayEntropyExplanation();
        else if (choice == 3) displayAttackVectors();
        else if (choice == 4) displayBestPractices();
        else if (choice == 5) break;
    }
}

} // namespace SAM
