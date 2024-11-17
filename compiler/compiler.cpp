#include "compiler.h"
#include "../chunk/chunk.h"
#include "../common.h"
#include "../tokens/tokens.h"
#include <limits>
#include <sstream>

bool Compiler::lookupUnary(TokenType type) {
    bool isNewline = false;
    switch (type) {
    case TokenType::Minus:
        unary();
        break;
    case TokenType::Not:
        unary();
        break;
    case TokenType::Sin:
        unary();
        break;
    case TokenType::Cos:
        unary();
        break;
    case TokenType::Tan:
        unary();
        break;
    case TokenType::Sqrt:
        unary();
        break;
    case TokenType::Abs:
        unary();
        break;
    case TokenType::IntCast:
        unary();
        break;
    case TokenType::RealCast:
        unary();
        break;
    case TokenType::StringCast:
        unary();
        break;
    case TokenType::Reverse:
        unary();
        break;
    case TokenType::Length:
        unary();
        break;
    case TokenType::Integer:
        value();
        break;
    case TokenType::String:
        value();
        break;
    case TokenType::Real:
        value();
        break;
    case TokenType::Boolean:
        value();
        break;
    case TokenType::Char:
        value();
        break;
    case TokenType::Identifier:
        resolver();
        break;
    case TokenType::Newline:
        advance();
        isNewline = true;
        break;
    case TokenType::Then:
        isNewline = true;
        break;
    case TokenType::Rsqrbracket:
        isNewline = true;
        break;
    case TokenType::Assignment:
        isNewline = true;
        break;
    case TokenType::Lparen:
        grouping();
        break;
    case TokenType::Rparen:
        isNewline = true;
        break;
    case TokenType::Mid:
        parseMidStatement();
        break;
    case TokenType::RandomInt:
        parseRandomStatement(false);
        break;
    case TokenType::RandomReal:
        parseRandomStatement(true);
        break;
    default:
        return isNewline;
    }
    return isNewline;
}

void Compiler::parseMidStatement() {
    consume(TokenType::Lparen, "Expected ( after MID");
    advance();
    expression();
    consume(TokenType::Comma, "expected ',' after expression");
    advance();
    expression();
    consume(TokenType::Comma, "Expected ',' after second argument");
    advance();
    expression();
    advance();
    emitConstant(builtintype::Mid);
    emit(OpCode::Builtin);
}

void Compiler::parseSystemStatement() {
    advance();
    expression();
    advance();
    emitConstant(builtintype::System);
    emit(OpCode::Builtin);
}

void Compiler::parseRandomStatement(bool isreal) {
    consume(TokenType::Lparen, "Expected ( after functionCall");
    advance();
    expression();
    consume(TokenType::Comma, "expected ',' after lb");
    advance();
    expression();
    advance();
    if (isreal) {
        emitConstant(builtintype::RandomReal);
    } else {
        emitConstant(builtintype::RandomInt);
    }
    emit(OpCode::Builtin);
}

void Compiler::andJump() {
    int endJump = emitJump(OpCode::JumpNE);
    emitPop();
    parsePrecedence(Precedence::And);
    patchJump(endJump);
}

void Compiler::lookupBinary(TokenType type) {
    switch (type) {
    case TokenType::Plus:
        binary();
        break;
    case TokenType::Minus:
        binary();
        break;
    case TokenType::Star:
        binary();
        break;
    case TokenType::Slash:
        binary();
        break;
    case TokenType::Div:
        binary();
        break;
    case TokenType::Mod:
        binary();
        break;
    case TokenType::Ampersand:
        binary();
        break;
    case TokenType::Greater:
        binary();
        break;
    case TokenType::Greater_equal:
        binary();
        break;
    case TokenType::Lesser_equal:
        binary();
        break;
    case TokenType::Lesser:
        binary();
        break;
    case TokenType::Not_equals:
        binary();
        break;
    case TokenType::Equals:
        binary();
        break;
    case TokenType::And:
        binary();
        break;
    case TokenType::Or:
        binary();
        break;
    default:
        return;
    }
}

