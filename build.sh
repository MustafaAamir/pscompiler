#!/usr/bin/bash

g++ error/error.cpp main.cpp vm/vm.cpp chunk/chunk.cpp compiler/compiler.cpp run/run.cpp tests/tests.cpp  lexer/lexer.cpp tokens/tokens.cpp -Ofast -march=native -o pscompiler
