#include <iostream>
#include <vector>
#include <unordered_map>

#include "token.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg)
#endif

#include "jump_table.h"
#include "token_reader.h"
#include "grammar.h"
#include "parse_table.h"
#include "parse_string.h"
#include "parse_tree.h"
#include "pbl_utility.h"
#include "parse_operations.h"
#include "ast_node.h"

void tabs_out(int tabs){
    for (int i = 0; i < tabs; i++){
        std::cout << ' ';
    }
}

void tree_out(parse_tree &tree, parse_node &root, int tabs = 0){
    tabs_out(tabs);
    std::cout << root.gu.string_representation;
    if (root.optional_token.has_value()) std::cout << "(type/val: " << root.optional_token.value().type << " | " << root.optional_token.value().attribute << ")";
    std::cout << std::endl;
    tabs += 2;
    for (int i = root.children.size() - 1; i >= 0; i--){
        tree_out(tree, tree.get_node(root.children[i]), tabs);
    }
}

int main() {

    auto tokenizer_data = prepare_tokenizer();

    parse_data pd;
    prepare_parse(pd);

    auto source = pbl_utility::read_file("../test_source_2.pbl");
    source += '\n';
    std::cout << source << std::endl;

    auto parse_tree_array_decl = parse_source(tokenizer_data, pd, source);
    tree_out(parse_tree_array_decl, parse_tree_array_decl.get_root());
    //T_lexer_test();
    //T_grammar_test();
    return 0;
}

