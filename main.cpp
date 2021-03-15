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
    //std::string source = "1+2;\n 2+1+(2);";
    //std::string source_var_decl = "var float i = 1;";
    //std::string source_fun_decl = "overload def sum(int a, int b : int) { var int a = 1; }";
    std::string source_fun_call = "1 + fcall(1, 1, fcall(1));";

    auto tokenizer_data = prepare_tokenizer();

    parse_data pd;
    prepare_parse(pd);

    //auto parse_tree = parse_source(tokenizer_data, pd, source);
    //auto parse_tree_2 = parse_source(tokenizer_data, pd, source_var_decl);
    //auto parse_tree_fun_decl = parse_source(tokenizer_data, pd, source_fun_decl);
    auto parse_tree_fun_call = parse_source(tokenizer_data, pd, source_fun_call);
    tree_out(parse_tree_fun_call, parse_tree_fun_call.get_root());

    //T_lexer_test();
    //T_grammar_test();
    return 0;
}

