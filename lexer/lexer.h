#pragma once
#include "../common.h"
#include "../tokens/tokens.h"
#include "../error/error.h"



class Lexer {
private:
    ErrorReporter Error;
    const string *input;
    vector<std::unique_ptr<struct Token>> TokenList;
    char currentChar;
    int line;
    int column;
    size_t idx;
    void advance();
    void makeWord();
    void makeChar();
    void makeNumber();
    void makeString();
    char nextNChar(size_t n);
    static const unordered_map<string, TokenType> keywords;
    static const unordered_map<string, TokenType> symbols;
public:
    Lexer() = default;
    Lexer(string *input);
    void initLexer(string *_input);
    const vector<Token*> makeTokens(bool print);
};





