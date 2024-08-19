#include "chunk.h"
#include "../tokens/tokens.h"
#include "../common.h"
#include <cstdio>

void Chunk::writeChunk(OpCode opCode) {
    writeByte(static_cast<std::byte>(opCode));
}

size_t Chunk::addConstant(Value &&Value) {
    constantPool.emplace_back(std::move(Value));
    return constantPool.size() - 1;
}

void Chunk::disassembleChunk(const std::string msg) {
    std::cout << "\n      " << Modifier(AnsiCode::FG_BMAGENTA) << msg << Modifier(AnsiCode::FG_DEFAULT)<<std::endl;
    offset = 0;
    while (offset < bytecode.size()) {
        disassembleInstruction();
    }
    std::cout << "      " << Modifier(AnsiCode::FG_BMAGENTA)<< "END" << Modifier(AnsiCode::FG_DEFAULT) << "\n" << std::endl;
}

void Chunk::disassembleInstruction() {
    std::cout << Modifier(AnsiCode::FG_GREEN);
    printf("%02zx ", offset);
    std::cout << Modifier(AnsiCode::FG_DEFAULT);
    auto it = OpCodeMap.find(static_cast<OpCode>(read(offset++)));
    if (it == OpCodeMap.end()) {
        Error.report(0, 0, "Compiler", "If you encounter this error, create a minimal, reproducible example and pull an issue on www.github.com/MustafaAamir/Pseudoish");
    }
    switch (it->first) {
        case (OpCode::Constant): {
            const auto idx = static_cast<uint16_t>(read(offset++));
            const auto idx1 = static_cast<uint16_t>(read(offset++));
            const auto newidx = static_cast<size_t>((idx << 8) & 0xff00) | (idx1 & 0xff);
            const auto value = getConstant(newidx);
            if (const auto string = std::get_if<std::string>(&value)) {
                std::cout << Modifier(AnsiCode::FG_BMAGENTA);
                printf("Constant[%04zx] -> ", newidx);
                std::cout << Modifier(AnsiCode::FG_BBLUE) << "\""
                    << *string << "\"" << std::endl << Modifier(AnsiCode::FG_DEFAULT);
            } else if (const auto integer = std::get_if<i64>(&value)) {
                std::cout << Modifier(AnsiCode::FG_BMAGENTA);
                printf("Constant[%04zx] -> ", newidx);
                std::cout << Modifier(AnsiCode::FG_BBLUE) << *integer << std::endl << Modifier(AnsiCode::FG_BBLUE);
            } else if (const auto real = std::get_if<double>(&value)) {
                std::cout << Modifier(AnsiCode::FG_BMAGENTA);
                printf("Constant[%04zx] -> ", newidx);
                std::cout << Modifier(AnsiCode::FG_BBLUE) << *real<< std::endl << Modifier(AnsiCode::FG_BBLUE);
            } else if (const auto ch = std::get_if<char>(&value)) {
                std::cout << Modifier(AnsiCode::FG_BMAGENTA);
                printf("Constant[%04zx] -> ", newidx);
                std::cout << Modifier(AnsiCode::FG_BBLUE) << *ch << std::endl << Modifier(AnsiCode::FG_BBLUE);
            } else if (const auto bl = std::get_if<bool>(&value)) {
                std::cout << Modifier(AnsiCode::FG_BMAGENTA);
                printf("Constant[%04zx] -> ", newidx);
                std::cout << Modifier(AnsiCode::FG_BBLUE) << (*bl ? "TRUE" : "FALSE") << std::endl << Modifier(AnsiCode::FG_BBLUE);
            }
            break;
                }
        case (OpCode::Call): {
            const auto idx = static_cast<uint16_t>(read(offset++));
            const auto idx1 = static_cast<uint16_t>(read(offset++));
            const auto newidx = static_cast<size_t>((idx << 8) & 0xff00) | (idx1 & 0xff);
            const auto value = getConstant(newidx);
            cout << "Call jmpdst " << value << " to "<< (offset - get<i64>(value)) << " from " << offset << endl;
            break;
                }
        case (OpCode::Jump):
        case (OpCode::JumpNE): {
            const auto distance = static_cast<size_t>(read(offset++));
            std::cout << Modifier(AnsiCode::FG_BBLUE);
            printf("%s distance %02zx  -> to %02zx\n", it->second.c_str(), distance, offset + distance);
            std::cout << Modifier(AnsiCode::FG_DEFAULT);
            break;
        }
        case (OpCode::Loop): {
            const auto distance = static_cast<size_t>(read(offset++));
            std::cout << Modifier(AnsiCode::FG_BBLUE);
            printf("%s %02zx    # ->%02zx\n", it->second.c_str(),  distance, offset - distance);
            std::cout << Modifier(AnsiCode::FG_DEFAULT);
            break;
        }
        default: {
            std::cout << Modifier(AnsiCode::FG_BBLUE);
            printf("%s\n", it->second.c_str());
            std::cout << Modifier(AnsiCode::FG_DEFAULT);
            break;
        }
    }
}


