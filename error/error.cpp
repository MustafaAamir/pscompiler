#include "error.h"
#include <sstream>


void ErrorReporter::report(const Token &token, const std::string category, const std::string message) {
    std::stringstream ss;
    ss << Modifier(AnsiCode::FG_RED) << category << " error: "  << Modifier(AnsiCode::FG_DEFAULT)
        << message <<". Line " << token.line << ", column " << token.column << std::endl;
    std::string err = ss.str();
    throw std::runtime_error(err);
}

void ErrorReporter::report(int line, int column, const std::string category, const std::string message) {
    std::stringstream ss;
    ss << Modifier(AnsiCode::FG_RED) << category << " error: "  << Modifier(AnsiCode::FG_DEFAULT)
        << message <<". Line " << line << ", column " << column << std::endl;
    std::string err = ss.str();
    throw std::runtime_error(err);
}

void ErrorReporter::report(std::pair<int, int> currentPosition, const std::string category, const std::string message) {
    std::stringstream ss;
    ss << Modifier(AnsiCode::FG_RED) << category << " error: "  << Modifier(AnsiCode::FG_DEFAULT)
        << message << ". Line " << get<0>(currentPosition) << ", column " << get<1>(currentPosition) << std::endl;
    std::string err = ss.str();
    throw std::runtime_error(err);
}


void ErrorReporter::logger(const char *func, const std::string msg, int indentation, std::optional<Token> optToken) {
    if (logging) {
        for (int i = 0; i < indentation; i++) {
            std::cout << "  ";
        }
        std::cout << Modifier(AnsiCode::FG_MAGENTA) << std::string(func) << " -> "
        << Modifier(AnsiCode::FG_DEFAULT) << msg;
        if (optToken.has_value()) {
            const auto token = optToken.value();
            std::cout << Modifier(AnsiCode::FG_GREEN) << " where (" << token.type << " : " << token.literal << ")";
        }
        std::cout << Modifier(AnsiCode::FG_DEFAULT) << std::endl;
    }

    return;
}

