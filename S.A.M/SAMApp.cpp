#include "SAMApp.h"
#include "SecurityEducation.h"
#include "TerminalUI.h"
#include <iomanip>
#include <iostream>

namespace SAM {

SAMApp::SAMApp() : analyzer(), generator(), logger("analysis_report.txt") {}

void SAMApp::displayReportUI(const AnalysisReport &report) const {
  TerminalUI::printHeader("EVALUATION RESULT & SECURITY BREAKDOWN");

  std::string color = report.getStrengthColor();

  std::cout << TerminalUI::BRIGHT_CYAN
            << "  Candidate Password (Masked) : " << TerminalUI::RESET
            << report.getMaskedPassword() << "\n";
  std::cout << TerminalUI::BRIGHT_CYAN
            << "  Strength Classification    : " << color << TerminalUI::BOLD
            << report.getStrengthString() << TerminalUI::RESET << "\n";
  std::cout << TerminalUI::BRIGHT_CYAN
            << "  Overall Security Score     : " << color << TerminalUI::BOLD
            << report.score << " / 100" << TerminalUI::RESET << "\n";
  std::cout << TerminalUI::BRIGHT_CYAN
            << "  Calculated Entropy         : " << TerminalUI::BRIGHT_YELLOW
            << std::fixed << std::setprecision(2) << report.entropyBits
            << " bits" << TerminalUI::RESET << "\n";

  TerminalUI::printDivider('-', 75, TerminalUI::CYAN);

  std::cout << TerminalUI::BOLD << "  CHARACTER METRICS & DISTRIBUTION:\n"
            << TerminalUI::RESET;
  TerminalUI::printStatus(
      "Total Length", std::to_string(report.length) + " characters",
      (report.length >= 12 ? TerminalUI::GREEN : TerminalUI::YELLOW));
  TerminalUI::printStatus(
      "Uppercase Letters (A-Z)", std::to_string(report.uppercaseCount),
      (report.uppercaseCount > 0 ? TerminalUI::GREEN : TerminalUI::RED));
  TerminalUI::printStatus(
      "Lowercase Letters (a-z)", std::to_string(report.lowercaseCount),
      (report.lowercaseCount > 0 ? TerminalUI::GREEN : TerminalUI::RED));
  TerminalUI::printStatus(
      "Numeric Digits (0-9)", std::to_string(report.digitCount),
      (report.digitCount > 0 ? TerminalUI::GREEN : TerminalUI::RED));
  TerminalUI::printStatus(
      "Special Symbols (!@#$)", std::to_string(report.specialCharCount),
      (report.specialCharCount > 0 ? TerminalUI::GREEN : TerminalUI::RED));
  TerminalUI::printStatus("Unique Characters",
                          std::to_string(report.uniqueCharCount),
                          TerminalUI::BRIGHT_CYAN);

  TerminalUI::printDivider('-', 75, TerminalUI::CYAN);

  std::cout << TerminalUI::BOLD << "  VULNERABILITY ANALYSIS:\n"
            << TerminalUI::RESET;
  TerminalUI::printStatus(
      "Dictionary Match",
      (report.isDictionaryMatch ? "MATCH FOUND (CRITICAL VULNERABILITY)"
                                : "CLEAN (Not in weak dictionary)"),
      (report.isDictionaryMatch ? TerminalUI::BRIGHT_RED : TerminalUI::GREEN));

  if (!report.dictionarySubstrings.empty()) {
    std::string subStr = "";
    for (size_t i = 0; i < report.dictionarySubstrings.size(); ++i) {
      subStr += "'" + report.dictionarySubstrings[i] + "'" +
                (i < report.dictionarySubstrings.size() - 1 ? ", " : "");
    }
    TerminalUI::printStatus("Embedded Dictionary Substrings", subStr,
                            TerminalUI::YELLOW);
  }

  TerminalUI::printStatus(
      "Sequential Patterns",
      (report.hasSequentialPattern ? "DETECTED" : "CLEAN"),
      (report.hasSequentialPattern ? TerminalUI::YELLOW : TerminalUI::GREEN));
  if (report.hasSequentialPattern) {
    std::string seqStr = "";
    for (size_t i = 0; i < report.sequentialPatternsFound.size(); ++i) {
      seqStr += "'" + report.sequentialPatternsFound[i] + "'" +
                (i < report.sequentialPatternsFound.size() - 1 ? ", " : "");
    }
    TerminalUI::printStatus("Found Sequences", seqStr,
                            TerminalUI::BRIGHT_YELLOW);
  }

  TerminalUI::printStatus(
      "Repeated Characters", (report.hasRepeatedPattern ? "DETECTED" : "CLEAN"),
      (report.hasRepeatedPattern ? TerminalUI::YELLOW : TerminalUI::GREEN));
  if (report.hasRepeatedPattern) {
    std::string repStr = "";
    for (size_t i = 0; i < report.repeatedPatternsFound.size(); ++i) {
      repStr += "'" + report.repeatedPatternsFound[i] + "'" +
                (i < report.repeatedPatternsFound.size() - 1 ? ", " : "");
    }
    TerminalUI::printStatus("Found Repeats", repStr, TerminalUI::BRIGHT_YELLOW);
  }

  TerminalUI::printDivider('-', 75, TerminalUI::CYAN);

  std::cout << TerminalUI::BOLD << TerminalUI::BRIGHT_YELLOW
            << "  SECURITY RECOMMENDATIONS & TIPS:\n"
            << TerminalUI::RESET;
  for (const auto &rec : report.recommendations) {
    std::cout << TerminalUI::YELLOW << "   * " << rec << TerminalUI::RESET
              << "\n";
  }
  TerminalUI::printDivider('-', 75, TerminalUI::CYAN);
}

void SAMApp::handleEvaluatePassword() {
  TerminalUI::clearScreen();
  TerminalUI::printBanner();
  TerminalUI::printHeader("MODULE 1: PASSWORD STRENGTH EVALUATOR");

  std::string pwd =
      TerminalUI::getStringInput("  Enter candidate password to evaluate: ");

  AnalysisReport report = analyzer.analyze(pwd);

  TerminalUI::clearScreen();
  TerminalUI::printBanner();
  displayReportUI(report);

  std::cout << "\n";
  std::string saveChoice = TerminalUI::getStringInput(
      "  Log this evaluation report to analysis_report.txt? (y/n): ");
  if (saveChoice == "y" || saveChoice == "Y") {
    if (logger.logReport(report)) {
      TerminalUI::printSuccess("Report logged successfully to " +
                               logger.getFilePath());
    } else {
      TerminalUI::printError("Failed to write report to disk!");
    }
  }

  TerminalUI::pause();
}

void SAMApp::handleVulnerabilityChecker() {
  TerminalUI::clearScreen();
  TerminalUI::printBanner();
  TerminalUI::printHeader("MODULE 2: COMMON WEAKNESS & DICTIONARY CHECKER");

  std::cout << TerminalUI::CYAN
            << "  Cross-referencing input against dictionary ("
            << analyzer.getDictionaryManager().getDictionarySize()
            << " entries) and pattern engine...\n\n"
            << TerminalUI::RESET;

  std::string pwd =
      TerminalUI::getStringInput("  Enter password to audit for weaknesses: ");
  AnalysisReport report = analyzer.analyze(pwd);

  TerminalUI::clearScreen();
  TerminalUI::printBanner();
  TerminalUI::printHeader("DICTIONARY & PATTERN AUDIT RESULTS");

  if (report.isDictionaryMatch) {
    TerminalUI::printError(
        "CRITICAL: Password matches a known weak password in dictionary!");
  } else {
    TerminalUI::printSuccess(
        "PASS: Password is not an exact match for any dictionary entry.");
  }

  if (!report.dictionarySubstrings.empty()) {
    TerminalUI::printWarning(
        "WARNING: Password contains weak dictionary words inside it!");
    for (const auto &sub : report.dictionarySubstrings) {
      std::cout << TerminalUI::YELLOW << "   -> Matched substring: '" << sub
                << "'\n"
                << TerminalUI::RESET;
    }
  } else {
    TerminalUI::printSuccess("PASS: No weak dictionary sub-words detected.");
  }

  if (report.hasSequentialPattern) {
    TerminalUI::printWarning(
        "WARNING: Sequential keyboard/alphabet/number patterns detected!");
    for (const auto &seq : report.sequentialPatternsFound) {
      std::cout << TerminalUI::YELLOW << "   -> Pattern: '" << seq << "'\n"
                << TerminalUI::RESET;
    }
  } else {
    TerminalUI::printSuccess("PASS: No sequential patterns detected.");
  }

  if (report.hasRepeatedPattern) {
    TerminalUI::printWarning(
        "WARNING: Consecutive repeated character sequences detected!");
    for (const auto &rep : report.repeatedPatternsFound) {
      std::cout << TerminalUI::YELLOW << "   -> Repeat: '" << rep << "'\n"
                << TerminalUI::RESET;
    }
  } else {
    TerminalUI::printSuccess("PASS: No repeated character sequences detected.");
  }

  TerminalUI::pause();
}

void SAMApp::handleSecurityEducationAndGenerator() {
  while (true) {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("MODULE 3: SECURITY EDUCATION & GENERATOR CENTER");

    std::cout << TerminalUI::CYAN << "  [1] " << TerminalUI::BOLD
              << "Interactive Security Education & Standards\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [2] " << TerminalUI::BOLD
              << "Generate Strong 4-Word Passphrase\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [3] " << TerminalUI::BOLD
              << "Generate High-Entropy Complex Password\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [4] " << TerminalUI::BOLD
              << "Return to Main Menu\n\n"
              << TerminalUI::RESET;

    int choice = TerminalUI::getIntInput("  Enter selection (1-4): ", 1, 4);

    if (choice == 1) {
      SecurityEducation::displayEducationMenu();
    } else if (choice == 2) {
      std::string passphrase = generator.generatePassphrase(4, true, true);
      AnalysisReport report = analyzer.analyze(passphrase);

      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printHeader("GENERATED PASSPHRASE");

      std::cout << TerminalUI::BRIGHT_GREEN << TerminalUI::BOLD
                << "  Generated Passphrase : " << passphrase
                << TerminalUI::RESET << "\n";
      std::cout << TerminalUI::BRIGHT_CYAN
                << "  Calculated Entropy   : " << report.entropyBits
                << " bits (" << report.getStrengthString() << ")\n"
                << TerminalUI::RESET;
      std::cout << TerminalUI::CYAN
                << "  Score                : " << report.score << " / 100\n\n"
                << TerminalUI::RESET;
      TerminalUI::printInfo(
          "Passphrases combine high entropy with easy human memorability.");
      TerminalUI::pause();
    } else if (choice == 3) {
      int len = TerminalUI::getIntInput(
          "  Enter desired password length (8-64, default 16): ", 8, 64);
      std::string complexPwd =
          generator.generateComplexPassword(len, true, true, true, true);
      AnalysisReport report = analyzer.analyze(complexPwd);

      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printHeader("GENERATED COMPLEX PASSWORD");

      std::cout << TerminalUI::BRIGHT_GREEN << TerminalUI::BOLD
                << "  Generated Password : " << complexPwd << TerminalUI::RESET
                << "\n";
      std::cout << TerminalUI::BRIGHT_CYAN
                << "  Calculated Entropy : " << report.entropyBits << " bits ("
                << report.getStrengthString() << ")\n"
                << TerminalUI::RESET;
      std::cout << TerminalUI::CYAN << "  Score              : " << report.score
                << " / 100\n\n"
                << TerminalUI::RESET;
      TerminalUI::pause();
    } else if (choice == 4) {
      break;
    }
  }
}

void SAMApp::handleAuditReports() {
  while (true) {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("MODULE 4: AUDIT REPORTING & HISTORY LOGGING");

    std::cout << TerminalUI::CYAN << "  [1] " << TerminalUI::BOLD
              << "View Evaluation History Summary\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [2] " << TerminalUI::BOLD
              << "View Raw Log File (" << logger.getFilePath() << ")\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [3] " << TerminalUI::BOLD
              << "Clear Audit Log History\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [4] " << TerminalUI::BOLD
              << "Return to Main Menu\n\n"
              << TerminalUI::RESET;

    int choice = TerminalUI::getIntInput("  Enter choice (1-4): ", 1, 4);

    if (choice == 1) {
      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printHeader("AUDIT HISTORY SUMMARY");

      auto history = logger.loadAuditHistory();
      if (history.empty()) {
        TerminalUI::printInfo("No evaluation history logged yet.");
      } else {
        std::cout << TerminalUI::CYAN << TerminalUI::BOLD << std::left
                  << std::setw(22) << "TIMESTAMP" << std::setw(18)
                  << "MASKED PWD" << std::setw(15) << "RATING" << std::setw(10)
                  << "SCORE" << std::setw(12) << "ENTROPY" << TerminalUI::RESET
                  << "\n";
        TerminalUI::printDivider('-', 75, TerminalUI::CYAN);

        for (const auto &entry : history) {
          std::cout << std::left << std::setw(22) << entry.timestamp
                    << std::setw(18) << entry.maskedPassword << std::setw(15)
                    << entry.strengthRating << std::setw(10)
                    << (std::to_string(entry.score) + "/100") << std::fixed
                    << std::setprecision(1) << entry.entropyBits << " bits\n";
        }
      }
      TerminalUI::pause();
    } else if (choice == 2) {
      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printHeader("RAW AUDIT LOG FILE");

      auto lines = logger.loadRawReportLines();
      if (lines.empty()) {
        TerminalUI::printInfo("Log file is empty or missing.");
      } else {
        for (const auto &line : lines) {
          std::cout << line << "\n";
        }
      }
      TerminalUI::pause();
    } else if (choice == 3) {
      std::string confirm = TerminalUI::getStringInput(
          "  Are you sure you want to clear audit history? (y/n): ");
      if (confirm == "y" || confirm == "Y") {
        if (logger.clearAuditHistory()) {
          TerminalUI::printSuccess("Audit history cleared successfully.");
        } else {
          TerminalUI::printError("Failed to clear audit history.");
        }
      }
      TerminalUI::pause();
    } else if (choice == 4) {
      break;
    }
  }
}

void SAMApp::handleDictionaryManagement() {
  while (true) {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("MODULE 5:DICTIONARY MANAGEMENT");

    auto &dict = analyzer.getDictionaryManager();
    std::cout << TerminalUI::BRIGHT_CYAN
              << "  Current Dictionary Size: " << TerminalUI::BOLD
              << dict.getDictionarySize() << " words\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::BRIGHT_CYAN
              << "  Dictionary File Path   : " << dict.getFilePath() << "\n\n"
              << TerminalUI::RESET;

    std::cout << TerminalUI::CYAN << "  [1] " << TerminalUI::BOLD
              << "List Sample Dictionary Entries\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [2] " << TerminalUI::BOLD
              << "Add New Weak Password Entry\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [3] " << TerminalUI::BOLD
              << "Reload Dictionary from Disk\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [4] " << TerminalUI::BOLD
              << "Return to Main Menu\n\n"
              << TerminalUI::RESET;

    int choice = TerminalUI::getIntInput("  Enter choice (1-4): ", 1, 4);

    if (choice == 1) {
      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printHeader("DICTIONARY ENTRIES");

      const auto &list = dict.getWeakPasswords();
      int count = 0;
      for (const auto &word : list) {
        std::cout << std::left << std::setw(18) << word;
        count++;
        if (count % 4 == 0)
          std::cout << "\n";
      }
      if (count % 4 != 0)
        std::cout << "\n";
      TerminalUI::pause();
    } else if (choice == 2) {
      std::string newWord = TerminalUI::getStringInput(
          "  Enter new weak password word to add to dictionary: ");
      if (dict.addPasswordToDictionary(newWord)) {
        TerminalUI::printSuccess("Added '" + newWord +
                                 "' to weak password dictionary!");
      } else {
        TerminalUI::printWarning("Word is already present in dictionary!");
      }
      TerminalUI::pause();
    } else if (choice == 3) {
      if (dict.loadDictionary()) {
        TerminalUI::printSuccess(
            "Dictionary reloaded from disk. Total entries: " +
            std::to_string(dict.getDictionarySize()));
      } else {
        TerminalUI::printError("Failed to reload dictionary from file.");
      }
      TerminalUI::pause();
    } else if (choice == 4) {
      break;
    }
  }
}

void SAMApp::handleEthicsNotice() {
  TerminalUI::clearScreen();
  TerminalUI::printBanner();
  TerminalUI::printHeader("MODULE 6: ETHICS, PRIVACY & AUTHOR STATEMENT");

  std::vector<std::string> lines = {
      "PROJECT AUTHOR & METADATA:",
      "  Author     : Muhammad Salman Jawed",
      "  Degree     : BS Digital Forensics and Cybersecurity (BSDFCS)",
      "  Project    : S.A.M - Secure Auth Monitor",
      "",
      "ETHICAL USE GUARANTEE & PRIVACY STATEMENT:",
      "  1. S.A.M operates entirely offline on local sample test data.",
      "  2. Actual passwords evaluated are NEVER transmitted over any network.",
      "  3. Log files (analysis_report.txt) store ONLY MASKED representations",
      "     (e.g., 'P**********!') to strictly preserve user privacy.",
      "  4. This software is built exclusively for academic coursework,",
      "     defensive security training, and awareness education."};
  TerminalUI::printBox("Ethical Guidelines & Project Ownership", lines,
                       TerminalUI::MAGENTA);
  TerminalUI::pause();
}

void SAMApp::run() {
  TerminalUI::initTerminal();

  while (true) {
    TerminalUI::clearScreen();
    TerminalUI::printBanner();
    TerminalUI::printHeader("MAIN CONTROLLER MENU");

    std::cout << TerminalUI::CYAN << "  [1] " << TerminalUI::BOLD
              << "Evaluate Password Strength & Complexity\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [2] " << TerminalUI::BOLD
              << "Run Dictionary & Vulnerability Pattern Audit\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [3] " << TerminalUI::BOLD
              << "Security Education Center & Passphrase Generator\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [4] " << TerminalUI::BOLD
              << "View & Manage Session Audit Reports\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [5] " << TerminalUI::BOLD
              << "Manage RockYou Password Dictionary\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [6] " << TerminalUI::BOLD
              << "Ethics & Security Privacy Statement\n"
              << TerminalUI::RESET;
    std::cout << TerminalUI::CYAN << "  [7] " << TerminalUI::BOLD
              << "Exit S.A.M\n\n"
              << TerminalUI::RESET;

    int choice = TerminalUI::getIntInput("  Select option (1-7): ", 1, 7);

    if (choice == 1)
      handleEvaluatePassword();
    else if (choice == 2)
      handleVulnerabilityChecker();
    else if (choice == 3)
      handleSecurityEducationAndGenerator();
    else if (choice == 4)
      handleAuditReports();
    else if (choice == 5)
      handleDictionaryManagement();
    else if (choice == 6)
      handleEthicsNotice();
    else if (choice == 7) {
      TerminalUI::clearScreen();
      TerminalUI::printBanner();
      TerminalUI::printSuccess(
          "Thank you for using S.A.M (Secure Auth Monitor). Stay secure!");
      break;
    }
  }
}

} // namespace SAM
