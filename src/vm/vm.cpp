#include "vm.h"
#include <sstream>
#include <stdexcept>

namespace minicc {

VM::VM(const std::vector<Quad>& ir, std::istream& in, std::ostream& out)
    : ir_(ir), in_(in), out_(out) {
    preprocess();
}

void VM::preprocess() {
    for (size_t i = 0; i < ir_.size(); ++i) {
        const auto& q = ir_[i];
        if (q.op == Op::LABEL) {
            labels_[q.dst] = i;
        }
        if (q.op == Op::FUNC_BEGIN) {
            func_entries_[q.dst] = i;
        }
    }
}

bool VM::is_temp(const std::string& name) const {
    return !name.empty() && name[0] == 't' && name.size() > 1;
}

int VM::get_var(const std::string& name) const {
    if (name.empty()) return 0;

    // Try to parse as integer literal
    bool all_digits = true;
    for (size_t i = (name[0] == '-' ? 1 : 0); i < name.size(); ++i) {
        if (name[i] < '0' || name[i] > '9') { all_digits = false; break; }
    }
    if (all_digits && !name.empty() && name != "-") {
        return std::stoi(name);
    }

    // Check current frame locals
    if (!call_stack_.empty()) {
        const auto& locals = call_stack_.back().locals;
        auto it = locals.find(name);
        if (it != locals.end()) return it->second;
    }

    // Check globals
    auto it = globals_.find(name);
    if (it != globals_.end()) return it->second;

    return 0;
}

void VM::set_var(const std::string& name, int val) {
    if (!call_stack_.empty()) {
        call_stack_.back().locals[name] = val;
    } else {
        globals_[name] = val;
    }
}

int VM::run() {
    // Start from "main" function
    auto main_it = func_entries_.find("main");
    if (main_it == func_entries_.end()) {
        throw std::runtime_error("VM: no 'main' function found");
    }

    size_t ip = main_it->second + 1; // skip FUNC_BEGIN
    Frame main_frame;
    main_frame.func_name = "main";
    main_frame.return_ip = ir_.size();
    call_stack_.push_back(main_frame);

    while (ip < ir_.size()) {
        const auto& q = ir_[ip];

        switch (q.op) {
            case Op::LOAD_CONST: {
                int val = std::stoi(q.src1);
                set_var(q.dst, val);
                break;
            }

            case Op::ASSIGN: {
                int val = get_var(q.src1);
                set_var(q.dst, val);
                break;
            }

            case Op::ADD: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a + b);
                break;
            }

            case Op::SUB: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a - b);
                break;
            }

            case Op::MUL: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a * b);
                break;
            }

            case Op::DIV: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                if (b == 0) throw std::runtime_error("VM: division by zero");
                set_var(q.dst, a / b);
                break;
            }

            case Op::LT: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a < b ? 1 : 0);
                break;
            }

            case Op::LE: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a <= b ? 1 : 0);
                break;
            }

            case Op::GT: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a > b ? 1 : 0);
                break;
            }

            case Op::GE: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a >= b ? 1 : 0);
                break;
            }

            case Op::EQ: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a == b ? 1 : 0);
                break;
            }

            case Op::NE: {
                int a = get_var(q.src1);
                int b = get_var(q.src2);
                set_var(q.dst, a != b ? 1 : 0);
                break;
            }

            case Op::JMP: {
                auto lit = labels_.find(q.dst);
                if (lit == labels_.end()) throw std::runtime_error("VM: unknown label " + q.dst);
                ip = lit->second;
                continue;
            }

            case Op::JZ: {
                int cond = get_var(q.src1);
                if (cond == 0) {
                    auto lit = labels_.find(q.dst);
                    if (lit == labels_.end()) throw std::runtime_error("VM: unknown label " + q.dst);
                    ip = lit->second;
                    continue;
                }
                break;
            }

            case Op::LABEL:
                // no-op, just a marker
                break;

            case Op::READ: {
                int val = 0;
                in_ >> val;
                // Set in current frame or globals depending on scope
                // First check if variable exists in any frame
                if (!call_stack_.empty()) {
                    call_stack_.back().locals[q.dst] = val;
                } else {
                    globals_[q.dst] = val;
                }
                break;
            }

            case Op::WRITE: {
                int val = get_var(q.dst);
                out_ << val << "\n";
                output_buf_ += std::to_string(val) + "\n";
                break;
            }

            case Op::PARAM: {
                int val = get_var(q.dst);
                param_stack_.push_back(val);
                break;
            }

            case Op::CALL: {
                std::string func_name = q.src1;
                auto fit = func_entries_.find(func_name);
                if (fit == func_entries_.end()) {
                    throw std::runtime_error("VM: undefined function '" + func_name + "'");
                }

                Frame new_frame;
                new_frame.func_name = func_name;
                new_frame.return_ip = ip + 1;
                new_frame.return_var = q.dst;

                // Copy globals that are not temps into new frame
                // (simplified: the new frame starts with access to param)
                // Transfer parameter
                size_t func_ip = fit->second + 1;
                // Look for PARAM instruction after FUNC_BEGIN
                while (func_ip < ir_.size() && ir_[func_ip].op == Op::PARAM) {
                    if (!param_stack_.empty()) {
                        new_frame.locals[ir_[func_ip].dst] = param_stack_.back();
                        param_stack_.pop_back();
                    }
                    func_ip++;
                }

                call_stack_.push_back(new_frame);
                ip = fit->second + 1;

                // Skip PARAM instructions (already processed above)
                while (ip < ir_.size() && ir_[ip].op == Op::PARAM) {
                    ip++;
                }
                continue;
            }

            case Op::RET: {
                // Not explicitly used in current IR gen, handled by FUNC_END
                break;
            }

            case Op::FUNC_BEGIN: {
                // If we hit a nested FUNC_BEGIN while executing, skip
                // the entire function body up to its matching FUNC_END.
                // We only enter functions via CALL, never by falling through.
                if (!call_stack_.empty() && call_stack_.back().func_name != q.dst) {
                    int depth = 1;
                    ip++;
                    while (ip < ir_.size() && depth > 0) {
                        if (ir_[ip].op == Op::FUNC_BEGIN) depth++;
                        if (ir_[ip].op == Op::FUNC_END) depth--;
                        ip++;
                    }
                    continue; // ip already points past FUNC_END
                }
                break;
            }

            case Op::FUNC_END: {
                if (call_stack_.size() <= 1) {
                    // Main function ending
                    return 0;
                }

                // Get return value: function name variable in current scope
                Frame& current = call_stack_.back();
                int return_val = 0;
                auto it = current.locals.find(current.func_name);
                if (it != current.locals.end()) {
                    return_val = it->second;
                }

                size_t ret_ip = current.return_ip;
                std::string ret_var = current.return_var;
                call_stack_.pop_back();

                // Set return value in calling frame
                set_var(ret_var, return_val);
                ip = ret_ip;
                continue;
            }

            default:
                break;
        }

        ip++;
    }

    return 0;
}

} // namespace minicc
