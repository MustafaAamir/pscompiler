#include "lexer.h"
#include "../common.h"
#include "../tokens/tokens.h"
#include <cctype>
#include <algorithm>

const unordered_map<string, TokenType> Lexer::symbols = {
    {"(", TokenType::Lparen},
    {")", TokenType::Rparen},
    {"+", TokenType::Plus},
    {"-", TokenType::Minus},
    {"*", TokenType::Star},
    {"/", TokenType::Slash},
    {"//", TokenType::Comment},
    {"&", TokenType::Ampersand},
    {"<", TokenType::Lesser},
    {"<-", TokenType::Assignment},
    {"<=", TokenType::Lesser_equal},
    {"=", TokenType::Equals},
    {"<>", TokenType::Not_equals},
    {">", TokenType::Greater},
    {">=", TokenType::Greater_equal},
    {"'", TokenType::Apostrophe},
    {"\"", TokenType::Speech},
    {"[", TokenType::Lsqrbracket},
    {"]", TokenType::Rsqrbracket},
    {"^", TokenType::Caret},
    {":", TokenType::Colon},
    {",", TokenType::Comma},
    {".", TokenType::Period},
    {"\n", TokenType::Newline},
};

const unordered_map<string, TokenType> Lexer::keywords = {
    {"DECLARE", TokenType::Declare},
    {"CONSTANT", TokenType::Constant},
    {"CASE", TokenType::Case},
    {"OF", TokenType::Of},
    {"OTHERWISE", TokenType::Otherwise},
    {"ENDCASE", TokenType::Endcase},
    {"INTEGER", TokenType::Integer_t},
    {"REAL", TokenType::Real_t},
    {"BOOLEAN", TokenType::Boolean_t},
    {"CHAR", TokenType::Char_t},
    {"STRING", TokenType::String_t},
    {"Date", TokenType::Date_t},
    {"TRUE", TokenType::True},
    {"FALSE", TokenType::False},
    {"DIV",    TokenType::Div},
    {"MOD",   TokenType::Mod},
    {"TYPE", TokenType::Type},
    {"ENDTYPE", TokenType::Endtype},
    {"FOR", TokenType::For},
    {"TO", TokenType::To},
    {"STEP", TokenType::Step},
    {"NEXT", TokenType::Next},
    {"IF", TokenType::If},
    {"THEN", TokenType::Then},
    {"ELSE", TokenType::Else},
    {"ENDIF", TokenType::Endif},
    {"WHILE", TokenType::While},
    {"DO", TokenType::Do},
    {"ENDWHILE", TokenType::Endwhile},
    {"ARRAY", TokenType::Array},
    {"REPEAT", TokenType::Repeat},
    {"UNTIL", TokenType::Until},
    {"BREAK", TokenType::Break},
    {"CONTINUE", TokenType::Continue},
    {"PROCEDURE", TokenType::Procedure},
    {"BYREF", TokenType::Byref},
    {"BYVAL", TokenType::Byval},
    {"ENDPROCEDURE", TokenType::Endprocedure},
    {"CALL", TokenType::Call},
    {"FUNCTION", TokenType::Function},
    {"RETURNS", TokenType::Returns},
    {"RETURN", TokenType::Return},
    {"ENDFUNCTION", TokenType::Endfunction},
    {"OUTPUT", TokenType::Output},
    {"PRINT", TokenType::Output},
    {"INPUT", TokenType::Input},
    {"OPENFILE", TokenType::Openfile},
    {"READFILE", TokenType::Readfile},
    {"WRITEFILE", TokenType::Writefile},
    {"CLOSEFILE", TokenType::Closefile},
    {"READ", TokenType::Read},
    {"WRITE", TokenType::Write},
    {"APPEND", TokenType::Append},
    {"RANDOM", TokenType::Random},
    {"SEEK", TokenType::Seek},
    {"GETRECORD", TokenType::Getrecord},
    {"PUTRECORD", TokenType::Putrecord},
    {"AND", TokenType::And},
    {"OR", TokenType::Or},
    {"NOT", TokenType::Not},
};

Lexer::Lexer(string *input) {
    initLexer(input);
}

void Lexer::initLexer(string *_input) {
  _input->erase(remove(_input->begin(), _input->end(), '\r'), _input->end());
  input = _input;
  idx = SIZE_MAX;
  currentChar = 0;
  line = 1;
  column = 0;
  advance();
}

void Lexer::advance() {
    if (++idx >= input->size()) {
        return;
    }
    if (currentChar == '\n') {
        ++line;
        column = 0;
    }
    currentChar = (*input)[idx];
    column++;
}

char Lexer::nextNChar(size_t n) {
    if (idx + n >= input->size()) {
        return 0;
    }
    return (*input)[idx + n];
}
char escFmt(char c) {
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case '\'':
            return '\'';
        case '"':
            return '"';
        case '\\':
            return '\\';
        default:
            return -1;
    }
}

void Lexer::makeString() {
    int startColumn = column;
    advance();
    string str;
    while (currentChar != '"' && idx < input->size()) {
        if (currentChar == '\\') {
            advance();
            char c = escFmt(currentChar);
            if (c == -1) {
                Error.report(line, column, "Syntax", "Invalid escape sequence");
            }
        } else {
            str += currentChar;
        }
        advance();
    }
    if (idx >= input->size() || currentChar != '"') {
        Error.report(line, column, "Syntax", "String literal is invalid. End with '\"\'");
    }
    TokenList.emplace_back(new Token(TokenType::String, line, startColumn, str));
}