Precedence Compiler::lookupPrecedence(TokenType type) {
    const auto it = precedenceMap.find(type);
    if (it == precedenceMap.end()) {
        Error.report(currentToken, "Compile", "Precedence not found");
    }
    return it->second;
}

void Compiler::emit(OpCode opCode, std::optional<std::byte> argument) {
    chunkPosition.push_back(
        std::make_pair(currentToken.line, currentToken.column));
    chunk->writeChunk(opCode);
    if (argument) {
        chunkPosition.push_back(
            std::make_pair(currentToken.line, currentToken.column));
        chunk->writeByte(*argument);
    }
}

void Compiler::emitConstant(Value &&value) {
    auto idx = static_cast<uint16_t>(chunk->addConstant(std::move(value)));
    if (idx > std::numeric_limits<uint16_t>::max()) {
        Error.report(currentToken, "Stack overflow",
                     "too many constants in one chunk");
    }
    emit(OpCode::Constant, static_cast<std::byte>((idx >> 8) & 0xff));
    chunkPosition.push_back(
        std::make_pair(currentToken.line, currentToken.column));
    chunk->writeByte(static_cast<std::byte>(idx & 0xff));
}

void Compiler::emitCall(i64 &&idx) {
    const auto index =
        static_cast<uint16_t>(chunk->addConstant(std::move(idx)));
    if (index > std::numeric_limits<uint16_t>::max()) {
        Error.report(currentToken, "Stack overflow",
                     "too many constants in one chunk");
    }
    emit(OpCode::Call, static_cast<std::byte>((index >> 8) & 0xff));
    chunkPosition.push_back(
        std::make_pair(currentToken.line, currentToken.column));
    chunk->writeByte(static_cast<std::byte>(index & 0xff));
}

void Compiler::emitPop() { emit(OpCode::Pop); }

void Compiler::expression() { parsePrecedence(Precedence::None); }

void Compiler::initCompiler(string &input) {
    idx = 0;
    scopeDepth = 0;
    lexer.initLexer(&input);
    TokenList = lexer.makeTokens(false);
    if (TokenList.size() == 0) {
        Error.report(currentToken, "Compile", "Insufficient number of tokens");
    }
    if (TokenList.size() == 1) {
        currentToken = *TokenList[0];
        return;
    }
    currentToken = *TokenList[0];
    peekToken = *TokenList[1];
    chunk = std::make_unique<Chunk>();
    idx += 1;
}

bool Compiler::match(TokenType type) {
    if (currentToken.type != type) {
        return false;
    }
    advance();
    return true;
}

void Compiler::parseProcedureStatement() {
    consume(TokenType::Identifier, "Expected Identifier after PROCEDURE");
    const string name = get<string>(currentToken.literal);
    // consume(TokenType::Lparen, "Expected (");
    // consume(TokenType::Rparen, "Expected )");
    consume(TokenType::Newline, "Expected newline after Identifier");
    advance();
    beginScope();
    const size_t normalJump = emitJump(OpCode::Jump);
    const size_t callJmp = chunk->bytecode.size();
    functionIdxMap.emplace(name, callJmp);
    block(TokenType::Endprocedure);
    endScope();
    emit(OpCode::EndFunction);
    patchJump(normalJump);
    advance();
}

void Compiler::parseCallStatement() {
    consume(TokenType::Identifier, "Expected Identifier after CALL");
    const string name = get<string>(currentToken.literal);
    // consume(TokenType::Lparen, "Expected ( after Identifier");
    // consume(TokenType::Rparen, "Expected ) after args");
    const auto it = functionIdxMap.find(name);
    if (it == functionIdxMap.end()) {
        Error.report(currentToken, "Compiler",
                     "Function / Procedure is undefined");
    }
    auto idx = static_cast<i64>(it->second);
    emitCall(std::move(idx));
    consume(TokenType::Newline, "Expected newline after )");
}

std::unique_ptr<Chunk> Compiler::compile(string &input) {
    initCompiler(input);
    while (peekToken.type != TokenType::Eof) {
        program();
    }
    emit(OpCode::Return);
    return std::move(chunk);
}

