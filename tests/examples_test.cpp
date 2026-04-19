#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include "lexer_pascal/lex_pascal.h"
#include "parser/parser.h"
#include "ir/three_addr.h"
#include "vm/vm.h"

using namespace minicc;

static std::string run_file(const std::string& path, const std::string& input = "") {
    auto tokens = lex_pascal_file(path);
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_TRUE(ast != nullptr) << "parse failed for " << path;
    if (!ast) return "";

    EXPECT_TRUE(parser.errors().empty()) << "parse errors in " << path;

    IRGenerator irgen;
    auto ir = irgen.generate(ast.get());

    std::istringstream in_stream(input);
    std::ostringstream out_stream;
    VM vm(ir, in_stream, out_stream);
    vm.run();

    return out_stream.str();
}

TEST(Examples, Factorial5) {
    EXPECT_EQ(run_file("examples/factorial.pas", "5"), "120\n");
}

TEST(Examples, Factorial0) {
    EXPECT_EQ(run_file("examples/factorial.pas", "0"), "1\n");
}

TEST(Examples, HelloEcho) {
    EXPECT_EQ(run_file("examples/hello.pas", "42"), "42\n");
}

TEST(Examples, GCD_48_18) {
    EXPECT_EQ(run_file("examples/gcd.pas", "48 18"), "6\n");
}

TEST(Examples, GCD_100_75) {
    EXPECT_EQ(run_file("examples/gcd.pas", "100 75"), "25\n");
}

TEST(Examples, GCD_7_3) {
    EXPECT_EQ(run_file("examples/gcd.pas", "7 3"), "1\n");
}

TEST(Examples, Power2_10) {
    EXPECT_EQ(run_file("examples/power.pas", "2 10"), "1024\n");
}

TEST(Examples, Power3_4) {
    EXPECT_EQ(run_file("examples/power.pas", "3 4"), "81\n");
}

TEST(Examples, Power5_0) {
    EXPECT_EQ(run_file("examples/power.pas", "5 0"), "1\n");
}

TEST(Examples, Sieve10) {
    EXPECT_EQ(run_file("examples/sieve.pas", "10"), "2\n3\n5\n7\n");
}

TEST(Examples, Sieve20) {
    EXPECT_EQ(run_file("examples/sieve.pas", "20"),
              "2\n3\n5\n7\n11\n13\n17\n19\n");
}

TEST(Examples, Sieve2) {
    EXPECT_EQ(run_file("examples/sieve.pas", "2"), "2\n");
}

TEST(Examples, Sort3_1_2) {
    EXPECT_EQ(run_file("examples/sort.pas", "3 1 2"), "1\n2\n3\n");
}

TEST(Examples, Sort5_5_5) {
    EXPECT_EQ(run_file("examples/sort.pas", "5 5 5"), "5\n5\n5\n");
}

TEST(Examples, Sort9_1_4) {
    EXPECT_EQ(run_file("examples/sort.pas", "9 1 4"), "1\n4\n9\n");
}
