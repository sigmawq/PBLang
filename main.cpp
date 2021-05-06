#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg)
#endif

#include "ast/abstract_syntax_tree.h"
#include "parse_operations.h"
#include "utility/pbl_utility.h"
#include "ast/ast_node.h"
#include "ast/ast_converter.h"

void tree_out(parse_tree const& tree, parse_node const& root, int tabs = 0){
    pbl_utility::tabs_out(tabs);
    std::cout << root.gu.string_representation;
    if (root.optional_token.has_value()) std::cout << "(type/val: " << root.optional_token.value().type << " | " << root.optional_token.value().attribute << ")";
    std::cout << std::endl;
    tabs += 2;
    for (int i = root.children.size() - 1; i >= 0; i--){
        tree_out(tree, tree.get_node_const(root.children[i]), tabs);
    }
}

int main() {

    auto tokenizer_data = prepare_tokenizer();

    parse_data pd;
    prepare_parse(pd);

    auto source = pbl_utility::read_file("../pbl_little_test.pbl");
    source += '\n';
    std::cout << source << std::endl;

    auto parse_tree_array_decl = parse_source(tokenizer_data, pd, source);
    tree_out(parse_tree_array_decl, parse_tree_array_decl.get_root());

    std::cout << "AST converted : " << std::endl;
    auto ast_root = convert_parse_tree_to_ast(parse_tree_array_decl);
    std::cout << ast_root.value()->to_string_recursive() << std::endl;

    return 0;
}