void Compiler::advance() {
    if (TokenList[idx]->type != TokenType::Eof && idx < TokenList.size()) {
        currentToken = peekToken;
        peekToken = *TokenList[++idx];
    }
    return;
}

void Compiler::retreat() {
    if (idx > 2) {
        peekToken = currentToken;
        currentToken = *TokenList[--idx];
    }
    return;
}

TokenType Compiler::getArrayDeclarationType() {
    while (currentToken.type != TokenType::Rsqrbracket ||
           peekToken.type != TokenType::Of)
        advance();
    advance();
    TokenType arrayType = peekToken.type;
    while (currentToken.type != TokenType::Lsqrbracket)
        retreat();
    return arrayType;
}

void Compiler::expressionStatement() {
    if (match(TokenType::Output)) {
        printStatement();
    } else if (match(TokenType::Declare)) {
        parseDeclareStatement();
    } else if (currentToken.type == TokenType::Identifier &&
               (peekToken.type == TokenType::Assignment ||
                peekToken.type == TokenType::Lsqrbracket)) {
        parseAssignmentStatement();
    } else if (currentToken.type == TokenType::Newline) {
        advance();
    } else if (currentToken.type == TokenType::Input) {
        parseInputStatement();

    } else {
        expression();
        consume(TokenType::Newline, "Unexpected line of expression()");
    }
    return;
}

void Compiler::parseAssignmentStatement() {
    OpCode opSet;
    bool isArray = false;
    auto identifier = currentToken;
    isArray = peekToken.type == TokenType::Lsqrbracket;
    if (checkGlobalExists()) {
        opSet = isArray ? OpCode::SetGlobalArray : OpCode::SetGlobal;
    } else if (checkLocalExists()) {
        opSet = isArray ? OpCode::SetLocalArray : OpCode::SetLocal;
    } else {
        Error.report(currentToken, "Compile",
                     "Variable " + get<string>(identifier.literal) +
                         " not declared in this scope");
    }
    if (!isArray) {
        value();
    } else {
        consume(TokenType::Lsqrbracket, "expected [ after array identifier");
        advance();
        expression();
        consume(TokenType::Rsqrbracket, "expected ] after array identifier");
        emitConstant(std::move(identifier.literal));
    }
    consume(TokenType::Assignment, "Expected <-");
    advance();
    expression();
    consume(TokenType::Newline, "Unexpected end of expression");
    emit(opSet);
    emitPop();
}

void Compiler::parseForAssignmentStatement(Token iterator) {
    auto opSet = scopeDepth == 0 ? OpCode::SetGlobal : OpCode::SetLocal;
    emitConstant(std::move(iterator.literal));
    consume(TokenType::Assignment, "Expected <- after iteratore");
    advance();
    expression();
    emit(opSet);
}

void Compiler::beginScope() { ++scopeDepth; }

void Compiler::block(TokenType type) {
    while (peekToken.type != TokenType::Eof && peekToken.type != type) {
        program();
    }
    consume(type, "Unexpected end of scope");
}

void Compiler::block(TokenType type, TokenType endBlock2) {
    while (peekToken.type != TokenType::Eof && peekToken.type != type &&
           peekToken.type != endBlock2) {
        program();
    }
    peekToken.type == type
        ? consume(type, "Unexpected end of scope, expected Endif")
        : consume(endBlock2, "Unexpected end of scope Else");
}

size_t Compiler::emitJump(OpCode opCode) {
    emit(opCode, std::byte(0xff));
    return chunk->bytecode.size();
}

void Compiler::patchJump(size_t offset) {
    const auto distance = chunk->bytecode.size() - offset;
    if (distance > std::numeric_limits<unsigned char>::max()) {
        Error.report(currentToken, "Stack overflow", "Jump block too large");
    }
    chunk->patch(offset - 1, static_cast<std::byte>(distance));
    chunkPosition.push_back(
        std::make_pair(currentToken.line, currentToken.column));
}

