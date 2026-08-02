#include "AuditLogger.h"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace SAM {

AuditLogger::AuditLogger(const std::string& filePath)
    : reportFilePath(filePath) {}

std::string AuditLogger::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm tmStruct;
#ifdef _WIN32
    localtime_s(&tmStruct, &now);
#else
    localtime_r(&now, &tmStruct);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmStruct, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool AuditLogger::logReport(const AnalysisReport& report) {
    std::ofstream file(reportFilePath, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    std::string timestamp = getCurrentTimestamp();
    std::string masked = report.getMaskedPassword();

    file << "========================================================\n";
    file << "TIMESTAMP        : " << timestamp << "\n";
    file << "MASKED PASSWORD  : " << masked << "\n";
    file << "STRENGTH RATING  : " << report.getStrengthString() << "\n";
    file << "COMPLEXITY SCORE : " << report.score << " / 100\n";
    file << "ENTROPY          : " << std::fixed << std::setprecision(2) << report.entropyBits << " bits\n";
    file << "LENGTH           : " << report.length << " characters\n";
    file << "CHAR BREAKDOWN   : Upper=" << report.uppercaseCount 
         << " | Lower=" << report.lowercaseCount 
         << " | Digits=" << report.digitCount 
         << " | Symbols=" << report.specialCharCount << "\n";
    file << "DICTIONARY MATCH : " << (report.isDictionaryMatch ? "YES (Vulnerable!)" : "NO") << "\n";
    file << "PATTERNS FOUND   : " << (report.hasSequentialPattern || report.hasRepeatedPattern ? "YES" : "NO") << "\n";
    if (!report.recommendations.empty()) {
        file << "RECOMMENDATIONS  :\n";
        for (const auto& rec : report.recommendations) {
            file << "  - " << rec << "\n";
        }
    }
    file << "========================================================\n\n";

    file.close();
    return true;
}

std::vector<AuditLogEntry> AuditLogger::loadAuditHistory() const {
    std::vector<AuditLogEntry> history;
    std::ifstream file(reportFilePath);
    if (!file.is_open()) {
        return history;
    }

    std::string line;
    AuditLogEntry currentEntry;
    bool inEntry = false;

    while (std::getline(file, line)) {
        if (line.find("TIMESTAMP        :") != std::string::npos) {
            inEntry = true;
            currentEntry.timestamp = line.substr(line.find(":") + 2);
        } else if (line.find("MASKED PASSWORD  :") != std::string::npos) {
            currentEntry.maskedPassword = line.substr(line.find(":") + 2);
        } else if (line.find("STRENGTH RATING  :") != std::string::npos) {
            currentEntry.strengthRating = line.substr(line.find(":") + 2);
        } else if (line.find("COMPLEXITY SCORE :") != std::string::npos) {
            std::string scoreStr = line.substr(line.find(":") + 2);
            try { currentEntry.score = std::stoi(scoreStr); } catch (...) { currentEntry.score = 0; }
        } else if (line.find("ENTROPY          :") != std::string::npos) {
            std::string entStr = line.substr(line.find(":") + 2);
            try { currentEntry.entropyBits = std::stod(entStr); } catch (...) { currentEntry.entropyBits = 0.0; }
        } else if (line.find("LENGTH           :") != std::string::npos) {
            std::string lenStr = line.substr(line.find(":") + 2);
            try { currentEntry.length = std::stoul(lenStr); } catch (...) { currentEntry.length = 0; }
        } else if (line.find("DICTIONARY MATCH :") != std::string::npos) {
            currentEntry.dictionaryMatch = (line.find("YES") != std::string::npos);
        } else if (line.find("PATTERNS FOUND   :") != std::string::npos) {
            currentEntry.patternDetected = (line.find("YES") != std::string::npos);
        } else if (line.find("========================================================") != std::string::npos) {
            if (inEntry && !currentEntry.timestamp.empty()) {
                history.push_back(currentEntry);
                currentEntry = AuditLogEntry();
                inEntry = false;
            }
        }
    }
    file.close();
    return history;
}

std::vector<std::string> AuditLogger::loadRawReportLines() const {
    std::vector<std::string> lines;
    std::ifstream file(reportFilePath);
    if (!file.is_open()) {
        return lines;
    }
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    return lines;
}

bool AuditLogger::clearAuditHistory() {
    std::ofstream file(reportFilePath, std::ios::trunc);
    if (!file.is_open()) return false;
    file.close();
    return true;
}

const std::string& AuditLogger::getFilePath() const {
    return reportFilePath;
}

} // namespace SAM
