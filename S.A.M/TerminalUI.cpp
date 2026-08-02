#include "TerminalUI.h"
#include <iostream>
#include <limits>
#include <iomanip>

#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

namespace SAM {

const std::string TerminalUI::RESET         = "\033[0m";

// Summer Sky Color Theme (#38BDF8) + Distinct Alert Color Scheme
const std::string TerminalUI::CYAN          = "\033[38;2;56;189;248m";   // Summer Sky (#38BDF8) - Info / Prompts / Metric Labels
const std::string TerminalUI::BRIGHT_CYAN   = "\033[38;2;56;189;248;1m"; // Bright Summer Sky (#38BDF8)
const std::string TerminalUI::GREEN         = "\033[38;2;16;185;129m";   // Bright Emerald (#10B981) - Success
const std::string TerminalUI::BRIGHT_GREEN = "\033[38;2;110;231;183;1m";// Electric Mint (#6EE7B7)
const std::string TerminalUI::YELLOW        = "\033[38;2;245;158;11m";   // Amber Yellow (#F59E0B) - Warnings
const std::string TerminalUI::BRIGHT_YELLOW= "\033[38;2;251;191;36;1m"; // Bright Amber (#FBBF24)
const std::string TerminalUI::RED           = "\033[38;2;239;68;68m";    // Crimson Red (#EF4444) - Errors
const std::string TerminalUI::BRIGHT_RED    = "\033[38;2;248;113;113;1m";// Bright Crimson (#F87171)
const std::string TerminalUI::MAGENTA      = "\033[38;2;168;85;247m";   // Bright Purple (#A855F7)
const std::string TerminalUI::BOLD         = "\033[1m";
const std::string TerminalUI::DIM          = "\033[38;2;52;211;153m";   // Muted Jade Subtext (#34D399)
const std::string TerminalUI::PEACH        = "\033[38;2;255;186;147;1m"; // Warm Peach (#FFBA93)
const std::string TerminalUI::BADGE_RED     = "\033[48;2;239;68;68m\033[38;2;255;255;255;1m";

const std::string TerminalUI::VERSION      = "v1.0.4";
const std::string TerminalUI::BUILD_NUMBER = "2026.08.02-RELEASE";

void TerminalUI::initTerminal() {
#ifdef _WIN32
    // Enable ANSI escape sequence support in Windows Console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    // Set code page to UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void TerminalUI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

static std::string getLinearGradientColor(double t) {
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    int r, g, b;
    if (t <= 0.5) {
        // Green (16, 185, 129) -> Orange (249, 115, 22)
        double factor = t / 0.5;
        r = static_cast<int>(16 + (249 - 16) * factor);
        g = static_cast<int>(185 + (115 - 185) * factor);
        b = static_cast<int>(129 + (22 - 129) * factor);
    } else {
        // Orange (249, 115, 22) -> Red (239, 68, 68)
        double factor = (t - 0.5) / 0.5;
        r = static_cast<int>(249 + (239 - 249) * factor);
        g = static_cast<int>(115 + (68 - 115) * factor);
        b = static_cast<int>(22 + (68 - 22) * factor);
    }

    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + ";1m";
}

static void printLinearGradientLine(const std::string& line, int indentSpaces = 20) {
    std::cout << std::string(indentSpaces, ' ');
    
    std::vector<std::string> glyphs;
    size_t i = 0;
    while (i < line.length()) {
        unsigned char c = static_cast<unsigned char>(line[i]);
        size_t len = 1;
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        
        if (i + len <= line.length()) {
            glyphs.push_back(line.substr(i, len));
            i += len;
        } else {
            glyphs.push_back(line.substr(i));
            break;
        }
    }

    size_t total = glyphs.size();
    for (size_t g = 0; g < total; ++g) {
        double t = (total > 1) ? static_cast<double>(g) / static_cast<double>(total - 1) : 0.0;
        std::cout << getLinearGradientColor(t) << glyphs[g];
    }
    std::cout << TerminalUI::RESET << "\n";
}

void TerminalUI::printBanner() {
    std::cout << "\n";
    printLinearGradientLine("███████╗    █████╗    ███╗   ███╗", 20);
    printLinearGradientLine("██╔════╝   ██╔══██╗   ████╗ ████║", 20);
    printLinearGradientLine("███████╗   ███████║   ██╔████╔██║", 20);
    printLinearGradientLine("╚════██║   ██╔══██║   ██║╚██╔╝██║", 20);
    printLinearGradientLine("███████║██╗██║  ██║██╗██║ ╚═╝ ██║", 20);
    printLinearGradientLine("╚══════╝╚═╝╚═╝  ╚═╝╚═╝╚═╝     ╚═╝", 20);
    printLinearGradientLine("Secure Auth Monitor", 28);
    std::cout << PEACH << "                      " << VERSION << " | Build " << BUILD_NUMBER << RESET << "\n\n";
}

void TerminalUI::printHeader(const std::string& title) {
    printDivider('=', 75, CYAN);
    std::cout << CYAN << BOLD << "  " << title << RESET << "\n";
    printDivider('=', 75, CYAN);
}

static size_t getVisibleLength(const std::string& str) {
    size_t len = 0;
    bool inEscape = false;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\033') {
            inEscape = true;
        } else if (inEscape) {
            if (str[i] == 'm') {
                inEscape = false;
            }
        } else {
            len++;
        }
    }
    return len;
}