void Compiler::parseIfStatement() {
    advance();
    expression();
    consume(TokenType::Then, "Expected Then");
    size_t thenJump = emitJump(OpCode::JumpNE);
    emitPop();
    beginScope();
    block(TokenType::Endif, TokenType::Else);
    endScope();
    size_t elseJump = emitJump(OpCode::Jump);
    patchJump(thenJump);
    if (currentToken.type == TokenType::Else) {
        advance();
        beginScope();
        block(TokenType::Endif);
        endScope();
    }
    patchJump(elseJump);
    advance();
}

void Compiler::program() {
    switch (currentToken.type) {
    case TokenType::System:
        parseSystemStatement();
        break;
    case TokenType::Function:
        return;
    case TokenType::Call:
        parseCallStatement();
        break;
    case TokenType::Procedure:
        parseProcedureStatement();
        break;
    case TokenType::Repeat:
        parseRepeatLoopStatement();
        break;
    case TokenType::While:
        parseWhileLoopStatement();
        return;
    case TokenType::For:
        parseForLoopStatement();
        return;
    case TokenType::If:
        parseIfStatement();
        break;
    default:
        expressionStatement();
        break;
    }
}

void Compiler::endScope() {
    --scopeDepth;
    int count = 0;
    for (auto it = identifiers.begin(); it != identifiers.end(); ++it) {
        if (it->depth > scopeDepth) {
            ++count;
            emit(OpCode::PopLocal);
        } else {
            break;
        }
    }
    for (int i = 0; i < count; i++) {
        identifiers.pop_back();
    }
}

void Compiler::printStatement(void) {
    expression();
    consume(TokenType::Newline, "Unexpected end of expression");
    emit(OpCode::Output);
}

void Compiler::parseInputStatement(void) {
    OpCode opSet;
    consume(TokenType::Identifier, "Expected identifier after Input");
    if (checkGlobalExists()) {
        opSet = OpCode::SetGlobal;
    } else if (checkLocalExists()) {
        opSet = OpCode::SetLocal;
    } else {
        Error.report(currentToken, "Compiler",
                     "Identifier after Input is undefined");
    }
    emitConstant(std::move(currentToken.literal));
    emit(OpCode::Input);
    emit(opSet);
    advance();
}

void Compiler::value() {
    auto value = currentToken.literal;
    emitConstant(std::move(value));
}

bool Compiler::checkLocalExists() {
    for (auto identifier : identifiers) {
        if (identifier.name == get<string>(currentToken.literal) &&
            identifier.depth <= scopeDepth) {
            return true;
        }
    }
    return false;
}
bool Compiler::checkGlobalExists() {
    for (auto identifier : identifiers) {
        if (identifier.name == get<string>(currentToken.literal) &&
            identifier.depth == 0) {
            return true;
        }
    }
    return false;
}

void Compiler::parseArrayIdentifier(bool isArray) {
    if (!isArray)
        return;
    consume(TokenType::Lsqrbracket, "Expected [ after array identifier");
    advance();
    expression();
    consume(TokenType::Rsqrbracket, "expected [ after array index");
    if (currentToken.type == TokenType::Rsqrbracket) {
        return;
    } else {
        throw std::runtime_error("Can't :()");
    }
}

void Compiler::resolver() {
    OpCode opGet;
    bool isArrayt = peekToken.type == TokenType::Lsqrbracket;
    auto identifier = currentToken;
    if (checkGlobalExists()) {
        opGet = isArrayt ? OpCode::GetGlobalArray : OpCode::GetGlobal;
    } else if (checkLocalExists()) {
        opGet = isArrayt ? OpCode::GetLocalArray : OpCode::GetLocal;
    } else {
        Error.report(currentToken, "Compile",
                     "Variable " + get<string>(currentToken.literal) +
                         " not declared in this scope");
    }

    if (!isArrayt) {
        value();
        emit(opGet);
        return;
    }
    parseArrayIdentifier(isArrayt);
    emitConstant(std::move(identifier.literal));
    emit(opGet);
    return;
}

