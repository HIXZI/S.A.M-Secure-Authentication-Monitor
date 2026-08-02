#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>
#include <vector>
#include "PasswordAnalyzer.h"

namespace SAM {

struct AuditLogEntry {
    std::string timestamp;
    std::string maskedPassword;
    int score;
    std::string strengthRating;
    double entropyBits;
    size_t length;
    bool dictionaryMatch;
    bool patternDetected;
};

class AuditLogger {
private:
    std::string reportFilePath;

    std::string getCurrentTimestamp() const;

public:
    explicit AuditLogger(const std::string& filePath = "analysis_report.txt");

    bool logReport(const AnalysisReport& report);
    std::vector<AuditLogEntry> loadAuditHistory() const;
    std::vector<std::string> loadRawReportLines() const;
    bool clearAuditHistory();

    const std::string& getFilePath() const;
};

} // namespace SAM

#endif // AUDIT_LOGGER_H
