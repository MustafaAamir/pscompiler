all:
	g++ error/error.cpp main.cpp vm/vm.cpp chunk/chunk.cpp compiler/compiler.cpp run/run.cpp tests/tests.cpp  lexer/lexer.cpp tokens/tokens.cpp -Ofast -march=native -o pscompiler

debug:
	g++ error/error.cpp main.cpp vm/vm.cpp chunk/chunk.cpp compiler/compiler.cpp run/run.cpp tests/tests.cpp  lexer/lexer.cpp tokens/tokens.cpp -Ofast -march=native -g -o pscompilerdebug

nofast:
	g++ error/error.cpp main.cpp vm/vm.cpp chunk/chunk.cpp compiler/compiler.cpp run/run.cpp tests/tests.cpp  lexer/lexer.cpp tokens/tokens.cpp -O0 -o pscompiler

nofastdebug:
	g++ error/error.cpp main.cpp vm/vm.cpp chunk/chunk.cpp compiler/compiler.cpp run/run.cpp tests/tests.cpp  lexer/lexer.cpp tokens/tokens.cpp -O0 -g -o pscompilerdebug

