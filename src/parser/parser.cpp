#include "parser.h"
#include <stdexcept>

namespace minicc {

static const Token EOF_TOKEN = {TokenKind::END_OF_FILE, "EOF", 0};

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), pos_(0) {}

const Token& Parser::peek() const {
    if (pos_ < tokens_.size()) return tokens_[pos_];
    return EOF_TOKEN;
}

const Token& Parser::advance() {
    const Token& t = peek();
    if (pos_ < tokens_.size()) pos_++;
    return t;
}

bool Parser::check(TokenKind k) const {
    return peek().kind == k;
}

bool Parser::match(TokenKind k) {
    if (check(k)) { advance(); return true; }
    return false;
}

void Parser::expect(TokenKind k, const std::string& context) {
    if (!match(k)) {
        error("expected '" + std::string(token_kind_name(k)) + "' " + context);
    }
}

void Parser::error(const std::string& msg) {
    errors_.push_back({peek().line, msg});
}

void Parser::skip_eoln() {
    while (check(TokenKind::EOLN)) advance();
}

AstPtr Parser::parse() {
    return parse_program();
}

// program -> 'begin' decl_list ';' stmt_list 'end'
AstPtr Parser::parse_program() {
    skip_eoln();
    int ln = peek().line;
    expect(TokenKind::BEGIN, "at start of program");
    skip_eoln();

    auto prog = make_node(NodeKind::Program, "program", ln);
    prog->add(parse_decl_list());

    skip_eoln();
    expect(TokenKind::SEMI, "after declarations");
    skip_eoln();

    prog->add(parse_stmt_list());

    skip_eoln();
    expect(TokenKind::END, "at end of program");
    skip_eoln();

    return prog;
}

// decl_list -> decl (';' decl)*
AstPtr Parser::parse_decl_list() {
    auto list = make_node(NodeKind::DeclList, "", peek().line);
    list->add(parse_decl());

    while (true) {
        skip_eoln();
        // Look ahead: if ';' followed by 'integer' => more declarations
        if (check(TokenKind::SEMI)) {
            size_t saved = pos_;
            advance();
            skip_eoln();
            if (check(TokenKind::INTEGER)) {
                list->add(parse_decl());
            } else {
                pos_ = saved;
                break;
            }
        } else {
            break;
        }
    }
    return list;
}

// decl -> var_decl | func_decl
AstPtr Parser::parse_decl() {
    skip_eoln();
    if (!check(TokenKind::INTEGER)) {
        error("expected 'integer' in declaration");
        return nullptr;
    }

    // Look ahead to distinguish: integer function ... vs integer <id>
    size_t saved = pos_;
    advance(); // consume 'integer'
    skip_eoln();

    if (check(TokenKind::FUNCTION)) {
        pos_ = saved;
        return parse_func_decl();
    }

    pos_ = saved;
    return parse_var_decl();
}

// var_decl -> 'integer' SYMBOL
AstPtr Parser::parse_var_decl() {
    skip_eoln();
    int ln = peek().line;
    expect(TokenKind::INTEGER, "in variable declaration");
    skip_eoln();

    if (!check(TokenKind::SYMBOL)) {
        error("expected identifier in variable declaration");
        return nullptr;
    }
    const Token& id_tok = advance();
    skip_eoln();

    return make_node(NodeKind::VarDecl, id_tok.text, ln);
}

// func_decl -> 'integer' 'function' SYMBOL '(' param ')' ';' func_body
AstPtr Parser::parse_func_decl() {
    skip_eoln();
    int ln = peek().line;
    expect(TokenKind::INTEGER, "in function declaration");
    skip_eoln();
    expect(TokenKind::FUNCTION, "in function declaration");
    skip_eoln();

    std::string fname;
    if (check(TokenKind::SYMBOL)) {
        fname = advance().text;
    } else {
        error("expected function name");
    }

    skip_eoln();
    expect(TokenKind::LPAREN, "after function name");
    skip_eoln();

    auto func = make_node(NodeKind::FuncDef, fname, ln);

    // parameter: 'integer' SYMBOL
    if (check(TokenKind::INTEGER)) {
        auto param = parse_var_decl();
        if (param) {
            param->kind = NodeKind::ParamDecl;
            func->add(std::move(param));
        }
    } else if (check(TokenKind::SYMBOL)) {
        // parameter without type
        func->add(make_node(NodeKind::ParamDecl, advance().text, peek().line));
    }

    skip_eoln();
    expect(TokenKind::RPAREN, "after parameter");
    skip_eoln();
    expect(TokenKind::SEMI, "after function header");
    skip_eoln();

    // function body: 'begin' decl_list ';' stmt_list 'end'
    int bln = peek().line;
    expect(TokenKind::BEGIN, "at start of function body");
    skip_eoln();

    auto body = make_node(NodeKind::FuncBody, "", bln);
    body->add(parse_decl_list());

    skip_eoln();
    expect(TokenKind::SEMI, "after function declarations");
    skip_eoln();

    body->add(parse_stmt_list());

    skip_eoln();
    expect(TokenKind::END, "at end of function body");
    skip_eoln();

    func->add(std::move(body));
    return func;
}

// stmt_list -> stmt (';' stmt)*
AstPtr Parser::parse_stmt_list() {
    auto list = make_node(NodeKind::StmtList, "", peek().line);
    skip_eoln();
    list->add(parse_stmt());

    while (true) {
        skip_eoln();
        if (check(TokenKind::SEMI)) {
            advance();
            skip_eoln();
            if (check(TokenKind::END) || check(TokenKind::END_OF_FILE)) break;
            list->add(parse_stmt());
        } else {
            break;
        }
    }
    return list;
}

