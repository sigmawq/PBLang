//
// Created by swqd on 4/2/21.
//

#ifndef LEXER_TEST_ABSTRACT_SYNTAX_TREE_H
#define LEXER_TEST_ABSTRACT_SYNTAX_TREE_H

#include "../parse_tree.h"
#include "ast_node.h"

using namespace pbl_utility;

class abstract_syntax_tree {
    std::vector<ast_node> nodes;

public:
    ast_node& add_node(AST_NODE_TYPE type);
    ast_node& add_node();
};

#endif //LEXER_TEST_ABSTRACT_SYNTAX_TREE_H