void Lexer::makeChar() {
    if (idx + 2 >= input->size()) {
        Error.report(line, column, "Syntax", "CHAR is invalid");
    }
    int startColumn = column;
    advance();
    char c;
    if (currentChar == '\\') {
        advance();
        c = escFmt(currentChar);
        if (c == -1){
            Error.report(line, column, "Syntax", "Invalid esq sequence");
        }
    } else if (currentChar == '\'') {
        Error.report(line, column, "Syntax", "CHAR must contain at least 1 character");
    } else {
        c = currentChar;
    }
    if (idx + 1 >= input->size() || (*input)[idx + 1] != '\'') {
        Error.report(line, column, "Syntax", "CHAR can only contain one character");
    }
    advance();
    TokenList.emplace_back(new Token(TokenType::Char, line, startColumn, c));
}

void Lexer::makeWord() {
  size_t startIdx = idx;
  int startColumn = column;

  for (; idx < input->size(); Lexer::advance()) {
      if (!isalpha(currentChar) && !isdigit(currentChar) && currentChar != '_') {
          break;
      }
  }
  string word = input->substr(startIdx, idx - startIdx);
  unordered_map<string, TokenType>::const_iterator it = Lexer::keywords.find(word);
  if (it == Lexer::keywords.end()) {
      TokenList.emplace_back(new Token(TokenType::Identifier, line, startColumn, word));
  } else {
      if (it->second == TokenType::True) {
          TokenList.emplace_back(new Token(TokenType::Boolean, line, startColumn, true));
      } else if (it->second == TokenType::False) {
          TokenList.emplace_back(new Token(TokenType::Boolean, line, startColumn, false));
      } else {
          TokenList.emplace_back(new Token(it->second, line, startColumn, std::monostate{}));
      }
  }
}

void Lexer::makeNumber() {
    int startIdx = idx;
    int startColumn = column;
    bool real = false;
    for ( ; idx < input->size(); advance()) {
        if (currentChar == '.' && !real) {
            real = true;
            continue;
        }
        if (!isdigit(currentChar)) {break;}
    }
    if (real == true) {
        TokenList.emplace_back(new Token(TokenType::Real, line, startIdx, atof(input->substr(startIdx, idx - startIdx).c_str())));
    } else {
        TokenList.emplace_back(new Token(TokenType::Integer, line, startIdx, strtoll(input->substr(startIdx, idx - startIdx).c_str(), NULL, 10)));
    }
    if (currentChar != '/' || real) { return; }
    int i = 0;
    char c = nextNChar(i);
    while (isdigit(c = nextNChar(++i))) {}
    if (c != '/' || !isdigit(c = nextNChar(++i))) return;
    for (int j = 0; j < i; j++){
      advance();
    }
    while (isdigit(currentChar) && idx < input->size()) advance();
    TokenList.pop_back();
    TokenList.emplace_back(new Token(TokenType::Date, line, startColumn, input->substr(startIdx, idx - startIdx)));
}

const vector<Token *> Lexer::makeTokens(bool print) {
    size_t reserve = input->size() / 10;
    if (reserve < 1) {
        reserve = 1;
    }
    TokenList.reserve(reserve);
    size_t currentTokensIdx = TokenList.size();
    vector<Token *> currentTokens;
    while (idx < input->size()) {
        if (isalpha(currentChar)) {
            makeWord();
            continue;
        } else if (isdigit(currentChar)) {
            makeNumber();
            continue;
        } else {
            string c1(1, currentChar);
            string c2;
            c2.push_back(currentChar);
            c2.push_back(nextNChar(1));
            auto t1 = symbols.find(c1);
            auto t2 = symbols.find(c2);
            if (t2 != symbols.end()) {
                if (t2->second == TokenType::Comment){
                    int commentLine = line;
                    while (line == commentLine && idx < input->size()) {advance();}
                } else {
                    TokenList.emplace_back(new Token(t2->second, line, column));
                }
                advance();
                advance();
                continue;
            } else if (t1 != symbols.end()) {
                switch (t1->second) {
                    case TokenType::Apostrophe: Lexer::makeChar(); break;
                    case TokenType::Speech: Lexer::makeString(); break;
                    default: TokenList.emplace_back(new Token(t1->second, line, column)); break;
                }
            } else if (currentChar != ' ' && currentChar != '\t'){
                Error.report(line, column, "Syntax", "invalid character");
            }
        }
        advance();
    }

    TokenList.emplace_back(new Token(TokenType::Eof, line, column));
    currentTokens.reserve(TokenList.size() - currentTokensIdx + 1);
    for (size_t i = currentTokensIdx; i < TokenList.size() - 1; i++) {
        currentTokens.push_back(TokenList[i].get());
    }
    currentTokens.push_back(new Token(TokenType::Eof, line, column));
    if (print) {
        cout << Modifier(AnsiCode::FG_BLUE);
        for (auto token : currentTokens) {
            cout << *token << endl;
        }
        cout << Modifier(AnsiCode::FG_DEFAULT);
    }
    return currentTokens;
}


