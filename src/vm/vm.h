#pragma once
#ifndef MINICC_VM_H
#define MINICC_VM_H

#include "../ir/three_addr.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace minicc {

class VM {
public:
    explicit VM(const std::vector<Quad>& ir,
                std::istream& in = std::cin,
                std::ostream& out = std::cout);

    int run();
    const std::string& output() const { return output_buf_; }

private:
    const std::vector<Quad>& ir_;
    std::istream& in_;
    std::ostream& out_;
    std::string output_buf_;

    // Global variable store
    std::unordered_map<std::string, int> globals_;

    // Call stack frame
    struct Frame {
        std::unordered_map<std::string, int> locals;
        std::string func_name;
        size_t return_ip;
        std::string return_var;
    };

    std::vector<Frame> call_stack_;
    std::vector<int> param_stack_;

    // label -> instruction index
    std::unordered_map<std::string, size_t> labels_;
    // function name -> instruction index
    std::unordered_map<std::string, size_t> func_entries_;

    void preprocess();
    int get_var(const std::string& name) const;
    void set_var(const std::string& name, int val);
    bool is_temp(const std::string& name) const;
};

} // namespace minicc

#endif
