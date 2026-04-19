#include "lex_pascal.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace minicc {

static const char* KEYWORDS[] = {
    "begin", "end", "integer", "if", "then",
    "else", "function", "read", "write"
};
static const int KEYWORD_COUNT = 9;

static const struct { const char* word; TokenKind kind; } EXTRA_KEYWORDS[] = {
    {"while", TokenKind::WHILE},
    {"do",    TokenKind::DO},
    {"div",   TokenKind::DIV},
    {"mod",   TokenKind::MOD},
};

const char* token_kind_name(TokenKind k) {
    switch (k) {
        case TokenKind::BEGIN: return "begin";
        case TokenKind::END: return "end";
        case TokenKind::INTEGER: return "integer";
        case TokenKind::IF: return "if";
        case TokenKind::THEN: return "then";
        case TokenKind::ELSE: return "else";
        case TokenKind::FUNCTION: return "function";
        case TokenKind::READ: return "read";
        case TokenKind::WRITE: return "write";
        case TokenKind::WHILE: return "while";
        case TokenKind::DO: return "do";
        case TokenKind::SYMBOL: return "SYMBOL";
        case TokenKind::CONSTANT: return "CONSTANT";
        case TokenKind::EQ: return "=";
        case TokenKind::NE: return "<>";
        case TokenKind::LE: return "<=";
        case TokenKind::LT: return "<";
        case TokenKind::GE: return ">=";
        case TokenKind::GT: return ">";
        case TokenKind::ADD: return "+";
        case TokenKind::SUB: return "-";
        case TokenKind::MUL: return "*";
        case TokenKind::DIV: return "div";
        case TokenKind::MOD: return "mod";
        case TokenKind::ASSIGN: return ":=";
        case TokenKind::LPAREN: return "(";
        case TokenKind::RPAREN: return ")";
        case TokenKind::SEMI: return ";";
        case TokenKind::COMMA: return ",";
        case TokenKind::EOLN: return "EOLN";
        case TokenKind::END_OF_FILE: return "EOF";
        default: return "?";
    }
}

static TokenKind lookup_keyword(const std::string& s) {
    for (int i = 0; i < KEYWORD_COUNT; ++i) {
        if (s == KEYWORDS[i]) {
            return static_cast<TokenKind>(i + 1);
        }
    }
    for (const auto& ek : EXTRA_KEYWORDS) {
        if (s == ek.word) return ek.kind;
    }
    return TokenKind::SYMBOL;
}

std::vector<Token> lex_pascal(const std::string& source) {
    std::vector<Token> tokens;
    int line = 1;
    size_t pos = 0;
    const size_t len = source.size();

    while (pos < len) {
        char ch = source[pos];

        if (ch == '\n') {
            tokens.push_back({TokenKind::EOLN, "EOLN", line});
            line++;
            pos++;
            continue;
        }

        if (ch == ' ' || ch == '\t' || ch == '\r') {
            pos++;
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(ch))) {
            std::string word;
            while (pos < len && (std::isalpha(static_cast<unsigned char>(source[pos]))
                                 || std::isdigit(static_cast<unsigned char>(source[pos]))
                                 || source[pos] == '_')) {
                word.push_back(source[pos]);
                pos++;
            }
            TokenKind kind = lookup_keyword(word);
            tokens.push_back({kind, word, line});
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            std::string num;
            while (pos < len && std::isdigit(static_cast<unsigned char>(source[pos]))) {
                num.push_back(source[pos]);
                pos++;
            }
            tokens.push_back({TokenKind::CONSTANT, num, line});
            continue;
        }

        switch (ch) {
            case '<':
                pos++;
                if (pos < len && source[pos] == '=') {
                    tokens.push_back({TokenKind::LE, "<=", line});
                    pos++;
                } else if (pos < len && source[pos] == '>') {
                    tokens.push_back({TokenKind::NE, "<>", line});
                    pos++;
                } else {
                    tokens.push_back({TokenKind::LT, "<", line});
                }
                break;
            case '>':
                pos++;
                if (pos < len && source[pos] == '=') {
                    tokens.push_back({TokenKind::GE, ">=", line});
                    pos++;
                } else {
                    tokens.push_back({TokenKind::GT, ">", line});
                }
                break;
            case '=':
                tokens.push_back({TokenKind::EQ, "=", line});
                pos++;
                break;
            case ':':
                pos++;
                if (pos < len && source[pos] == '=') {
                    tokens.push_back({TokenKind::ASSIGN, ":=", line});
                    pos++;
                }
                break;
            case '-':
                tokens.push_back({TokenKind::SUB, "-", line});
                pos++;
                break;
            case '+':
                tokens.push_back({TokenKind::ADD, "+", line});
                pos++;
                break;
            case '*':
                tokens.push_back({TokenKind::MUL, "*", line});
                pos++;
                break;
            case ',':
                tokens.push_back({TokenKind::COMMA, ",", line});
                pos++;
                break;
            case '(':
                tokens.push_back({TokenKind::LPAREN, "(", line});
                pos++;
                break;
            case ')':
                tokens.push_back({TokenKind::RPAREN, ")", line});
                pos++;
                break;
            case ';':
                tokens.push_back({TokenKind::SEMI, ";", line});
                pos++;
                break;
            default:
                pos++;
                break;
        }
    }

    tokens.push_back({TokenKind::END_OF_FILE, "EOF", line});
    return tokens;
}

std::vector<Token> lex_pascal_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return lex_pascal(ss.str());
}

} // namespace minicc
