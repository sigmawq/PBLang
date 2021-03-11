cmake_minimum_required(VERSION 3.17)
project(lexer_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(lexer_test main.cpp token.h grammar.h parse_operations.h jump_table.h token_reader.h parse_table.h parse_string.h parse_tree.h pbl_utility.h)