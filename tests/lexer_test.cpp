#include <gtest/gtest.h>
#include "lexer_pascal/lex_pascal.h"

using namespace minicc;

TEST(LexerPascal, KeywordsRecognized) {
    auto tokens = lex_pascal("begin end integer if then else function read write");
    // Expect 9 keywords + EOLN? No newline -> just EOF
    // Actually no newlines so just keywords + EOF
    ASSERT_GE(tokens.size(), 10u);
    EXPECT_EQ(tokens[0].kind, TokenKind::BEGIN);
    EXPECT_EQ(tokens[1].kind, TokenKind::END);
    EXPECT_EQ(tokens[2].kind, TokenKind::INTEGER);
    EXPECT_EQ(tokens[3].kind, TokenKind::IF);
    EXPECT_EQ(tokens[4].kind, TokenKind::THEN);
    EXPECT_EQ(tokens[5].kind, TokenKind::ELSE);
    EXPECT_EQ(tokens[6].kind, TokenKind::FUNCTION);
    EXPECT_EQ(tokens[7].kind, TokenKind::READ);
    EXPECT_EQ(tokens[8].kind, TokenKind::WRITE);
    EXPECT_EQ(tokens.back().kind, TokenKind::END_OF_FILE);
}

TEST(LexerPascal, IdentifiersAndConstants) {
    auto tokens = lex_pascal("x 42 abc123");
    // x=SYMBOL, 42=CONSTANT, abc123=SYMBOL, EOF
    ASSERT_GE(tokens.size(), 4u);
    EXPECT_EQ(tokens[0].kind, TokenKind::SYMBOL);
    EXPECT_EQ(tokens[0].text, "x");
    EXPECT_EQ(tokens[1].kind, TokenKind::CONSTANT);
    EXPECT_EQ(tokens[1].text, "42");
    EXPECT_EQ(tokens[2].kind, TokenKind::SYMBOL);
    EXPECT_EQ(tokens[2].text, "abc123");
}

TEST(LexerPascal, Operators) {
    auto tokens = lex_pascal("<= <> < >= > = := - * ( ) ;");
    ASSERT_GE(tokens.size(), 12u);
    EXPECT_EQ(tokens[0].kind, TokenKind::LE);
    EXPECT_EQ(tokens[1].kind, TokenKind::NE);
    EXPECT_EQ(tokens[2].kind, TokenKind::LT);
    EXPECT_EQ(tokens[3].kind, TokenKind::GE);
    EXPECT_EQ(tokens[4].kind, TokenKind::GT);
    EXPECT_EQ(tokens[5].kind, TokenKind::EQ);
    EXPECT_EQ(tokens[6].kind, TokenKind::ASSIGN);
    EXPECT_EQ(tokens[7].kind, TokenKind::SUB);
    EXPECT_EQ(tokens[8].kind, TokenKind::MUL);
    EXPECT_EQ(tokens[9].kind, TokenKind::LPAREN);
    EXPECT_EQ(tokens[10].kind, TokenKind::RPAREN);
    EXPECT_EQ(tokens[11].kind, TokenKind::SEMI);
}

TEST(LexerPascal, NewlinesTracked) {
    auto tokens = lex_pascal("begin\n  integer x\n");
    // begin, EOLN, integer, x, EOLN, EOF
    int eoln_count = 0;
    for (const auto& t : tokens) {
        if (t.kind == TokenKind::EOLN) eoln_count++;
    }
    EXPECT_EQ(eoln_count, 2);
}

TEST(LexerPascal, LexConcatBugFixed) {
    // The old lex_concat pushed the counter index instead of the character.
    // This test verifies that identifiers are correctly built from characters.
    auto tokens = lex_pascal("abc");
    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].kind, TokenKind::SYMBOL);
    EXPECT_EQ(tokens[0].text, "abc");  // NOT garbled or empty
}

TEST(LexerPascal, EmptyInput) {
    auto tokens = lex_pascal("");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::END_OF_FILE);
}

TEST(LexerPascal, LineNumbers) {
    auto tokens = lex_pascal("begin\ninteger x;\nend");
    // 'end' should be on line 3
    for (const auto& t : tokens) {
        if (t.kind == TokenKind::END && t.text == "end") {
            EXPECT_EQ(t.line, 3);
        }
    }
}
