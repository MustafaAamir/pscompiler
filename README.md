# Minimal bytecode compiler for pseudocode

# Pre-requisites
- Any C++ compiler

# Installation guide for linux
```bash
git clone https://github.com/MustafaAamir/pscompiler
cd pscompiler
make
./pscompiler --help
```
# Optional
### To enable interactive REPL history, install the GNU-rlwrap utility
```bash
dnf install rlwrap

//run pscompiler with rlwrap
rlwrap ./pscompiler --help
```

### To enable a minimal GUI interface
Open pscompiler/error/error.h in your editor

```bash
vi error/error.h
```
set gui to `true` to disable ANSI colors
```c++
const bool gui = true;
```
compile the project
```bash
make
./pscompiler --help
```
install GUI dependencies

```bash
pip install PyQt6
python gui.py
```
- pscompiler is primarily a CLI application.

### To enable lowercase keywords
Open pscompiler/lexer/lexer.h

```bash
vi lexer/lexer.h
```

set `lowercase` to `true`
```c++
const bool lowercase = true;
```

# Program Features
- REPL
- File execution
- Benchmarking with `--benchmark` flag
- Tokenization with `--lexer` flag
- CLI interface
- Minimal GUI

# Language features
- I/O statements `INPUT Identifier`, `OUTPUT Expression`
- Conditional Expressions `IF...THEN...ELSE`
```
IF <Condition> THEN
    (Statement*)
ELSE
    (Statement*)
ENDIF
```
- Count-controlled loop  `FOR...TO...NEXT`
```
DECLARE <Identifier> : INTEGER
FOR <Identifier> <- <expression> TO <expression>
  (Statement*)
NEXT <Identifier>
```
- Pre-condition loop `WHILE...DO...ENDWHILE`
```
WHILE <Condition> DO
    (Statement)*
ENDWHILE
```
- Post-condition loop `REPEAT...UNTIL`
```
REPEAT
   (Statement)*
UNTIL <Condition>
```
- Dynamic typing
- type-convertions `INTEGER_CAST, REAL_CAST, STRING_CAST`
```
INTEGER_CAST([INTEGER | REAL])
REAL_CAST([INTEGER | REAL])
STRING_CAST([INTEGER | REAL | CHAR | STRING])
```
- builtin functions `sin, cos, tan, abs, sqrt` (radians)
- 1-D Arrays
```
DECLARE <Identifier> : ARRAY[<LB:UB>] OF <DataType>
```
- non-parameterized procedures
```
PROCEDURE <Identifier>()
    (Statement)*
ENDPROCEDURE
```

# Pending features
- Hashmaps: (trivial to implement if std::unorderd_map<Value, Value> is utilized)
- Parameterized procedures & functions: when called CALL Add(1, 2) declares parameters as local variables, pops values off of the value stack and sets them to the respective parameters.
- Recursion
- Static type checking
- Referencing & dereferencing ^Ptr, Ptr^: Ptr of type uint16_t used as index in Chunk::constantPool
- File handling: READFILE, WRITEFILE, OPENFILE, CLOSEFILE

# Problems
- Insufficient support for FOR loops. Increment value is +1 by default, therefore only loops going in the positive direction are allowed. Loops in the negative direction could be implemented by adding an addtional LT & GT comparison before the loop commences, and emiting the appropriate comparison operator (LesserEqual || GreaterEqual) based on the result.
- Project architecture needs refactoring. ErrorReporter should be a seperate global entity to ensure configurability, i.e. enabling logging or lowercase lexing via command-line arguments.





