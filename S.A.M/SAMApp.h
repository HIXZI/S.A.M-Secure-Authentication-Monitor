#ifndef SAM_APP_H
#define SAM_APP_H

#include "PasswordAnalyzer.h"
#include "PassphraseGenerator.h"
#include "AuditLogger.h"

namespace SAM {

class SAMApp {
private:
    PasswordAnalyzer analyzer;
    PassphraseGenerator generator;
    AuditLogger logger;

    void handleEvaluatePassword();
    void handleVulnerabilityChecker();
    void handleSecurityEducationAndGenerator();
    void handleAuditReports();
    void handleDictionaryManagement();
    void handleEthicsNotice();

    void displayReportUI(const AnalysisReport& report) const;

public:
    SAMApp();
    void run();
};

} // namespace SAM

#endif // SAM_APP_H
