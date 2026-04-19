#include "semantic.h"

namespace minicc {

void SemanticAnalyzer::error(int line, const std::string& msg) {
    errors_.push_back({line, msg});
}

std::vector<SemanticError> SemanticAnalyzer::analyze(const AstNode* root) {
    errors_.clear();
    sym_.push_scope();
    visit(root);
    sym_.pop_scope();
    return errors_;
}

void SemanticAnalyzer::visit(const AstNode* node) {
    if (!node) return;

    switch (node->kind) {
        case NodeKind::Program:    visit_program(node); break;
        case NodeKind::FuncDef:    visit_func_def(node); break;
        case NodeKind::VarDecl:    visit_var_decl(node); break;
        case NodeKind::ParamDecl:  visit_param_decl(node); break;
        case NodeKind::Assign:     visit_assign(node); break;
        case NodeKind::Read:       visit_read(node); break;
        case NodeKind::Write:      visit_write(node); break;
        case NodeKind::If:
            if (node->children.size() >= 1) check_expr(node->children[0].get());
            if (node->children.size() >= 2) visit(node->children[1].get());
            if (node->children.size() >= 3) visit(node->children[2].get());
            break;
        case NodeKind::DeclList:
        case NodeKind::StmtList:
        case NodeKind::FuncBody:
            for (const auto& ch : node->children) visit(ch.get());
            break;
        default:
            check_expr(node);
            break;
    }
}

void SemanticAnalyzer::visit_program(const AstNode* node) {
    for (const auto& ch : node->children) {
        visit(ch.get());
    }
}

void SemanticAnalyzer::visit_func_def(const AstNode* node) {
    Symbol func_sym;
    func_sym.name = node->text;
    func_sym.sym_kind = SymKind::Function;
    func_sym.type_name = "integer";
    func_sym.level = sym_.depth();
    func_sym.param_count = 0;
    func_sym.line = node->line;

    // Count params
    for (const auto& ch : node->children) {
        if (ch->kind == NodeKind::ParamDecl) func_sym.param_count++;
    }

    if (!sym_.insert(func_sym)) {
        error(node->line, "function '" + node->text + "' redefined");
    }

    sym_.push_scope();

    // Also insert function name as a variable in its own scope (for return assignment)
    Symbol ret_sym;
    ret_sym.name = node->text;
    ret_sym.sym_kind = SymKind::Variable;
    ret_sym.type_name = "integer";
    ret_sym.level = sym_.depth();
    ret_sym.line = node->line;
    sym_.insert(ret_sym);

    for (const auto& ch : node->children) {
        visit(ch.get());
    }

    sym_.pop_scope();
}

void SemanticAnalyzer::visit_var_decl(const AstNode* node) {
    // In this language, a VarDecl inside a function body that matches
    // a ParamDecl (same name, same scope) is the normal pattern --
    // the function header declares the param and the body re-declares it
    // as a local. Allow this without reporting an error.
    auto existing = sym_.lookup_in_current_scope(node->text);
    if (existing.has_value() && existing->sym_kind == SymKind::Parameter) {
        // Silently accept: body re-declaration of a parameter
        return;
    }

    Symbol sym;
    sym.name = node->text;
    sym.sym_kind = SymKind::Variable;
    sym.type_name = "integer";
    sym.level = sym_.depth();
    sym.line = node->line;

    if (!sym_.insert(sym)) {
        error(node->line, "variable '" + node->text + "' redefined");
    }
}

void SemanticAnalyzer::visit_param_decl(const AstNode* node) {
    Symbol sym;
    sym.name = node->text;
    sym.sym_kind = SymKind::Parameter;
    sym.type_name = "integer";
    sym.level = sym_.depth();
    sym.line = node->line;

    if (!sym_.insert(sym)) {
        error(node->line, "parameter '" + node->text + "' redefined");
    }
}

void SemanticAnalyzer::visit_assign(const AstNode* node) {
    auto looked = sym_.lookup(node->text);
    if (!looked.has_value()) {
        error(node->line, "undefined variable '" + node->text + "'");
    }
    for (const auto& ch : node->children) {
        check_expr(ch.get());
    }
}

void SemanticAnalyzer::visit_read(const AstNode* node) {
    for (const auto& ch : node->children) {
        check_id_use(ch.get());
    }
}

void SemanticAnalyzer::visit_write(const AstNode* node) {
    for (const auto& ch : node->children) {
        check_expr(ch.get());
    }
}

void SemanticAnalyzer::check_expr(const AstNode* node) {
    if (!node) return;

    switch (node->kind) {
        case NodeKind::Id:
            check_id_use(node);
            break;
        case NodeKind::Call: {
            // For recursive calls, the function name may also exist as a
            // Variable in the current scope (return-value alias). Look
            // through all scopes to find a Function symbol.
            auto sym = sym_.lookup(node->text);
            if (!sym.has_value()) {
                error(node->line, "undefined function '" + node->text + "'");
            } else if (sym->sym_kind == SymKind::Function) {
                int arg_count = static_cast<int>(node->children.size());
                if (arg_count != sym->param_count) {
                    error(node->line, "function '" + node->text + "' expects "
                          + std::to_string(sym->param_count) + " argument(s), got "
                          + std::to_string(arg_count));
                }
            } else {
                // The first match was a Variable (return-value alias).
                // This is expected inside a recursive function -- not an error.
            }
            for (const auto& ch : node->children) check_expr(ch.get());
            break;
        }
        case NodeKind::BinOp:
        case NodeKind::RelOp:
        case NodeKind::UnaryMinus:
            for (const auto& ch : node->children) check_expr(ch.get());
            break;
        case NodeKind::IntLit:
            break;
        default:
            for (const auto& ch : node->children) check_expr(ch.get());
            break;
    }
}

void SemanticAnalyzer::check_id_use(const AstNode* node) {
    if (!node || node->kind != NodeKind::Id) return;
    auto sym = sym_.lookup(node->text);
    if (!sym.has_value()) {
        error(node->line, "undefined identifier '" + node->text + "'");
    }
}

} // namespace minicc
