#include <gtest/gtest.h>
#include <sstream>
#include "lexer_pascal/lex_pascal.h"
#include "parser/parser.h"
#include "ir/three_addr.h"
#include "vm/vm.h"

using namespace minicc;

static std::string run_program(const std::string& source, const std::string& input = "") {
    auto tokens = lex_pascal(source);
    Parser parser(tokens);
    auto ast = parser.parse();
    if (!ast) throw std::runtime_error("parse failed");

    IRGenerator irgen;
    auto ir = irgen.generate(ast.get());

    std::istringstream in_stream(input);
    std::ostringstream out_stream;
    VM vm(ir, in_stream, out_stream);
    vm.run();

    return out_stream.str();
}

TEST(VM, ReadWriteEcho) {
    std::string output = run_program(
        "begin\n"
        "   integer x;\n"
        "   read(x);\n"
        "   write(x)\n"
        "end\n",
        "42"
    );
    EXPECT_EQ(output, "42\n");
}

TEST(VM, SimpleAssignment) {
    std::string output = run_program(
        "begin\n"
        "   integer x;\n"
        "   x:=7;\n"
        "   write(x)\n"
        "end\n"
    );
    EXPECT_EQ(output, "7\n");
}

TEST(VM, ArithmeticExpression) {
    std::string output = run_program(
        "begin\n"
        "   integer x;\n"
        "   integer y;\n"
        "   x:=10;\n"
        "   y:=x-3;\n"
        "   write(y)\n"
        "end\n"
    );
    EXPECT_EQ(output, "7\n");
}

TEST(VM, Multiplication) {
    std::string output = run_program(
        "begin\n"
        "   integer a;\n"
        "   integer b;\n"
        "   a:=6;\n"
        "   b:=a*7;\n"
        "   write(b)\n"
        "end\n"
    );
    EXPECT_EQ(output, "42\n");
}

TEST(VM, IfStatementThenBranch) {
    std::string output = run_program(
        "begin\n"
        "   integer x;\n"
        "   integer y;\n"
        "   x:=1;\n"
        "   if x<=1 then y:=10 else y:=20;\n"
        "   write(y)\n"
        "end\n"
    );
    EXPECT_EQ(output, "10\n");
}

TEST(VM, IfStatementElseBranch) {
    std::string output = run_program(
        "begin\n"
        "   integer x;\n"
        "   integer y;\n"
        "   x:=5;\n"
        "   if x<=1 then y:=10 else y:=20;\n"
        "   write(y)\n"
        "end\n"
    );
    EXPECT_EQ(output, "20\n");
}

TEST(VM, FactorialFunction) {
    std::string output = run_program(
        "begin\n"
        "   integer m;\n"
        "   integer k;\n"
        "   integer function f(integer n);\n"
        "      begin\n"
        "         integer n;\n"
        "         if n<=0 then f:=1\n"
        "         else f:=n*f(n-1)\n"
        "      end;\n"
        "   read(m);\n"
        "   k:=f(m);\n"
        "   write(k)\n"
        "end\n",
        "5"
    );
    EXPECT_EQ(output, "120\n");
}

TEST(VM, FactorialOfZero) {
    std::string output = run_program(
        "begin\n"
        "   integer m;\n"
        "   integer k;\n"
        "   integer function f(integer n);\n"
        "      begin\n"
        "         integer n;\n"
        "         if n<=0 then f:=1\n"
        "         else f:=n*f(n-1)\n"
        "      end;\n"
        "   read(m);\n"
        "   k:=f(m);\n"
        "   write(k)\n"
        "end\n",
        "0"
    );
    EXPECT_EQ(output, "1\n");
}

TEST(VM, FactorialOf1) {
    std::string output = run_program(
        "begin\n"
        "   integer m;\n"
        "   integer k;\n"
        "   integer function f(integer n);\n"
        "      begin\n"
        "         integer n;\n"
        "         if n<=0 then f:=1\n"
        "         else f:=n*f(n-1)\n"
        "      end;\n"
        "   read(m);\n"
        "   k:=f(m);\n"
        "   write(k)\n"
        "end\n",
        "1"
    );
    EXPECT_EQ(output, "1\n");
}
