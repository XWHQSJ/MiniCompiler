#include "three_addr.h"
#include <sstream>

namespace minicc {

const char* op_name(Op op) {
    switch (op) {
        case Op::ADD:        return "ADD";
        case Op::SUB:        return "SUB";
        case Op::MUL:        return "MUL";
        case Op::DIV:        return "DIV";
        case Op::MOD:        return "MOD";
        case Op::ASSIGN:     return "ASSIGN";
        case Op::LOAD_CONST: return "LOAD_CONST";
        case Op::JMP:        return "JMP";
        case Op::JZ:         return "JZ";
        case Op::LABEL:      return "LABEL";
        case Op::CALL:       return "CALL";
        case Op::RET:        return "RET";
        case Op::READ:       return "READ";
        case Op::WRITE:      return "WRITE";
        case Op::LT:         return "LT";
        case Op::LE:         return "LE";
        case Op::GT:         return "GT";
        case Op::GE:         return "GE";
        case Op::EQ:         return "EQ";
        case Op::NE:         return "NE";
        case Op::PARAM:      return "PARAM";
        case Op::FUNC_BEGIN: return "FUNC_BEGIN";
        case Op::FUNC_END:   return "FUNC_END";
        default:             return "???";
    }
}

std::string IRGenerator::new_temp() {
    return "t" + std::to_string(temp_count_++);
}

std::string IRGenerator::new_label() {
    return "L" + std::to_string(label_count_++);
}

void IRGenerator::emit(Op op, const std::string& dst,
                       const std::string& s1, const std::string& s2) {
    quads_.push_back({op, dst, s1, s2});
}

std::vector<Quad> IRGenerator::generate(const AstNode* root) {
    quads_.clear();
    temp_count_ = 0;
    label_count_ = 0;
    gen_node(root);
    return quads_;
}

void IRGenerator::gen_node(const AstNode* node) {
    if (!node) return;

    switch (node->kind) {
        case NodeKind::Program:
            emit(Op::FUNC_BEGIN, "main");
            for (const auto& ch : node->children) {
                gen_node(ch.get());
            }
            emit(Op::FUNC_END, "main");
            break;

        case NodeKind::FuncDef: {
            emit(Op::FUNC_BEGIN, node->text);
            // parameter
            for (const auto& ch : node->children) {
                if (ch->kind == NodeKind::ParamDecl) {
                    emit(Op::PARAM, ch->text);
                } else {
                    gen_node(ch.get());
                }
            }
            emit(Op::FUNC_END, node->text);
            break;
        }

        case NodeKind::FuncBody:
        case NodeKind::DeclList:
        case NodeKind::StmtList:
            for (const auto& ch : node->children) {
                gen_node(ch.get());
            }
            break;

        case NodeKind::VarDecl:
            // declarations don't generate IR in this simple model
            break;

        default:
            gen_stmt(node);
            break;
    }
}

void IRGenerator::gen_stmt(const AstNode* node) {
    if (!node) return;

    switch (node->kind) {
        case NodeKind::Assign: {
            if (!node->children.empty()) {
                std::string val = gen_expr(node->children[0].get());
                emit(Op::ASSIGN, node->text, val);
            }
            break;
        }

        case NodeKind::Read:
            if (!node->children.empty()) {
                emit(Op::READ, node->children[0]->text);
            }
            break;

        case NodeKind::Write:
            if (!node->children.empty()) {
                std::string val = gen_expr(node->children[0].get());
                emit(Op::WRITE, val);
            }
            break;

        case NodeKind::If: {
            // children: [0]=condition, [1]=then_stmt, [2]=else_stmt
            std::string else_label = new_label();
            std::string end_label = new_label();

            if (node->children.size() >= 1) {
                gen_condition(node->children[0].get(), else_label);
            }
            if (node->children.size() >= 2) {
                gen_stmt(node->children[1].get());
            }
            emit(Op::JMP, end_label);
            emit(Op::LABEL, else_label);
            if (node->children.size() >= 3) {
                gen_stmt(node->children[2].get());
            }
            emit(Op::LABEL, end_label);
            break;
        }

        case NodeKind::While: {
            // children: [0]=condition, [1]=body_stmt
            std::string loop_label = new_label();
            std::string end_label = new_label();

            emit(Op::LABEL, loop_label);
            if (node->children.size() >= 1) {
                gen_condition(node->children[0].get(), end_label);
            }
            if (node->children.size() >= 2) {
                gen_stmt(node->children[1].get());
            }
            emit(Op::JMP, loop_label);
            emit(Op::LABEL, end_label);
            break;
        }

        default:
            // For any nested structure, try children
            for (const auto& ch : node->children) {
                gen_node(ch.get());
            }
            break;
    }
}

void IRGenerator::gen_condition(const AstNode* node, const std::string& false_label) {
    if (!node || node->kind != NodeKind::RelOp) {
        emit(Op::JZ, false_label, "0");
        return;
    }

    std::string left = gen_expr(node->children.size() > 0 ? node->children[0].get() : nullptr);
    std::string right = gen_expr(node->children.size() > 1 ? node->children[1].get() : nullptr);

    std::string cond_tmp = new_temp();
    Op cmp_op = Op::EQ;
    const std::string& op_text = node->text;
    if (op_text == "<")  cmp_op = Op::LT;
    else if (op_text == "<=") cmp_op = Op::LE;
    else if (op_text == ">")  cmp_op = Op::GT;
    else if (op_text == ">=") cmp_op = Op::GE;
    else if (op_text == "=")  cmp_op = Op::EQ;
    else if (op_text == "<>") cmp_op = Op::NE;

    emit(cmp_op, cond_tmp, left, right);
    emit(Op::JZ, false_label, cond_tmp);
}

std::string IRGenerator::gen_expr(const AstNode* node) {
    if (!node) return "0";

    switch (node->kind) {
        case NodeKind::IntLit: {
            std::string tmp = new_temp();
            emit(Op::LOAD_CONST, tmp, node->text);
            return tmp;
        }

        case NodeKind::Id:
            return node->text;

        case NodeKind::BinOp: {
            std::string left = gen_expr(node->children.size() > 0 ? node->children[0].get() : nullptr);
            std::string right = gen_expr(node->children.size() > 1 ? node->children[1].get() : nullptr);
            std::string tmp = new_temp();

            Op bin_op = Op::ADD;
            if (node->text == "-") bin_op = Op::SUB;
            else if (node->text == "*") bin_op = Op::MUL;
            else if (node->text == "/" || node->text == "div") bin_op = Op::DIV;
            else if (node->text == "mod") bin_op = Op::MOD;
            else if (node->text == "+") bin_op = Op::ADD;

            emit(bin_op, tmp, left, right);
            return tmp;
        }

        case NodeKind::Call: {
            // generate argument expression first
            std::string arg = "0";
            if (!node->children.empty()) {
                arg = gen_expr(node->children[0].get());
            }
            emit(Op::PARAM, arg);
            std::string tmp = new_temp();
            emit(Op::CALL, tmp, node->text, "1");
            return tmp;
        }

        case NodeKind::UnaryMinus: {
            std::string child = gen_expr(node->children.size() > 0 ? node->children[0].get() : nullptr);
            std::string tmp = new_temp();
            std::string zero = new_temp();
            emit(Op::LOAD_CONST, zero, "0");
            emit(Op::SUB, tmp, zero, child);
            return tmp;
        }

        default:
            return node->text.empty() ? "0" : node->text;
    }
}

std::string IRGenerator::dump() const {
    std::ostringstream ss;
    for (size_t i = 0; i < quads_.size(); ++i) {
        const auto& q = quads_[i];
        ss << i << ":\t" << op_name(q.op) << "\t" << q.dst;
        if (!q.src1.empty()) ss << "\t" << q.src1;
        if (!q.src2.empty()) ss << "\t" << q.src2;
        ss << "\n";
    }
    return ss.str();
}

} // namespace minicc
