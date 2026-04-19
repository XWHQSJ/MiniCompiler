#include <gtest/gtest.h>
#include "lexer_pascal/lex_pascal.h"
#include "parser/parser.h"
#include "ir/three_addr.h"
#include "ir/semantic.h"

using namespace minicc;

TEST(Semantic, UndefinedVariable) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   y:=1\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer sema;
    auto errors = sema.analyze(ast.get());
    EXPECT_GE(errors.size(), 1u);

    bool found_undef = false;
    for (const auto& e : errors) {
        if (e.message.find("undefined") != std::string::npos) {
            found_undef = true;
        }
    }
    EXPECT_TRUE(found_undef);
}

TEST(Semantic, RedefinedVariable) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   integer x;\n"
        "   write(x)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer sema;
    auto errors = sema.analyze(ast.get());
    bool found_redef = false;
    for (const auto& e : errors) {
        if (e.message.find("redefined") != std::string::npos) {
            found_redef = true;
        }
    }
    EXPECT_TRUE(found_redef);
}

TEST(Semantic, ValidProgram) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   read(x);\n"
        "   write(x)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer sema;
    auto errors = sema.analyze(ast.get());
    EXPECT_TRUE(errors.empty()) << "Unexpected semantic errors: " << errors.size();
}

TEST(IR, SimpleAssignment) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   x:=42\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    IRGenerator irgen;
    auto quads = irgen.generate(ast.get());
    EXPECT_GT(quads.size(), 0u);

    // Should contain a LOAD_CONST with 42
    bool found_42 = false;
    for (const auto& q : quads) {
        if (q.op == Op::LOAD_CONST && q.src1 == "42") {
            found_42 = true;
        }
    }
    EXPECT_TRUE(found_42);
}

TEST(IR, ReadWrite) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   read(x);\n"
        "   write(x)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    IRGenerator irgen;
    auto quads = irgen.generate(ast.get());

    bool has_read = false;
    bool has_write = false;
    for (const auto& q : quads) {
        if (q.op == Op::READ) has_read = true;
        if (q.op == Op::WRITE) has_write = true;
    }
    EXPECT_TRUE(has_read);
    EXPECT_TRUE(has_write);
}

TEST(IR, DumpFormat) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   x:=1\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    IRGenerator irgen;
    irgen.generate(ast.get());
    std::string dump = irgen.dump();
    EXPECT_NE(dump.find("FUNC_BEGIN"), std::string::npos);
    EXPECT_NE(dump.find("FUNC_END"), std::string::npos);
}
