#include <gtest/gtest.h>
#include "lexer_pascal/lex_pascal.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "ast/ast_print.h"
#include <fstream>
#include <sstream>

using namespace minicc;

TEST(Parser, SimpleProgram) {
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
    EXPECT_EQ(ast->kind, NodeKind::Program);
    EXPECT_TRUE(parser.errors().empty()) << "Parse errors: " << parser.errors().size();
}

TEST(Parser, FunctionDeclaration) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer m;\n"
        "   integer function f(integer n);\n"
        "      begin\n"
        "         integer n;\n"
        "         f:=1\n"
        "      end;\n"
        "   read(m);\n"
        "   write(m)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->kind, NodeKind::Program);

    // Should have DeclList and StmtList as children
    ASSERT_GE(ast->children.size(), 2u);

    // DeclList should contain a VarDecl and a FuncDef
    auto* decl_list = ast->children[0].get();
    ASSERT_NE(decl_list, nullptr);
    EXPECT_EQ(decl_list->kind, NodeKind::DeclList);

    bool has_func = false;
    for (const auto& ch : decl_list->children) {
        if (ch->kind == NodeKind::FuncDef) {
            has_func = true;
            EXPECT_EQ(ch->text, "f");
        }
    }
    EXPECT_TRUE(has_func);
}

TEST(Parser, IfStatement) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   integer y;\n"
        "   if x<=y then write(x) else write(y)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    EXPECT_TRUE(parser.errors().empty());
}

TEST(Parser, AssignmentWithExpression) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer a;\n"
        "   integer b;\n"
        "   a:=b*2-1\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    EXPECT_TRUE(parser.errors().empty()) << "Errors: " << parser.errors().size();
}

TEST(Parser, XmlOutput) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   write(x)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    std::string xml = ast_to_xml(ast.get());
    EXPECT_NE(xml.find("<Program"), std::string::npos);
    EXPECT_NE(xml.find("</Program>"), std::string::npos);
}

TEST(Parser, JsonOutput) {
    auto tokens = lex_pascal(
        "begin\n"
        "   integer x;\n"
        "   write(x)\n"
        "end\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    std::string json = ast_to_json(ast.get());
    EXPECT_NE(json.find("\"kind\": \"Program\""), std::string::npos);
}

TEST(Parser, GoldenFileHello) {
    // Parse the golden hello.pas and compare XML output
    std::string golden_path = "tests/golden/hello.pas";
    std::string expected_path = "tests/golden/hello.expected.xml";

    std::vector<Token> tokens;
    try {
        tokens = lex_pascal_file(golden_path);
    } catch (...) {
        GTEST_SKIP() << "Golden file not found: " << golden_path;
    }

    Parser parser(tokens);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    std::string actual_xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                             + ast_to_xml(ast.get());

    std::ifstream exp_file(expected_path);
    if (!exp_file.is_open()) {
        GTEST_SKIP() << "Expected file not found: " << expected_path;
    }
    std::ostringstream ss;
    ss << exp_file.rdbuf();
    std::string expected_xml = ss.str();

    EXPECT_EQ(actual_xml, expected_xml);
}
