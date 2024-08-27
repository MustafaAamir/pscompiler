#pragma once
#include "../common.h"


typedef enum class TokenType : uint8_t {
    Integer, Real, Char, String, Date, Boolean,

    Integer_t, Real_t, Char_t, String_t, Date_t, Boolean_t,

    Rparen, Lparen,

    Plus, Minus, Star, Slash, Div, Mod, Ampersand,

    Assignment, Colon, Comma,

    Equals, Not_equals, Greater, Lesser, Greater_equal, Lesser_equal,

    And, Or, Not,

    Apostrophe, Speech, Comment,

    True, False,

    Declare, Constant, Identifier,

    Data_type, Array, Lsqrbracket, Rsqrbracket,

    Type, Endtype, Caret, Period,

    If, Then, Else, Endif,

    Case, Of, Otherwise, Endcase,

    While, Do, Endwhile,

    Repeat, Until,

    For, To, Step, Next,

    Break, Continue,

    Procedure, Byref, Byval, Endprocedure, Call,

    Function, Endfunction, Returns, Return,

    Output, Input,

    Openfile, Readfile, Writefile, Closefile,

    Read, Write, Append, Random,

    Seek, Getrecord, Putrecord,

    Sin, Cos, Tan, Sqrt, Abs, IntCast, RealCast, StringCast,

    Newline, Eof,

} TokenType;

struct Token {
    TokenType type;
    int line;
    int column;
    Value literal;
    Token(TokenType type, int line, int column, const Value &literal = std::monostate{});
};

std::ostream &operator<<(std::ostream &os, const TokenType &t);
std::ostream &operator<<(std::ostream &os, const Token &t);
std::ostream &operator<<(std::ostream &os, const Value& l);


