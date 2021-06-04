cmake_minimum_required(VERSION 3.17)
project(pbl-compiler)

include_directories(/usr/include/llvm-11/)
include_directories(/usr/include/llvm-c-11/)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

set(CMAKE_CXX_STANDARD 17 )
add_executable(${PROJECT_NAME} main.cpp source/token.h source/grammar.h source/parse_operations.h source/jump_table.h source/token_reader.h source/parse_table.h source/parse_string.h source/parse_tree.h source/utility/pbl_utility.h source/ast/ast_node.h source/tests.h source/ast/AE_parser.h source/ast/ast_converter.h source/ast/ast_converter.cpp source/ast/abstract_syntax_tree.h source/ast/abstract_syntax_tree.cpp source/ast/conversion_jump_map.cpp source/ast/conversion_jump_map.h source/ast/opt_ast_node_container.cpp source/ast/opt_ast_node_container.h source/compile.h source/utility/scoped_timer.h source/llvm_ir/llvm_ir.h source/llvm_ir/ExprIR.cpp source/llvm_ir/ExprIR.h source/llvm_ir/NumberExprIR.cpp source/llvm_ir/NumberExprIR.h source/llvm_ir/ir_utility.h source/llvm_ir/IdentifierExprIR.cpp source/llvm_ir/IdentifierExprIR.h source/llvm_ir/BinaryExprIR.cpp source/llvm_ir/BinaryExprIR.h source/llvm_ir/CallExprIR.cpp source/llvm_ir/CallExprIR.h source/llvm_ir/FuncProtoIR.cpp source/llvm_ir/FuncProtoIR.h source/llvm_ir/FuncIR.cpp source/llvm_ir/FuncIR.h source/llvm_ir/VariableExprIR.cpp source/llvm_ir/VariableExprIR.h source/llvm_ir/IfExprIR.cpp source/llvm_ir/IfExprIR.h)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.o")
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(${PROJECT_NAME} ${llvm_libs})