void Compiler::unary() {
    auto op = currentToken;
    advance();
    parsePrecedence(Precedence::Unary);
    switch (op.type) {
    case TokenType::Minus:
        emit(OpCode::Negate);
        break;
    case TokenType::Not:
        emit(OpCode::Not);
        break;

    case TokenType::Sin:
        emitConstant(builtintype::Sin);
        emit(OpCode::Builtin);
        break;
    case TokenType::Cos:
        emitConstant(builtintype::Cos);
        emit(OpCode::Builtin);
        break;
    case TokenType::Tan:
        emitConstant(builtintype::Tan);
        emit(OpCode::Builtin);
        break;
    case TokenType::Abs:
        emitConstant(builtintype::Abs);
        emit(OpCode::Builtin);
        break;
    case TokenType::Sqrt:
        emitConstant(builtintype::Sqrt);
        emit(OpCode::Builtin);
        break;
    case TokenType::IntCast:
        emitConstant(builtintype::IntegerCast);
        emit(OpCode::Builtin);
        break;
    case TokenType::RealCast:
        emitConstant(builtintype::RealCast);
        emit(OpCode::Builtin);
        break;
    case TokenType::StringCast:
        emitConstant(builtintype::StringCast);
        emit(OpCode::Builtin);
        break;
    case TokenType::Reverse:
        emitConstant(builtintype::Reverse);
        emit(OpCode::Builtin);
        break;
    case TokenType::Length:
        emitConstant(builtintype::Length);
        emit(OpCode::Builtin);
        break;
    default:
        return;
    }
}

void Compiler::binary() {
    auto tok = currentToken;
    Precedence pr = lookupPrecedence(tok.type);
    advance();
    parsePrecedence(pr);
    switch (tok.type) {
    case TokenType::Not_equals:
        emit(OpCode::NotEqual);
        break;
    case TokenType::Equals:
        emit(OpCode::Equal);
        break;
    case TokenType::Greater:
        emit(OpCode::Greater);
        break;
    case TokenType::Lesser:
        emit(OpCode::Lesser);
        break;
    case TokenType::Greater_equal:
        emit(OpCode::GreaterEqual);
        break;
    case TokenType::Lesser_equal:
        emit(OpCode::LesserEqual);
        break;
    case TokenType::Plus:
        emit(OpCode::Add);
        break;
    case TokenType::Minus:
        emit(OpCode::Subtract);
        break;
    case TokenType::Star:
        emit(OpCode::Multiply);
        break;
    case TokenType::Slash:
        emit(OpCode::Divide);
        break;
    case TokenType::Mod:
        emit(OpCode::Mod);
        break;
    case TokenType::Div:
        emit(OpCode::Div);
        break;
    case TokenType::Ampersand:
        emit(OpCode::Concatenate);
        break;
    case TokenType::And:
        emit(OpCode::And);
        break;
    case TokenType::Or:
        emit(OpCode::Or);
        break;
    default:
        return;
    }
}

void Compiler::consume(TokenType type, string msg) {
    if (peekToken.type == type) {
        advance();
        return;
    }
    Error.report(currentToken, "Compiler", msg);
}

void Compiler::advanceIf(TokenType type, string msg) {
    if (currentToken.type == type) {
        advance();
        return;
    }
    Error.report(currentToken, "Compiler", msg);
}

void Compiler::grouping() {
    advance();
    expression();
    advance();
}

void Compiler::parsePrecedence(Precedence precedence) {
    if (lookupUnary(currentToken.type))
        return;
    while (precedence <= lookupPrecedence(peekToken.type)) {
        advance();
        lookupBinary(currentToken.type);
    }
}

