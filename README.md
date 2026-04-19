# MiniCompiler

![C++](https://img.shields.io/badge/language-C%2B%2B-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)

A tiny compiler for a Pascal-like mini-language. Covers the full pipeline from lexical analysis through to execution on a stack-based virtual machine.

## Compiler Pipeline

```
source.pas
   |   lex_pascal
   v
Token stream
   |   Parser (recursive descent)
   v
AST (Abstract Syntax Tree)
   |   SemanticAnalyzer
   v
Checked AST
   |   IRGenerator
   v
Three-address IR (quads)
   |   VM
   v
Program output
```

## Directory Layout

```
MiniCompiler/
├── src/
│   ├── lexer_pascal/     Pascal-like tokenizer (lex_pascal.h/.cpp)
│   ├── ast/              AST node types + XML/JSON printers
│   ├── symtable/         Scoped symbol table (push/pop)
│   ├── parser/           Recursive-descent parser -> AST
│   ├── ir/               Three-address IR generator + semantic analysis
│   ├── vm/               Stack-based virtual machine
│   └── driver/           Unified CLI entry point (main.cpp)
├── tests/                GoogleTest suites (lexer, parser, IR, VM)
│   └── golden/           Golden-file test data
├── examples/             Sample programs (hello.pas, factorial.pas)
├── LexAnalyze/           Legacy C-subset lexer (standalone)
├── PreCompiler/          Legacy preprocessor (standalone)
├── Scanner/              Legacy Pascal lexer (file-based, standalone)
├── Parser/               Legacy parser (renamed from Parase)
├── Syntaxer/             Legacy syntax analyzer (standalone)
├── CMakeLists.txt        Top-level CMake build
└── README.md
```

## Build

Requires CMake 3.16+ and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

29 tests cover: lexer (7), parser + golden file (7), semantic analysis + IR (6), VM end-to-end (9).

## CLI Usage

```bash
# Parse and print AST as XML (default)
./build/minicc examples/hello.pas

# JSON AST output
./build/minicc examples/hello.pas --json

# Show token stream
./build/minicc examples/hello.pas --tokens

# Dump three-address IR
./build/minicc examples/factorial.pas --ir

# Execute the program on the VM
echo "5" | ./build/minicc examples/factorial.pas --run
# => 120
```

## VM Instruction Reference

| Op          | Description                          |
|-------------|--------------------------------------|
| LOAD_CONST  | dst = literal value                  |
| ASSIGN      | dst = src1                           |
| ADD/SUB/MUL/DIV | dst = src1 op src2               |
| LT/LE/GT/GE/EQ/NE | dst = (src1 cmp src2) ? 1 : 0 |
| JMP         | Unconditional jump to label          |
| JZ          | Jump to label if src1 == 0           |
| LABEL       | Named label marker                   |
| READ        | Read integer from stdin into dst     |
| WRITE       | Write dst to stdout                  |
| PARAM       | Push argument onto parameter stack   |
| CALL        | dst = call src1 with src2 args       |
| RET         | Return from function                 |
| FUNC_BEGIN  | Start of function body               |
| FUNC_END    | End of function body                 |

## Legacy Modules

The top-level directories `LexAnalyze/`, `PreCompiler/`, `Scanner/`, `Parser/`, and `Syntaxer/` are the original standalone modules. They are preserved for reference. The unified pipeline lives under `src/`.

## License

[MIT](LICENSE)
