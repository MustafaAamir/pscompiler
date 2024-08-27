#pragma once
#include "../common.h"
#include "../tokens/tokens.h"
#include "../error/error.h"

 enum class OpCode : unsigned char {
    Constant, Pop, PopLocal,

    DefineGlobal, DefineGlobalArray,

    SetGlobal, SetGlobalArray,

    GetGlobal, GetGlobalArray,

    DefineLocal, DefineLocalArray,

    SetLocal, SetLocalArray,

    GetLocal, GetLocalArray,

    Equal, NotEqual, Greater, GreaterEqual, Lesser, LesserEqual, Add,
    Subtract, Divide, Multiply, Negate, Mod, Div, Concatenate,

    And, Or, Not, Output, Input, Jump, JumpNE, Loop,
    Builtin,

    Call, EndFunction,
    IncrementGlobal, Return
};


static const std::unordered_map<OpCode, std::string> OpCodeMap = {
    {OpCode::Constant, "Constant"},
    {OpCode::IncrementGlobal, "IncrementGlobal"},
    {OpCode::Pop, "Pop"},

    {OpCode::DefineGlobal, "DefineGlobal"},
    {OpCode::DefineGlobalArray, "DefineGlobalArray"},

    {OpCode::DefineLocal, "DefineLocal"},
    {OpCode::DefineLocalArray, "DefineLocalArray"},

    {OpCode::SetGlobal, "SetGlobal"},
    {OpCode::SetGlobalArray, "SetGlobalArray"},

    {OpCode::GetGlobal, "GetGlobal"},
    {OpCode::GetGlobalArray, "GetGlobalArray"},

    {OpCode::SetLocal, "SetLocal"},
    {OpCode::SetLocalArray, "SetLocalArray"},

    {OpCode::GetLocal, "GetLocal"},
    {OpCode::GetLocalArray, "GetLocalArray"},

    {OpCode::Equal, "Equal"},
    {OpCode::NotEqual, "NotEqual"},
    {OpCode::Greater, "Greater"},
    {OpCode::GreaterEqual, "GreaterEqual"},
    {OpCode::Lesser, "Lesser"},
    {OpCode::LesserEqual, "LesserEqual"},
    {OpCode::Add, "Add"},
    {OpCode::Subtract, "Subtract"},
    {OpCode::Divide, "Divide"},
    {OpCode::Multiply, "Multiply"},
    {OpCode::Negate, "Negate"},
    {OpCode::Mod, "Mod"},
    {OpCode::Builtin, "Builtin"},
    {OpCode::Div, "Div"},
    {OpCode::Concatenate, "Concatenate"},
    {OpCode::Not, "Not"},
    {OpCode::And, "And"},
    {OpCode::Or, "Or"},
    {OpCode::Output, "Output"},
    {OpCode::Input, "Input"},
    {OpCode::Jump, "Jump"},
    {OpCode::PopLocal, "PopLocal"},
    {OpCode::JumpNE, "JumpNE"},
    {OpCode::Call, "Call"},
    {OpCode::EndFunction, "EndFunction"},
    {OpCode::Return, "Return"},
};


class Chunk {
    private:
        size_t offset {0};
        ErrorReporter Error {};
        void disassembleInstruction();
    public:
        vector<std::pair<int, int>> poscode;
        void disassembleChunk(const std::string msg);
        std::vector<Value> constantPool {};
        std::vector<std::byte> bytecode {};
        std::byte read(size_t offset) const { return bytecode[offset]; }
        void writeByte(std::byte byte) { bytecode.push_back(byte); }
        void writeChunk(OpCode opCode);
        void patch(size_t offset, std::byte byte) { bytecode[offset] = byte; }
        Value getConstant(size_t idx) { return constantPool[idx]; } //modified
        size_t addConstant(Value &&value); // modified
};

