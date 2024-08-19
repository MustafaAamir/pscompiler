#pragma once
#include "../common.h"
#include "../lexer/lexer.h"
#include "../vm/vm.h"
#include "../error/error.h"
#include <unistd.h>
#include <fstream>
#include <chrono>


#define START_TIMER auto start = std::chrono::steady_clock::now()
#define STOP_TIMER auto stop = std::chrono::steady_clock::now(); \
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); \
            std::cout << "\n\nFinished in " << ms << " ms" << std::endl


void repl(bool bench);
void repLexer(bool bench);
void runFile(std::string fileName, bool bench, bool lexer);
void printColor(AnsiCode color, std::string msg, bool newline);
