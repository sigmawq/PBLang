cmake_minimum_required(VERSION 3.17)
project(pbl-compiler)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

set(CMAKE_CXX_STANDARD 17 )
add_executable(${PROJECT_NAME} main.cpp token.h grammar.h parse_operations.h jump_table.h token_reader.h parse_table.h parse_string.h parse_tree.h utility/pbl_utility.h ast/ast_node.h tests.h ast/AE_parser.h ast/ast_converter.h ast/ast_converter.cpp ast/abstract_syntax_tree.h ast/abstract_syntax_tree.cpp ast/conversion_jump_map.cpp ast/conversion_jump_map.h ast/opt_ast_node_container.cpp ast/opt_ast_node_container.h compile.h utility/scoped_timer.h)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.o")
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(${PROJECT_NAME} ${llvm_libs})