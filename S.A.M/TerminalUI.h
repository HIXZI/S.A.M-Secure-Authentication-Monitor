#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include <string>
#include <vector>

namespace SAM {

// ANSI Color Escape Sequences for Retro Terminal UI
class TerminalUI {
public:
    static const std::string RESET;
    static const std::string CYAN;
    static const std::string BRIGHT_CYAN;
    static const std::string GREEN;
    static const std::string BRIGHT_GREEN;
    static const std::string YELLOW;
    static const std::string BRIGHT_YELLOW;
    static const std::string RED;
    static const std::string BRIGHT_RED;
    static const std::string MAGENTA;
    static const std::string BOLD;
    static const std::string DIM;
    static const std::string PEACH;
    static const std::string BADGE_RED;

    static const std::string VERSION;
    static const std::string BUILD_NUMBER;

    // Initialization
    static void initTerminal();
    static void clearScreen();

    // UI Renderers
    static void printBanner();
    static void printHeader(const std::string& title);
    static void printBox(const std::string& title, const std::vector<std::string>& lines, const std::string& color = CYAN);
    static void printDivider(char ch = '-', int width = 75, const std::string& color = CYAN);
    static void printStatus(const std::string& label, const std::string& val, const std::string& color = BRIGHT_CYAN);
    
    // Alert Messages
    static void printSuccess(const std::string& message);
    static void printWarning(const std::string& message);
    static void printError(const std::string& message);
    static void printInfo(const std::string& message);

    // User Input Helpers
    static int getIntInput(const std::string& prompt, int minVal, int maxVal);
    static std::string getStringInput(const std::string& prompt, bool allowEmpty = false);
    static void pause();
    static void exitPauseWithCountdown(int seconds = 10);
};

} // namespace SAM

#endif // TERMINAL_UI_H
