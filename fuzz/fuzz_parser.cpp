#include <cstdint>
#include <cstddef>
#include <string>

#include "lexer_pascal/lex_pascal.h"
#include "parser/parser.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 4096) return 0;
    std::string input(reinterpret_cast<const char*>(data), size);
    try {
        auto tokens = minicc::lex_pascal(input);
        minicc::Parser parser(tokens);
        parser.parse();
    } catch (...) {
        // Parser should not crash on any input
    }
    return 0;
}
