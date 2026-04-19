#pragma once
#ifndef MINICC_SYMTABLE_H
#define MINICC_SYMTABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <stdexcept>

namespace minicc {

enum class SymKind { Variable, Parameter, Function };

struct Symbol {
    std::string name;
    SymKind sym_kind;
    std::string type_name;  // "integer"
    int level;
    int address;
    int param_count;  // for functions
    int line;
};

class SymTable {
public:
    void push_scope() {
        scopes_.emplace_back();
    }

    void pop_scope() {
        if (scopes_.empty()) {
            throw std::runtime_error("SymTable: pop on empty scope stack");
        }
        scopes_.pop_back();
    }

    bool insert(const Symbol& sym) {
        if (scopes_.empty()) push_scope();
        auto& top = scopes_.back();
        if (top.count(sym.name)) return false;
        top[sym.name] = sym;
        return true;
    }

    std::optional<Symbol> lookup(const std::string& name) const {
        for (int i = static_cast<int>(scopes_.size()) - 1; i >= 0; --i) {
            auto it = scopes_[i].find(name);
            if (it != scopes_[i].end()) return it->second;
        }
        return std::nullopt;
    }

    bool lookup_current_scope(const std::string& name) const {
        if (scopes_.empty()) return false;
        return scopes_.back().count(name) > 0;
    }

    std::optional<Symbol> lookup_in_current_scope(const std::string& name) const {
        if (scopes_.empty()) return std::nullopt;
        auto it = scopes_.back().find(name);
        if (it != scopes_.back().end()) return it->second;
        return std::nullopt;
    }

    int depth() const { return static_cast<int>(scopes_.size()); }

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes_;
};

} // namespace minicc

#endif
