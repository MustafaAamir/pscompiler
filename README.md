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
- I/O statements `input Identifier`, `output Expression`
- Conditional Expressions `if...then...else`
```
if <Condition> then
    (Statement)*
else
    (Statement)*
endif
```
- Count-controlled loop  `for...to...next`
```
declare <Identifier> : integer
for <Identifier> <- <expression> to <expression>
  (Statement)*
next <Identifier>
```
- Pre-condition loop `while...do...endwhile`
```
while <Condition> do
    (Statement)*
endwhile
```
- Post-condition loop `repeat...until`
```
repeat
   (Statement)*
until <Condition>
```
- Dynamic typing
- type-convertions `integer_cast, real_cast, string_cast`
```
integer_cast(<integer> | <real>) -> integer
real_cast(<integer>   | <real>) -> real
string_cast(<integer> | <real> | <char> | <string>) -> string
```
- 1-D Arrays
```
declare <Identifier> : array[<lb>:<ub>] of <DataType>
```
- non-parameterized procedures (experimental)
```
procedure <Identifier>()
    (Statement)*
endprocedure
```

# builtin functions
- Arithmetic:
```
sin(<integer>  | <real>) -> real
cos(<integer>  | <real>) -> real
tan(<integer>  | <real>) -> real
sqrt(<integer> | <real>) -> real
abs(<integer>  | <real>) -> integer

-- where lb <= returnvalue <= ub
random_integer(<lb>, <ub>) -> integer
random_real(<lb>, <ub>) -> real
```
- String Handling
```
mid(<string>, <startpos>, <length>) -> string
length(<string>) -> string
```

- Type-casting
```
integer_cast(<real> | <integer>) -> integer
real_cast(<real> | <integer>) -> real
string_cast(<string> | <char> | <real> | <integer>) -> string
```
- builtin functions with a single argument can be used without parentheses:
```
> output sin 1
Output: 0.84147

-- in aggregate
> output sin cos tan sqrt abs length mid("Hello, World!", 0, 5)
Output: 0.287797

-- is equivalent to
> output sin(cos(tan(sqrt(abs(length(mid("Hello, World!")))))))
Output: 0.287797
```
- However, parentheses are required for builtins with expression arguments:
```
> output sin 1 + 1
Output: 1.84147

> output sin(1) + 1
Output: 1.84147

> output sin(1 + 1)
Output: 0.909297
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
