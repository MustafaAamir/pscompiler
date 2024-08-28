#pragma once

#include "../chunk/chunk.h"
#include "../tokens/tokens.h"
#include "../lexer/lexer.h"
#include "../error/error.h"
#include <optional>

enum class Precedence {
    Newline,
    None,
    Assignment,
    Or,
    And,
    Equality,
    Comparison,
    Term,
    Factor,
    Unary,
    Call,
    Primary
};

enum class Scope {
    Global,
    Function,
    Case,
    If,
};

typedef struct Identifier {
    std::string name;
    int depth;
    Identifier(std::string name, int depth) : name(name), depth(depth) {};
} Identifier;

class Compiler {
    private:
        ErrorReporter Error {};
        bool panicMode;
        void increment(Value literal, i64 value, OpCode opGet, OpCode opSet);
        void parseMidStatement();
        void andJump();
        void beginScope();
        size_t emitJump(OpCode opCode);
        void   emitLoop(size_t jump);
        void patchJump(size_t offset);
        void program();
        void endScope();
        static const std::unordered_map<TokenType, TokenType> blockMap;
        void parseAssignmentStatement();
        void parseArrayIdentifier(bool isArray);
        void parseForAssignmentStatement(Token iterator);
        void grouping();
        void consume(TokenType type, std::string msg);
        void advanceIf(TokenType type, std::string msg);
        void resolver();
        void expressionStatement();
        void synchronize();
        void block(TokenType endBlock);
        void block(TokenType endBlock, TokenType endBlock2);
        void declareVariables(std::vector<Token> identifiers, TokenType type, bool newline, bool isArray);
        static const std::unordered_map<TokenType, Precedence> precedenceMap;
        void emitPendingGet();
        void emitConstant(Value &&value);
        void emitCall(i64 &&idx);
        void emitPop();
        void printStatement();
        template<typename T> bool isType(Value v) { return std::holds_alternative<T>(v);}
        void statement();
        void expression();
        std::string expressionBindingPower(Precedence precedence);
        void parsePrecedence(Precedence precedence);
        void parseCallStatement();

        void value();
        void unary();
        void binary();
        void lookupBinary(TokenType type);
        bool lookupUnary(TokenType type);
        Precedence lookupPrecedence(TokenType type);
        void lookupOpCode(TokenType type);

        void parseOutputStatement();
        void parseInputStatement();
        TokenType getArrayDeclarationType();
        void parseDeclareStatement();
        void parseForLoopStatement();
        void parseWhileLoopStatement();
        void parseRepeatLoopStatement();
        void parseIfStatement();
        void parseProcedureStatement();
        bool checkGlobalExists();
        bool checkLocalExists();


        size_t idx {0};
        Token currentToken = Token(TokenType::Eof, 0, 0, std::monostate{});
        Token peekToken = Token(TokenType::Eof, 0, 0, std::monostate{});
        Lexer lexer {};

        int scopeDepth {0};
        std::vector<Token *> TokenList;
        void parseIdentifierExpression();
        bool match(TokenType type);
        void advance();
        void retreat();
        void consume();
    public:
        unordered_map<string, size_t> functionIdxMap;
        std::vector<Identifier> identifiers;
        std::unordered_map<std::string, TokenType> globalsType;
        std::unordered_map<std::string, TokenType> localsType;
        std::unique_ptr<Chunk> chunk;
        vector<std::pair<int, int>> chunkPosition;
        void emit(OpCode opCode, std::optional<std::byte> argument = std::nullopt);
        Compiler() = default;
        std::unique_ptr<Chunk> compile(std::string &input, int line);
        void initCompiler(std::string &input);
};



