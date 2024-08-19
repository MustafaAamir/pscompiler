#include "common.h"
#include "tests/tests.h"
#include "run/run.h"

using std::pair;
using std::string;
using std::vector;

void printHelp() {
    std::cout << "Usage: interpreter [options] [filename]\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help       Show help message\n"
              << "  -b, --benchmark  Time the interpreter execution in ms\n"
              << "  -l, --lexer      Tokenize REPL prompts\n"
              << "  -t, --test       Run tests defined in tests/tests.cpp\n"
              << "\n"
              << "If no options or filename is provided, starts a REPL.\n";
}

const vector<pair<string, string>> argpair = {
    {std::make_pair("-h", "--help")},
    {std::make_pair("-l", "--lexer")},
    {std::make_pair("-b", "--benchmark")},
    {std::make_pair("-t", "--test")}
};


int main(int argc, char *argv[]) {
    bool benchmark = false;
    bool lexer     = false;
    bool testing = false;
    if (argc == 1) {
        printColor(AnsiCode::FG_BBLACK, "IGCSE/A-Level Pseudocode Compiler", true);
        repl(benchmark);
    }

    else if (argc == 2) {
        if (string(argv[1]) == "-h" || string(argv[1]) == "--help") {
            printHelp();
        } else if (string(argv[1]) == "--test" || string(argv[1]) == "-t") {
            invokeTests(benchmark, lexer);
        } else if (string(argv[1]) == "--lexer" || string(argv[1]) == "-l") {
            repLexer(true);
        } else if (string(argv[1]) == "--benchmark" || string(argv[1]) == "-b") {
            benchmark = true;
            repl(benchmark);
        } else {
            runFile(string(argv[1]), benchmark, lexer);
        }
    }
    else if (argc == 3) {
        auto a1 = string(argv[1]);
        auto a2 = string(argv[2]);
        try {
            for (const auto arg : argpair) {
                if (a2 == arg.first || a2 == arg.second) {
                    throw std::invalid_argument("Expected filename instead of " + a2);
                }
            }
            if (a1 == "-h" || a1 == "--help" || a2 == "-h" || a2 == "--help") {
                throw std::invalid_argument("[-h, --help] is a standalone argument");
            } else if (a1 == "-l" || a1 == "--lexer") {
                lexer = true;
            } else if (a1 == "-b" || a1 == "--benchmark") {
                benchmark= true;
            } else {
                throw std::invalid_argument("Invalid Option");
            }
        } catch (const std::invalid_argument &ex) {
            printColor(FG_RED, "Argument Error: ", false);
            std::cout << ex.what() << std::endl << std::endl << std::endl;
            printHelp();
            exit(0);
        }
        runFile(a2, benchmark, lexer);
    }

    return 0;
}

