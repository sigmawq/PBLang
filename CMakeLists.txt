cmake_minimum_required(VERSION 3.17)
project(lexer_test)

set(CMAKE_CXX_STANDARD 17)

SET(GCC_COVERAGE_COMPILE_FLAGS "-O3" "-Wreturn-type")
add_executable(lexer_test main.cpp token.h grammar.h parse_operations.h jump_table.h token_reader.h parse_table.h parse_string.h parse_tree.h utility/pbl_utility.h ast/ast_node.h tests.h ast/ast.h ast/AE_parser.h utility/advanced_stack.h ast/abstract_syntax_tree.h ast/ast_converter.h)