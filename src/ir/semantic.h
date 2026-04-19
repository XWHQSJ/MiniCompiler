#pragma once
#ifndef MINICC_SEMANTIC_H
#define MINICC_SEMANTIC_H

#include "../ast/ast.h"
#include "../symtable/symtable.h"
#include <vector>
#include <string>

namespace minicc {

struct SemanticError {
    int line;
    std::string message;
};

class SemanticAnalyzer {
public:
    std::vector<SemanticError> analyze(const AstNode* root);
    const std::vector<SemanticError>& errors() const { return errors_; }

private:
    SymTable sym_;
    std::vector<SemanticError> errors_;

    void error(int line, const std::string& msg);
    void visit(const AstNode* node);
    void visit_program(const AstNode* node);
    void visit_func_def(const AstNode* node);
    void visit_var_decl(const AstNode* node);
    void visit_param_decl(const AstNode* node);
    void visit_assign(const AstNode* node);
    void visit_read(const AstNode* node);
    void visit_write(const AstNode* node);
    void check_expr(const AstNode* node);
    void check_id_use(const AstNode* node);
};

} // namespace minicc

#endif
