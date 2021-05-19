cmake_minimum_required(VERSION 3.17)
project(pbl-compiler)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

set(CMAKE_CXX_STANDARD 17)
add_executable(pbl-compiler main.cpp token.h grammar.h parse_operations.h jump_table.h token_reader.h parse_table.h parse_string.h parse_tree.h utility/pbl_utility.h ast/ast_node.h tests.h ast/AE_parser.h ast/ast_converter.h ast/ast_converter.cpp ast/abstract_syntax_tree.h ast/abstract_syntax_tree.cpp ast/conversion_jump_map.cpp ast/conversion_jump_map.h ast/opt_ast_node_container.cpp ast/opt_ast_node_container.h compile.h utility/scoped_timer.h)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "pbl-compiler.o")
