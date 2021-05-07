//
// Created by swqd on 5/7/21.
//

#ifndef LEXER_TEST_COMPILE_H
#define LEXER_TEST_COMPILE_H

#include "parse_operations.h"

static void compile(std::string const& path) {
    // Run Tokenizer
    auto tokenizer_data = prepare_tokenizer();
    parse_data pd;
    prepare_parse(pd);

    auto source = pbl_utility::read_file(path);
    source += '\n';

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Source file: " << std::endl;
    std::cout << source << std::endl;
#endif

    // Run Parser
    auto parse_tree_array_decl = parse_source(tokenizer_data, pd, source);

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Parsed tree: " << std::endl;
    tree_out(parse_tree_array_decl, parse_tree_array_decl.get_root());
#endif

    // Run AST Converter
    auto ast_root = convert_parse_tree_to_ast(parse_tree_array_decl);

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] AST converted : " << std::endl;
    std::cout << ast_root.value()->to_string_recursive() << std::endl;
#endif
}

#endif //LEXER_TEST_COMPILE_H