void Compiler::parseDeclareStatement() {
    std::vector<Token> declareIdentifiers;
    while (true) {
        if (currentToken.type != TokenType::Identifier) {
            std::stringstream ss;
            ss << "Expected identifier instead of '" << currentToken.literal
               << "'";
            Error.report(currentToken, "Compile", ss.str());
        }
        declareIdentifiers.emplace_back(currentToken);
        advance();
        if (currentToken.type == TokenType::Comma)
            advance();
        else
            break;
    }
    if (currentToken.type != TokenType::Colon) {
        Error.report(currentToken, "Compile", "Expected Colon (:)");
    }
    advance();
    switch (currentToken.type) {
    case TokenType::Integer_t:
        declareVariables(declareIdentifiers, TokenType::Integer, true, false);
        break;
    case TokenType::String_t:
        declareVariables(declareIdentifiers, TokenType::String, true, false);
        break;
    case TokenType::Boolean_t:
        declareVariables(declareIdentifiers, TokenType::Boolean, true, false);
        break;
    case TokenType::Real_t:
        declareVariables(declareIdentifiers, TokenType::Real, true, false);
        break;
    case TokenType::Char_t:
        declareVariables(declareIdentifiers, TokenType::Char, true, false);
        break;
    case TokenType::Array: {
        consume(TokenType::Lsqrbracket, "Expected [ after ARRAY");
        TokenType arrayType = getArrayDeclarationType();
        advance();
        expression();
        consume(TokenType::Colon, "Expected : after expression");
        advance();
        expression();
        consume(TokenType::Rsqrbracket, "Expected ] after expression");
        consume(TokenType::Of, "Expected OF after ]");
        advance();
        switch (arrayType) {
        // single variable declaration for the time being;
        case TokenType::Integer_t:
            declareVariables(declareIdentifiers, TokenType::Integer, true,
                             true);
            break;
        case TokenType::String_t:
            declareVariables(declareIdentifiers, TokenType::String, true, true);
            break;
        case TokenType::Boolean_t:
            declareVariables(declareIdentifiers, TokenType::Boolean, true,
                             true);
            break;
        case TokenType::Real_t:
            declareVariables(declareIdentifiers, TokenType::Real, true, true);
            break;
        case TokenType::Char_t:
            declareVariables(declareIdentifiers, TokenType::Char, true, true);
            break;
        default:
            Error.report(currentToken, "Compile",
                         "Unexpected type in array declaration");
        }
        break;
    }
    default:
        Error.report(currentToken, "Compile", "Unexpected Type");
    }
}

void Compiler::declareVariables(std::vector<Token> declareIdentifiers,
                                TokenType type, bool newline, bool isArray) {
    for (auto declareIdentifier : declareIdentifiers) {
        string identifierName = get<string>(declareIdentifier.literal);
        Identifier newidentifier = Identifier(identifierName, scopeDepth);
        scopeDepth == 0 ? globalsType[identifierName] = type
                        : localsType[identifierName] = type;
        identifiers.emplace_back(newidentifier);
        emitConstant(std::move(declareIdentifier.literal));
        if (!isArray) {
            scopeDepth == 0 ? emit(OpCode::DefineGlobal)
                            : emit(OpCode::DefineLocal);
        } else {
            scopeDepth == 0 ? emit(OpCode::DefineGlobalArray)
                            : emit(OpCode::DefineLocalArray);
        }
    }
    if (newline) {
        consume(TokenType::Newline, "Unexpected end of declareStatement");
    }
}

void Compiler::emitLoop(size_t jump) {
    emit(OpCode::Loop);
    size_t offset = chunk->bytecode.size() - jump + 1;
    if (offset > std::numeric_limits<unsigned char>::max()) {
        Error.report(currentToken, "Stack overflow", "Loop body too large");
    }
    chunk->writeByte(static_cast<std::byte>(offset));
}