void TerminalUI::printBox(const std::string& title, const std::vector<std::string>& lines, const std::string& color) {
    int maxLen = getVisibleLength(title) + 4;
    for (const auto& line : lines) {
        int visLen = getVisibleLength(line);
        if (visLen + 4 > maxLen) {
            maxLen = visLen + 4;
        }
    }
    if (maxLen < 60) maxLen = 60;

    std::cout << color << "+" << std::string(maxLen - 2, '-') << "+" << RESET << "\n";
    if (!title.empty()) {
        int tLen = getVisibleLength(title);
        int pad = maxLen - 3 - tLen;
        if (pad < 0) pad = 0;
        std::cout << color << "| " << BOLD << title << RESET << std::string(pad, ' ') << color << "|" << RESET << "\n";
        std::cout << color << "+" << std::string(maxLen - 2, '=') << "+" << RESET << "\n";
    }
    for (const auto& line : lines) {
        int visLen = getVisibleLength(line);
        int pad = maxLen - 3 - visLen;
        if (pad < 0) pad = 0;
        std::cout << color << "| " << RESET << line << std::string(pad, ' ') << color << "|" << RESET << "\n";
    }
    std::cout << color << "+" << std::string(maxLen - 2, '-') << "+" << RESET << "\n";
}

void TerminalUI::printDivider(char ch, int width, const std::string& color) {
    std::cout << color << std::string(width, ch) << RESET << "\n";
}

void TerminalUI::printStatus(const std::string& label, const std::string& val, const std::string& color) {
    std::cout << BRIGHT_CYAN << "  [>] " << BOLD << std::left << std::setw(25) << label << ": " 
              << color << val << RESET << "\n";
}

void TerminalUI::printSuccess(const std::string& message) {
    std::cout << "  " << GREEN << message << RESET << "\n";
}

void TerminalUI::printWarning(const std::string& message) {
    std::cout << "  " << YELLOW << message << RESET << "\n";
}

void TerminalUI::printError(const std::string& message) {
    std::cout << "  " << BADGE_RED << " ERROR " << RESET << "  " << RED << message << RESET << "\n";
}

void TerminalUI::printInfo(const std::string& message) {
    std::cout << "  " << CYAN << message << RESET << "\n";
}

int TerminalUI::getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << BRIGHT_CYAN << prompt << RESET;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
        }
        if (std::cin.eof()) {
            return maxVal; // Return exit choice if EOF encountered
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printError("Invalid selection. Please enter a number between " + std::to_string(minVal) + " and " + std::to_string(maxVal) + ".");
    }
}

std::string TerminalUI::getStringInput(const std::string& prompt, bool allowEmpty) {
    std::string input;
    while (true) {
        std::cout << BRIGHT_CYAN << prompt << RESET;
        if (!std::getline(std::cin, input) && std::cin.eof()) {
            return "";
        }
        if (!input.empty() || allowEmpty) {
            return input;
        }
        printError("Input cannot be empty. Please try again.");
    }
}

void TerminalUI::pause() {
    std::cout << "\n" << DIM << "Press Enter to return to menu..." << RESET;
    if (std::cin.eof()) return;
    std::cin.get();
}

void TerminalUI::exitPauseWithCountdown(int seconds) {
    std::cout << "\n  " << DIM << "Press Enter to exit now (auto-closing in " << seconds << "s...): " << RESET;
    std::cout.flush();

#ifdef _WIN32
    int totalMs = seconds * 1000;
    int elapsedMs = 0;
    int lastDisplayedSec = seconds;

    while (elapsedMs < totalMs) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 13 || ch == 10) { // Enter key
                break;
            }
        }
        Sleep(100);
        elapsedMs += 100;
        int remainingSec = (totalMs - elapsedMs + 999) / 1000;
        if (remainingSec != lastDisplayedSec && remainingSec >= 0) {
            std::cout << "\r  " << DIM << "Press Enter to exit now (auto-closing in " << remainingSec << "s...): " << RESET;
            std::cout.flush();
            lastDisplayedSec = remainingSec;
        }
    }
#else
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
#endif
    std::cout << "\n";
}

} // namespace SAM
