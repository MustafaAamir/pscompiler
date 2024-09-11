#pragma once

#include "../tokens/tokens.h"
#include <ostream>
#include <exception>
#include <stdexcept>
#include <optional>
#include <unordered_map>

enum AnsiCode {
    FG_BLACK    = 30,
    FG_RED      = 31,
    FG_GREEN    = 32,
    FG_BLUE     = 34,
    FG_MAGENTA  = 35,
    FG_CYAN     = 36,
    FG_DEFAULT  = 39,
    BG_RED      = 41,
    BG_GREEN    = 42,
    BG_BLUE     = 44,
    BG_DEFAULT  = 49,
    FG_BBLACK   = 90,
    FG_BBLUE    = 94,
    FG_BMAGENTA = 95,
    FG_BWHITE   = 97
};


class Modifier {
    AnsiCode code;
public:
    Modifier(AnsiCode pCode) : code(pCode) {}
    friend std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
        return os << "\033[" << mod.code << "m";
    }
};

class ErrorReporter {
    public:
        ErrorReporter() = default;
        const bool logging = false;
        void logger(const char *func, const std::string msg, int indentation, std::optional<Token> optToken = std::nullopt);
        void report(const Token &token, const std::string category, const std::string message);
        void report(int line, int column, const std::string category, const std::string message);
        void report(std::pair<int, int> currentPosition, const std::string category, const std::string message);
};