void Compiler::parseForLoopStatement() {
    advance();
    Value i1 = currentToken.literal;
    auto i3 = currentToken;
    auto opGet = scopeDepth == 0 ? OpCode::GetGlobal : OpCode::GetLocal;
    auto opSet = scopeDepth == 0 ? OpCode::SetGlobal : OpCode::SetLocal;
    parseForAssignmentStatement(i3);
    consume(TokenType::To, "Expected To after expression");
    advance();
    size_t loopJump = chunk->bytecode.size();
    emitPop();
    expression();
    advance();
    emitConstant(std::move(i1));
    emit(opGet);
    emit(OpCode::GreaterEqual);
    size_t jumpne = emitJump(OpCode::JumpNE);
    emitPop();
    block(TokenType::Next);
    consume(TokenType::Identifier, "Expected identifier after i");
    emitConstant(std::move(currentToken.literal));
    emit(OpCode::IncrementGlobal);
    emitLoop(loopJump); // goto loopJump
    patchJump(jumpne);  // from jumpne to emitPop
    emitPop();
    advance();
}

void Compiler::parseRepeatLoopStatement() {
    advance();
    size_t loopJump = chunk->bytecode.size();
    block(TokenType::Until);
    advance();
    expression();
    size_t jumpne = emitJump(OpCode::JumpNE);
    emitPop();
    emitLoop(loopJump);
    patchJump(jumpne);
    emitPop();
    advance();
}

void Compiler::parseWhileLoopStatement() {
    size_t loop = chunk->bytecode.size();
    advance();
    expression();
    consume(TokenType::Do, "Expected Do after expression");
    size_t jumpne = emitJump(OpCode::JumpNE);
    emitPop();
    block(TokenType::Endwhile);
    emitLoop(loop);
    patchJump(jumpne);
    emitPop();
    advance();
}

const std::unordered_map<TokenType, Precedence> Compiler::precedenceMap = {
    {TokenType::Integer, Precedence::None},
    {TokenType::Real, Precedence::None},
    {TokenType::Char, Precedence::None},
    {TokenType::String, Precedence::None},
    {TokenType::Date, Precedence::None},
    {TokenType::Boolean, Precedence::None},

    {TokenType::Rparen, Precedence::Newline},
    {TokenType::Lparen, Precedence::Call},

    {TokenType::Plus, Precedence::Term},
    {TokenType::Minus, Precedence::Term},
    {TokenType::Star, Precedence::Factor},
    {TokenType::Slash, Precedence::Factor},
    {TokenType::Div, Precedence::Factor},
    {TokenType::Mod, Precedence::Factor},

    {TokenType::Ampersand, Precedence::Factor},

    {TokenType::Assignment, Precedence::Newline},
    {TokenType::Comma, Precedence::Newline},

    {TokenType::Equals, Precedence::Equality},
    {TokenType::Not_equals, Precedence::Equality},
    {TokenType::Greater, Precedence::Comparison},
    {TokenType::Lesser, Precedence::Comparison},
    {TokenType::Greater_equal, Precedence::Comparison},
    {TokenType::Lesser_equal, Precedence::Comparison},

    {TokenType::And, Precedence::And},
    {TokenType::Or, Precedence::Or},
    {TokenType::Not, Precedence::Unary},
    {TokenType::Sin, Precedence::Unary},
    {TokenType::Cos, Precedence::Unary},
    {TokenType::Tan, Precedence::Unary},
    {TokenType::Abs, Precedence::Unary},
    {TokenType::Sqrt, Precedence::Unary},
    {TokenType::IntCast, Precedence::Unary},
    {TokenType::RealCast, Precedence::Unary},
    {TokenType::StringCast, Precedence::Unary},

    {TokenType::Identifier, Precedence::None},

    {TokenType::Lsqrbracket, Precedence::Call},
    {TokenType::Rsqrbracket, Precedence::Newline},

    {TokenType::Caret, Precedence::Unary},
    {TokenType::Period, Precedence::Call},

    {TokenType::Byref, Precedence::Call},
    {TokenType::Byval, Precedence::Call},
    {TokenType::Call, Precedence::None},

    {TokenType::Newline, Precedence::Newline},
    {TokenType::Colon, Precedence::Newline},
    {TokenType::Rsqrbracket, Precedence::Newline},
    {TokenType::To, Precedence::Newline},
    {TokenType::Of, Precedence::Newline},
    {TokenType::Do, Precedence::Newline},
    {TokenType::Then, Precedence::Newline},
    {TokenType::Eof, Precedence::Newline}};
