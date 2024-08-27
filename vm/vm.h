#pragma once
#include "../chunk/chunk.h"
#include "../common.h"
#include "../compiler/compiler.h"
#include <cmath>
#include <sstream>


string trim(const string& str, const string& whitespace = " \t\n.");

typedef struct ValueArray {
    vector<Value> array;
    i64 ub;
    i64 lb;
    string name;
    ValueArray() = default;
    ValueArray(vector<Value> &array, i64 ub, i64 lb, string name) :
        array(array), ub(ub), lb(lb), name(name) {}
} ValueArray;

typedef struct Function {
    string name;
    uint8_t argsCount;
    vector<std::pair<string, TokenType>> ArgTypePairVector;
    TokenType returnValue;
    vector<uint8_t> bytecode;
    Function() = default;
    Function(string name, uint8_t argsCount, vector<std::pair<string, TokenType>> ArgTypePairVector,
        TokenType returnValue, vector<uint8_t> bytecode) : name(name), argsCount(argsCount),
        ArgTypePairVector(ArgTypePairVector), bytecode(bytecode) {}
} Function;

class VirtualMachine {
    public:
        void interpret(string input, int line);
        vector<Value> valueStack {};
    private:
        ErrorReporter Error;
        uint8_t reg {0};
        void printValueStack(OpCode opCode);
        int line;
        Compiler compiler {};
        void run();
        inline Value pop();
        inline void Builtin();
        inline bool isNumber(Value v);
        inline void BinOp(Value v1, Value v2, char op);
        inline void LogicalBinOp(Value v1, Value v2, char op);
        inline void Concatenate(Value v1, Value v2);
        template<typename T> bool isType(Value v);
        std::pair<int, int> position;
        std::unique_ptr<Chunk> chunk;
        unordered_map<string, Value> globals {};
        unordered_map<string, Value> locals  {};
        unordered_map<string, std::unique_ptr<ValueArray>> valueArrayMap;
        unordered_map<string, std::unique_ptr<Function>> FunctionMap;
        size_t offset {0};
};





