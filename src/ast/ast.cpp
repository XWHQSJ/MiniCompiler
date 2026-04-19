#include "ast.h"

namespace minicc {

const char* node_kind_name(NodeKind k) {
    switch (k) {
        case NodeKind::Program:    return "Program";
        case NodeKind::FuncDef:    return "FuncDef";
        case NodeKind::FuncBody:   return "FuncBody";
        case NodeKind::ParamDecl:  return "ParamDecl";
        case NodeKind::VarDecl:    return "VarDecl";
        case NodeKind::DeclList:   return "DeclList";
        case NodeKind::StmtList:   return "StmtList";
        case NodeKind::Assign:     return "Assign";
        case NodeKind::If:         return "If";
        case NodeKind::Read:       return "Read";
        case NodeKind::Write:      return "Write";
        case NodeKind::BinOp:      return "BinOp";
        case NodeKind::RelOp:      return "RelOp";
        case NodeKind::UnaryMinus: return "UnaryMinus";
        case NodeKind::Id:         return "Id";
        case NodeKind::IntLit:     return "IntLit";
        case NodeKind::Call:       return "Call";
        default:                   return "Unknown";
    }
}

AstPtr make_node(NodeKind k, const std::string& text, int line) {
    return std::make_unique<AstNode>(k, text, line);
}

} // namespace minicc
