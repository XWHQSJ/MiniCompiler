#pragma once
#ifndef MINICC_AST_H
#define MINICC_AST_H

#include <string>
#include <vector>
#include <memory>

namespace minicc {

enum class NodeKind {
    Program,
    FuncDef,
    FuncBody,
    ParamDecl,
    VarDecl,
    DeclList,
    StmtList,
    Assign,
    If,
    While,
    Read,
    Write,
    BinOp,
    RelOp,
    UnaryMinus,
    Id,
    IntLit,
    Call
};

const char* node_kind_name(NodeKind k);

struct AstNode {
    NodeKind kind;
    std::string text;
    std::vector<std::unique_ptr<AstNode>> children;
    int line = 0;

    AstNode(NodeKind k, const std::string& t, int ln)
        : kind(k), text(t), line(ln) {}

    void add(std::unique_ptr<AstNode> child) {
        if (child) children.push_back(std::move(child));
    }
};

using AstPtr = std::unique_ptr<AstNode>;

AstPtr make_node(NodeKind k, const std::string& text, int line);

} // namespace minicc

#endif
