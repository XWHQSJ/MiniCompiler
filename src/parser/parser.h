#pragma once
#ifndef MINICC_PARSER_H
#define MINICC_PARSER_H

#include "../lexer_pascal/lex_pascal.h"
#include "../ast/ast.h"
#include <vector>
#include <string>

namespace minicc {

struct ParseError {
    int line;
    std::string message;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    AstPtr parse();
    const std::vector<ParseError>& errors() const { return errors_; }

private:
    const std::vector<Token>& tokens_;
    size_t pos_;
    std::vector<ParseError> errors_;

    const Token& peek() const;
    const Token& advance();
    bool check(TokenKind k) const;
    bool match(TokenKind k);
    void expect(TokenKind k, const std::string& context);
    void error(const std::string& msg);

    void skip_eoln();

    AstPtr parse_program();
    AstPtr parse_decl_list();
    AstPtr parse_decl();
    AstPtr parse_var_decl();
    AstPtr parse_func_decl();
    AstPtr parse_stmt_list();
    AstPtr parse_stmt();
    AstPtr parse_assign_stmt();
    AstPtr parse_read_stmt();
    AstPtr parse_write_stmt();
    AstPtr parse_if_stmt();
    AstPtr parse_expr();
    AstPtr parse_term();
    AstPtr parse_factor();
    AstPtr parse_condition();
};

} // namespace minicc

#endif
