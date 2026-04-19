#pragma once
#ifndef MINICC_LEX_PASCAL_H
#define MINICC_LEX_PASCAL_H

#include <string>
#include <vector>

namespace minicc {

enum class TokenKind {
    BEGIN = 1, END, INTEGER, IF, THEN, ELSE, FUNCTION, READ, WRITE,
    SYMBOL = 10, CONSTANT = 11,
    EQ = 12, NE = 13, LE = 14, LT = 15, GE = 16, GT = 17,
    SUB = 18, MUL = 19, ASSIGN = 20,
    LPAREN = 21, RPAREN = 22, SEMI = 23,
    EOLN = 24, END_OF_FILE = 25,
    ADD = 26,
    WHILE = 27, DO = 28, COMMA = 29, DIV = 30, MOD = 31
};

struct Token {
    TokenKind kind;
    std::string text;
    int line;
};

const char* token_kind_name(TokenKind k);

std::vector<Token> lex_pascal(const std::string& source);
std::vector<Token> lex_pascal_file(const std::string& path);

} // namespace minicc

#endif
