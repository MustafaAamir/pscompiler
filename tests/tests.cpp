#include "../common.h"
#include "../vm/vm.h"
#include "../error/error.h"
#include "../run/run.h"
#include "tests.h"

void invokeTests(bool benchmark, bool lexer) {
    const vector<string> tests = {
        {"OUTPUT 1\n\n\n\n"},
        {"OUTPUT 10 + 5"},
        {"OUTPUT 10 - 5"},
        {"OUTPUT 10 * 5"},
        {"OUTPUT 10 / 5"},
        {"OUTPUT 10 DIV 5"},
        {"OUTPUT 10 MOD 5"},
        {"OUTPUT \"10\" & \"5\""},
        {"OUTPUT \"10\" & \'5\'"},
        {"OUTPUT '1' & \"5\""},
        {"OUTPUT 10 > 5"},
        {"OUTPUT 10 < 5"},
        {"OUTPUT 10 <= 5"},
        {"OUTPUT 10 >= 5"},
        {"OUTPUT 10 = 5"},
        {"OUTPUT 10 <> 5"},
        {"OUTPUT -10 * 5"},
        {"OUTPUT -10 * 5\nOUTPUT 1 - 2"},
        {"DECLARE X : INTEGER\nX <- 123\nOUTPUT X"},
        {"DECLARE X : INTEGER\nX <- 123\nX <- 890\nX <- 8234"},
        {"DECLARE X : INTEGER\nX <- (12 * 12 - (9 - 2))\nOUTPUT X"},
        {"DECLARE X, Y : INTEGER\nX <- (12 * 12 - (9 - 2))\nY <- X"},
        {"DECLARE X, Y, Z : INTEGER\nX <- (12 * 12 - (9 - 2))\nY <- X"},
        {"DECLARE Y : STRING\nY <-\"foobar\"\nOUTPUT Y"},
        {"DECLARE Y : STRING\nY <-\"foobar\" & \" Boobar\"\nOUTPUT Y"},
        {"DECLARE Y : BOOLEAN\nY <- TRUE\nOUTPUT Y"},
        {"DECLARE Y : BOOLEAN\nY <- FALSE = TRUE\nOUTPUT Y"},
        {"DECLARE Y : BOOLEAN\nDECLARE Z : BOOLEAN\nY <- FALSE = TRUE\nZ <- Y\nOUTPUT Z"},
        {"DECLARE Y : BOOLEAN\nY <- FALSE\nY <- Y\nOUTPUT Y"},
        {"OUTPUT 5 > 10 = 10 < 5"},
        {"OUTPUT 5 >= 10 = 10 <= 5"},
        {"OUTPUT 5 >= 10 = 11 < 6"},
        {"OUTPUT ((0) + -1)"},
        {"OUTPUT NOT TRUE"},
        {"OUTPUT TRUE AND NOT FALSE"},
        {"OUTPUT 123 > 91 AND 139 > 123"}
    };

    const vector<string> iotests = {
        {"INPUT int"},
        {"INPUT char"},
        {"INPUT real"},
        {"INPUT string"},
        {"INPUT boolean"},
    };

    const vector<string> arrTests = {
        {"arr[1] <- 3231\nOUTPUT arr[1]\nOUTPUT arr[1]\narr[2] <- 123\nOUTPUT arr[2]\n"},
    };

    int idx = 1;
    string declareAll = "DECLARE int : INTEGER\nDECLARE real : REAL\nDECLARE string : STRING\nDECLARE char : CHAR\nDECLARE boolean : BOOLEAN\n";
    string declareIntArray = "DECLARE arr : ARRAY[1:10] OF INTEGER\n";
    VirtualMachine vm;
    for (auto test : arrTests) {
        auto testn = declareIntArray + test + "\n";
        VirtualMachine vm;
        try {
            if (lexer) {
                Lexer lexer;
                cout << "Lexer:" << endl;
                if (benchmark) {
                    START_TIMER;
                    lexer.initLexer(&testn);
                    auto TokenList = lexer.makeTokens(true);
                    STOP_TIMER;
                } else {
                    lexer.initLexer(&testn);
                    auto TokenList = lexer.makeTokens(true);
                }
            } else {
                if (benchmark) {
                    START_TIMER;
                    vm.interpret(testn);
                    STOP_TIMER;
                } else {
                    vm.interpret(testn);
                }
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        cout << Modifier(AnsiCode::FG_GREEN) << "{\n" << trim(test) << "\n}\t" << " passed!\n"
            << Modifier(AnsiCode::FG_BLUE) << "(" << idx << "/" << tests.size()
            << ") " << "test(s) Passed!" << Modifier(AnsiCode::FG_DEFAULT) << endl;
        ++idx;
    }
}
