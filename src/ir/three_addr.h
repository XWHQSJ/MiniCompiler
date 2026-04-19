#pragma once
#ifndef MINICC_THREE_ADDR_H
#define MINICC_THREE_ADDR_H

#include <string>
#include <vector>
#include "../ast/ast.h"

namespace minicc {

enum class Op {
    ADD, SUB, MUL, DIV, MOD,
    ASSIGN, LOAD_CONST,
    JMP, JZ, LABEL,
    CALL, RET,
    READ, WRITE,
    LT, LE, GT, GE, EQ, NE,
    PARAM, FUNC_BEGIN, FUNC_END
};

const char* op_name(Op op);

struct Quad {
    Op op;
    std::string dst;
    std::string src1;
    std::string src2;
};

class IRGenerator {
public:
    std::vector<Quad> generate(const AstNode* root);
    const std::vector<Quad>& quads() const { return quads_; }
    std::string dump() const;

private:
    std::vector<Quad> quads_;
    int temp_count_ = 0;
    int label_count_ = 0;

    std::string new_temp();
    std::string new_label();

    void emit(Op op, const std::string& dst,
              const std::string& s1 = "", const std::string& s2 = "");

    void gen_node(const AstNode* node);
    std::string gen_expr(const AstNode* node);
    void gen_stmt(const AstNode* node);
    void gen_condition(const AstNode* node, const std::string& false_label);
};

} // namespace minicc

#endif
