# Minimal bytecode compiler for pseudocode

# Pre-requisites
- Any C++ compiler

# Installation guide for linux
```bash
git clone https://github.com/MustafaAamir/Pseudocode-Compiler
cd Pseudocode-Compiler
bash build.sh
./pscompiler --help
```

# Optional
- To enable interactive REPL history, install the GNU-rlwrap utility
```bash
dnf install rlwrap

//run pscompiler with rlwrap
rlwrap ./pscompiler --help
```

# Program Features
- REPL
- File execution
- Benchmarking with `--benchmark` flag
- Tokenization with `--lexer` flag
- CLI interface

# Language features
- I/O statements `INPUT Identifier`, `OUTPUT Expression`
- Conditional Expressions `IF...THEN...ELSE`
- Count-controlled loop  `FOR...TO...NEXT`
- Pre-condition loop `WHILE...DO...ENDWHILE`
- Post-condition loop `REPEAT...UNTIL`
- Dynamic typing

# Pending features
- Arrays
- Hashmaps
- Procedures & Functions
- Static type checking
- Type conversions
- Referencing & dereferencing

# Problems
- Insufficient support for `FOR` loops. Increment value is +1 by default, so only loops going in the positive direction are allowed






