#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "../lexer_pascal/lex_pascal.h"
#include "../parser/parser.h"
#include "../ast/ast.h"
#include "../ast/ast_print.h"
#include "../ir/semantic.h"
#include "../ir/three_addr.h"
#include "../vm/vm.h"

static void usage() {
    std::cerr << "Usage: minicc <file.pas> [-o output] [--json] [--ir] [--run]\n"
              << "\n"
              << "Flags:\n"
              << "  -o FILE    Write AST output to FILE (default: stdout)\n"
              << "  --json     Output AST as JSON instead of XML\n"
              << "  --ir       Print three-address IR\n"
              << "  --run      Run the program in the VM after compilation\n"
              << "  --tokens   Print token stream only\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    std::string input_file;
    std::string output_file;
    bool json_mode = false;
    bool show_ir = false;
    bool run_mode = false;
    bool tokens_only = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (std::strcmp(argv[i], "--json") == 0) {
            json_mode = true;
        } else if (std::strcmp(argv[i], "--ir") == 0) {
            show_ir = true;
        } else if (std::strcmp(argv[i], "--run") == 0) {
            run_mode = true;
        } else if (std::strcmp(argv[i], "--tokens") == 0) {
            tokens_only = true;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: no input file specified\n";
        usage();
        return 1;
    }

    // Step 1: Lex
    std::vector<minicc::Token> tokens;
    try {
        tokens = minicc::lex_pascal_file(input_file);
    } catch (const std::exception& e) {
        std::cerr << "Lexer error: " << e.what() << "\n";
        return 1;
    }

    if (tokens_only) {
        for (const auto& t : tokens) {
            std::cout << t.text << "\t\t" << static_cast<int>(t.kind) << "\n";
        }
        return 0;
    }

    // Step 2: Parse
    minicc::Parser parser(tokens);
    auto ast = parser.parse();

    if (!parser.errors().empty()) {
        std::cerr << "Parse errors:\n";
        for (const auto& e : parser.errors()) {
            std::cerr << "  line " << e.line << ": " << e.message << "\n";
        }
    }

    if (!ast) {
        std::cerr << "Error: failed to produce AST\n";
        return 1;
    }

    // Step 3: Semantic analysis
    minicc::SemanticAnalyzer sema;
    auto sem_errors = sema.analyze(ast.get());
    if (!sem_errors.empty()) {
        std::cerr << "Semantic errors:\n";
        for (const auto& e : sem_errors) {
            std::cerr << "  line " << e.line << ": " << e.message << "\n";
        }
    }

    // Step 4: Output AST
    std::string ast_output;
    if (json_mode) {
        ast_output = minicc::ast_to_json(ast.get()) + "\n";
    } else {
        ast_output = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                     + minicc::ast_to_xml(ast.get());
    }

    if (!output_file.empty()) {
        std::ofstream out(output_file);
        if (!out.is_open()) {
            std::cerr << "Error: cannot write to " << output_file << "\n";
            return 1;
        }
        out << ast_output;
    } else if (!show_ir && !run_mode) {
        std::cout << ast_output;
    }

    // Step 5: Generate IR
    minicc::IRGenerator irgen;
    auto ir = irgen.generate(ast.get());

    if (show_ir) {
        std::cout << irgen.dump();
    }

    // Step 6: Run VM
    if (run_mode) {
        try {
            minicc::VM vm(ir);
            vm.run();
        } catch (const std::exception& e) {
            std::cerr << "VM runtime error: " << e.what() << "\n";
            return 1;
        }
    }

    return parser.errors().empty() && sem_errors.empty() ? 0 : 1;
}
