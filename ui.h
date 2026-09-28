//
// Created by denni on 9/28/2026.
//

#ifndef KUHN_POKER_UI_H
#define KUHN_POKER_UI_H
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

namespace ui {

inline const std::string RESET   = "\033[0m";
inline const std::string BOLD    = "\033[1m";
inline const std::string DIM     = "\033[2m";
inline const std::string RED     = "\033[31m";
inline const std::string GREEN   = "\033[32m";
inline const std::string YELLOW  = "\033[33m";
inline const std::string BLUE    = "\033[34m";
inline const std::string MAGENTA = "\033[35m";
inline const std::string CYAN    = "\033[36m";

inline constexpr int W = 56;  // latimea interioara a chenarului

inline void EnableAnsi() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

inline void Clear() { std::cout << "\033[2J\033[H"; }

inline std::string Repeat(const std::string& s, int n) {
    std::string r;
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

inline std::string Fmt(double x, int precision = 2) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(precision) << x;
    return os.str();
}

inline std::string Signed(int x) { return (x > 0 ? "+" : "") + std::to_string(x); }

inline void BoxTop()    { std::cout << CYAN << "╭" << Repeat("─", W) << "╮" << RESET << "\n"; }
inline void BoxBottom() { std::cout << CYAN << "╰" << Repeat("─", W) << "╯" << RESET << "\n"; }
inline void BoxSep()    { std::cout << CYAN << "├" << Repeat("─", W) << "┤" << RESET << "\n"; }

// plain = textul vizibil (pentru aliniere), colored = acelasi text cu culori
inline void BoxLine(const std::string& plain, const std::string& colored) {
    std::cout << CYAN << "│" << RESET << colored
              << std::string(std::max(0, W - static_cast<int>(plain.size())), ' ')
              << CYAN << "│" << RESET << "\n";
}
inline void BoxLine(const std::string& plain) { BoxLine(plain, plain); }

// Text aliniat stanga + text aliniat dreapta pe aceeasi linie
inline void BoxSplit(const std::string& left_plain, const std::string& left_colored,
                     const std::string& right_plain, const std::string& right_colored) {
    const int gap = std::max(1, W - static_cast<int>(left_plain.size() + right_plain.size()));
    BoxLine(left_plain + std::string(gap, ' ') + right_plain,
            left_colored + std::string(gap, ' ') + right_colored);
}

}
#endif //KUHN_POKER_UI_H