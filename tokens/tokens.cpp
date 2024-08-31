#include "tokens.h"
#include "../common.h"

Token::Token(TokenType type, int line, int column, const Value& literal)
    : type(type), line(line), column(column), literal(literal) {}

const char *TOKEN_TO_STR[] {
    "INTEGER", "REAL", "CHAR", "STRING", "DATE", "BOOLEAN",

    "INTEGER_t", "REAL_t", "CHAR_t", "STRING_t", "DATE_t", "BOOLEAN_t",

    ")", "(",

    "+", "-", "*", "/", "DIV", "MOD", "&",

    "<-", ":", ",",

    "=", "<>", ">", "<", ">=", "<=",

    "AND", "OR", "NOT",

    "'", "\"", "//",

    "TRUE", "FALSE",

    "DECLARE", "CONSTANT", "IDENTIFIER",

    "DATA_TYPE", "ARRAY", "[", "]",

    "TYPE", "ENDTYPE", "^", ".",

    "IF", "THEN", "ELSE", "ENDIF",

    "CASE", "OF", "OTHERWISE", "ENDCASE",

    "WHILE", "DO", "ENDWHILE",

    "REPEAT", "UNTIL",

    "FOR", "TO", "STEP", "NEXT",

    "BREAK", "CONTINUE",

    "PROCEDURE", "BYREF", "BYVAL", "ENDPROCEDURE", "CALL",

    "FUNCTION", "ENDFUNCTION", "RETURNS", "RETURN",

    "OUTPUT", "INPUT",

    "OPENFILE", "READFILE", "WRITEFILE", "CLOSEFILE",

    "READ", "WRITE", "APPEND", "RANDOM", "SEEK",

    "GETRECORD", "PUTRECORD",

    "SIN", "COS", "TAN", "SQRT", "ABS", "INTCAST", "REALCAST", "STRINGCAST",
    "MID", "REVERSE", "LENGTH", "RANDOM_INTEGER", "RANDOM_REAL", "SYSTEM",

    "NEWLINE", "EOF",
};


std::ostream &operator<<(std::ostream &os, const TokenType &t) {
    os << TOKEN_TO_STR[static_cast<uint8_t>(t)];
    return os;
}

std::ostream &operator<<(std::ostream &os, const Value& l) {
    if (std::holds_alternative<double>(l)) {
        os << std::get<double>(l);
    } else if (std::holds_alternative<i64>(l)) {
        os << std::get<i64>(l);
    } else if (std::holds_alternative<std::string>(l)){
        os << "\"" << std::get<std::string>(l) << "\"";
    } else if (std::holds_alternative<char>(l)) {
        os << "'" << std::get<char>(l) << "'";
    } else if (std::holds_alternative<bool>(l)) {
        const bool printable = std::get<bool>(l);
        if (printable) {
            os << "TRUE";
        } else {
            os << "FALSE";
        }
    } else if (std::holds_alternative<std::monostate>(l)) {
        os << "";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
    os << "Token{type: " << t.type << ", line: " << t.line << ", column: " << t.column << ", value: " << t.literal << "}";
    return os;
}
