cmake_minimum_required(VERSION 3.16)
project(MiniCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------
# Core library: shared code for all targets
# ---------------------------------------------------------------
add_library(minicc_core STATIC
    src/lexer_pascal/lex_pascal.cpp
    src/ast/ast.cpp
    src/ast/ast_print.cpp
    src/parser/parser.cpp
    src/ir/three_addr.cpp
    src/ir/semantic.cpp
    src/vm/vm.cpp
)
target_include_directories(minicc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------
# Unified CLI driver
# ---------------------------------------------------------------
add_executable(minicc src/driver/main.cpp)
target_link_libraries(minicc PRIVATE minicc_core)

# ---------------------------------------------------------------
# Legacy module targets (original code, standalone)
# ---------------------------------------------------------------

# LexAnalyze -- C-subset lexer
add_executable(lex_c LexAnalyze/lexAnalyze.cpp)
target_compile_features(lex_c PRIVATE cxx_std_17)

# Scanner -- Pascal-like lexer (original, uses hardcoded paths -> now parameterised)
# Note: this compiles the original Scanner code as-is for reference
# add_executable(scanner Scanner/Scanner/lexAnalyze.cpp)

# PreCompiler -- C preprocessor
add_executable(precompiler PreCompiler/preCompiler.cpp)
target_compile_features(precompiler PRIVATE cxx_std_17)

# Parser (renamed from Parase)
# add_executable(parser_legacy Parser/Parase/paraser.cpp)

# Syntaxer
# add_executable(syntaxer Syntaxer/Syntaxer/syntaxAnalyze.cpp)

# ---------------------------------------------------------------
# Tests
# ---------------------------------------------------------------
option(MINICC_BUILD_TESTS "Build tests" ON)

if(MINICC_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(minicc_tests
        tests/lexer_test.cpp
        tests/parser_test.cpp
        tests/ir_test.cpp
        tests/vm_test.cpp
    )
    target_link_libraries(minicc_tests PRIVATE minicc_core GTest::gtest_main)
    target_include_directories(minicc_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    include(GoogleTest)
    gtest_discover_tests(minicc_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