// stmt -> read_stmt | write_stmt | if_stmt | assign_stmt
AstPtr Parser::parse_stmt() {
    skip_eoln();
    if (check(TokenKind::READ))  return parse_read_stmt();
    if (check(TokenKind::WRITE)) return parse_write_stmt();
    if (check(TokenKind::IF))    return parse_if_stmt();
    if (check(TokenKind::SYMBOL)) return parse_assign_stmt();

    error("expected statement");
    advance();
    return nullptr;
}

// assign -> SYMBOL ':=' expr
AstPtr Parser::parse_assign_stmt() {
    skip_eoln();
    int ln = peek().line;
    std::string name = advance().text;  // SYMBOL
    skip_eoln();

    // Accept both ':=' and '=' as assignment
    if (check(TokenKind::ASSIGN)) {
        advance();
    } else if (check(TokenKind::EQ)) {
        advance();
    } else {
        error("expected ':=' in assignment");
    }

    skip_eoln();
    auto node = make_node(NodeKind::Assign, name, ln);
    node->add(parse_expr());
    return node;
}

// read -> 'read' '(' SYMBOL ')'
AstPtr Parser::parse_read_stmt() {
    int ln = peek().line;
    advance(); // 'read'
    skip_eoln();
    expect(TokenKind::LPAREN, "after 'read'");
    skip_eoln();

    auto node = make_node(NodeKind::Read, "", ln);
    if (check(TokenKind::SYMBOL)) {
        node->add(make_node(NodeKind::Id, advance().text, peek().line));
    } else {
        error("expected identifier in read()");
    }

    skip_eoln();
    expect(TokenKind::RPAREN, "after read argument");
    return node;
}

// write -> 'write' '(' expr ')'
AstPtr Parser::parse_write_stmt() {
    int ln = peek().line;
    advance(); // 'write'
    skip_eoln();
    expect(TokenKind::LPAREN, "after 'write'");
    skip_eoln();

    auto node = make_node(NodeKind::Write, "", ln);
    node->add(parse_expr());

    skip_eoln();
    expect(TokenKind::RPAREN, "after write argument");
    return node;
}

// if -> 'if' condition 'then' stmt 'else' stmt
AstPtr Parser::parse_if_stmt() {
    int ln = peek().line;
    advance(); // 'if'
    skip_eoln();

    auto node = make_node(NodeKind::If, "", ln);
    node->add(parse_condition());

    skip_eoln();
    expect(TokenKind::THEN, "after condition");
    skip_eoln();

    node->add(parse_stmt());

    skip_eoln();
    expect(TokenKind::ELSE, "in if statement");
    skip_eoln();

    node->add(parse_stmt());
    return node;
}

// condition -> expr relop expr
AstPtr Parser::parse_condition() {
    auto left = parse_expr();
    skip_eoln();

    std::string op;
    int ln = peek().line;
    TokenKind k = peek().kind;
    if (k == TokenKind::LT || k == TokenKind::LE || k == TokenKind::GT ||
        k == TokenKind::GE || k == TokenKind::EQ || k == TokenKind::NE) {
        op = advance().text;
    } else {
        error("expected relational operator");
        op = "?";
    }

    skip_eoln();
    auto right = parse_expr();

    auto node = make_node(NodeKind::RelOp, op, ln);
    node->add(std::move(left));
    node->add(std::move(right));
    return node;
}

// expr -> term (('+' | '-') term)*
// Note: this language has '-' but not '+' as a token, so we handle '-' only
AstPtr Parser::parse_expr() {
    skip_eoln();
    auto left = parse_term();

    while (true) {
        skip_eoln();
        if (check(TokenKind::SUB)) {
            int ln = peek().line;
            advance();
            skip_eoln();
            auto right = parse_term();
            auto binop = make_node(NodeKind::BinOp, "-", ln);
            binop->add(std::move(left));
            binop->add(std::move(right));
            left = std::move(binop);
        } else {
            break;
        }
    }
    return left;
}

// term -> factor ('*' factor)*
AstPtr Parser::parse_term() {
    skip_eoln();
    auto left = parse_factor();

    while (true) {
        skip_eoln();
        if (check(TokenKind::MUL)) {
            int ln = peek().line;
            advance();
            skip_eoln();
            auto right = parse_factor();
            auto binop = make_node(NodeKind::BinOp, "*", ln);
            binop->add(std::move(left));
            binop->add(std::move(right));
            left = std::move(binop);
        } else {
            break;
        }
    }
    return left;
}

// factor -> CONSTANT | SYMBOL | SYMBOL '(' expr ')' | '(' expr ')'
AstPtr Parser::parse_factor() {
    skip_eoln();

    if (check(TokenKind::CONSTANT)) {
        const Token& t = advance();
        return make_node(NodeKind::IntLit, t.text, t.line);
    }

    if (check(TokenKind::SYMBOL)) {
        const Token& t = advance();
        skip_eoln();
        if (check(TokenKind::LPAREN)) {
            // function call: id '(' expr ')'
            advance();
            skip_eoln();
            auto call = make_node(NodeKind::Call, t.text, t.line);
            call->add(parse_expr());
            skip_eoln();
            expect(TokenKind::RPAREN, "after function call argument");
            return call;
        }
        return make_node(NodeKind::Id, t.text, t.line);
    }

    if (check(TokenKind::LPAREN)) {
        advance();
        skip_eoln();
        auto inner = parse_expr();
        skip_eoln();
        expect(TokenKind::RPAREN, "after parenthesized expression");
        return inner;
    }

    error("expected expression");
    advance();
    return make_node(NodeKind::IntLit, "0", peek().line);
}

} // namespace minicc
