#include "../common.h"
#include "run.h"

static bool cmdHandler(std::string cmd);
static void handleBlock(std::string &line) {
    for (const std::string str : {"IF", "FOR", "REPEAT", "WHILE"}) {
        const std::unordered_map<std::string, std::string> endMap = {
            {"IF", "ENDIF"},
            {"FOR", "NEXT"},
            {"REPEAT", "UNTIL"},
            {"WHILE", "ENDWHILE"},
        };
        auto it = endMap.find(str);
        if (it == endMap.end()) {
            throw std::runtime_error("My bad");
        }
        int idt = 0;
        std::string tmp;

        if (trim(line).substr(0, str.length()) == str) {
           ++idt;
            while (idt != 0) {
                printColor(FG_RED, "....", false);
                line += "\n";
                std::getline(std::cin, tmp);
                line += tmp;
                if (trim(tmp).substr(0, it->second.length()) == it->second) {
                    --idt;
                } else if (trim(tmp).substr(0, str.length()) == str) {
                    ++idt;
                }
            }
        }
    }
}
void repl(bool bench) {
    int idx = 0;
    int input = false;
    VirtualMachine vm;
    std::string tmp;
    while (true) {
        std::string line;
        printColor(FG_RED, "  > ", false);
        std::getline(std::cin, line);
        if (cmdHandler(line)) {
            continue;
        }
        handleBlock(line);
        line += "\n";
        try {
            if (bench) {
                START_TIMER;
                vm.interpret(line, idx);
                STOP_TIMER;
            } else {
                vm.interpret(line, idx);
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        ++idx;
    }
}

void repLexer(bool bench) {
    while (true) {
        std::cout << ">> ";
        std::string line;
        std::getline(std::cin, line);
        cmdHandler(line);
        line += '\n';
        Lexer lexer;
        std::cout << "Lexer:" << std::endl;
        if (bench) {
            START_TIMER;
            lexer.initLexer(&line);
            auto TokenList = lexer.makeTokens(true);
            STOP_TIMER;
        } else {
            lexer.initLexer(&line);
            auto TokenList = lexer.makeTokens(true);
        }
    };
}

void runFile(std::string fileName, bool bench, bool lexer) {
    std::ifstream file;
    try {
        file.open(fileName);
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
    std::string input = "";
    std::string line;
    while (std::getline(file, line)) {
       input += line + "\n";
    }
    //std::cout << input << std::endl;
    if (lexer) {
        Lexer lexer;
        if (bench) {
            START_TIMER;
            lexer.initLexer(&input);
            auto TokenList = lexer.makeTokens(true);
            STOP_TIMER;
        } else {
            lexer.initLexer(&input);
            auto TokenList = lexer.makeTokens(true);
        }
        return;
    }
    VirtualMachine vm;
    try {
        if (bench) {
            START_TIMER;
            vm.interpret(input, 0);
            STOP_TIMER;
        } else {
            vm.interpret(input, 0);
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}


static bool cmdHandler(std::string line) {
    if (line == "clear") {
#if defined(__linux__)
        system("clear");
#elif _WIN32
        system("cls");
#endif
    }
    if (line == "quit") {
        exit(0);
    }
    return false;
}

void printColor(AnsiCode color, std::string string, bool newline) {
    std::cout << Modifier(color) << string << Modifier(AnsiCode::FG_DEFAULT);
    if (newline) {
        std::cout << std::endl;
    }
}


