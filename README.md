# MiniCompiler

![C++](https://img.shields.io/badge/language-C%2B%2B-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)

A tiny C compiler built from scratch as a learning project. Each stage of the classic compiler pipeline is implemented as a standalone module so you can study, run, and modify them independently.

## About

MiniCompiler walks through the front-end phases of a C compiler:

1. **Preprocessing** -- expanding `#include` directives and macros
2. **Lexical analysis** -- tokenizing source code into a stream of tokens
3. **Parsing** -- checking syntactic structure against grammar rules
4. **Syntax tree construction** -- building and validating the parse tree

The project does **not** generate machine code; it stops after syntax analysis. Its purpose is educational: understanding how each compiler phase works in isolation.

## Compiler Pipeline

```
Source (.c)
    |
    v
PreCompiler  -->  expanded source
    |
    v
Scanner / LexAnalyze  -->  token stream (.dyd)
    |
    v
Parase (Parser)  -->  symbol table (.var), procedure table (.pro)
    |
    v
Syntaxer  -->  syntax tree (.dys)
```

> **Note:** The directory `Parase` is a historical misspelling of "Parser". It is kept as-is to preserve git history.

## Directory Layout

```
MiniCompiler/
├── PreCompiler/          Preprocessor -- handles #include and macro expansion
│   ├── preCompiler.cpp
│   ├── 1.c, 2.c, 1.h    Sample input files
├── LexAnalyze/           Lexical analyzer (C++ / STL version)
│   ├── lexAnalyze.cpp
│   ├── lexAnalyze.h
│   └── test.txt          Sample token input
├── Scanner/Scanner/      Lexical analyzer (C / file-based version)
│   ├── lexAnalyze.cpp
│   └── lexAnalyze.h
├── Parase/Parase/        Recursive-descent parser
│   ├── paraser.cpp
│   └── paraser.h
├── Syntaxer/Syntaxer/    Syntax analyzer and tree builder
│   ├── syntaxAnalyze.cpp
│   └── syntaxAnalyze.h
├── tree.xml              Example parse-tree output
├── LICENSE
└── README.md
```

## Requirements

- A C++ compiler: g++, clang++, or MSVC
- C++11 or later recommended (C++17 preferred)

## Build and Run

Each module is a self-contained program. Compile and run them individually:

```bash
# PreCompiler -- expand includes and macros
g++ PreCompiler/preCompiler.cpp -std=c++17 -o precompiler
./precompiler PreCompiler/1.c

# LexAnalyze -- tokenize from file
g++ LexAnalyze/lexAnalyze.cpp -std=c++17 -o lexanalyze
./lexanalyze LexAnalyze/test.txt

# Scanner -- alternative lexer (C-style)
g++ Scanner/Scanner/lexAnalyze.cpp -std=c++17 -o scanner
./scanner

# Parser -- recursive-descent parse
g++ Parase/Parase/paraser.cpp -std=c++17 -o parser
./parser

# Syntaxer -- syntax analysis and tree output
g++ Syntaxer/Syntaxer/syntaxAnalyze.cpp -std=c++17 -o syntaxer
./syntaxer
```

**Visual Studio users:** Each subdirectory corresponds to a VS project. Open the `.sln` files directly if they are present, or create a new empty project and add the source files.

## Status

Educational / experimental. The compiler front-end covers preprocessing through syntax analysis. It does not produce intermediate representation or binary code.

## License

[MIT](LICENSE)
