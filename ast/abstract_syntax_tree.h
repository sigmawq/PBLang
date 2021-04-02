//
// Created by swqd on 4/2/21.
//

#ifndef LEXER_TEST_ABSTRACT_SYNTAX_TREE_H
#define LEXER_TEST_ABSTRACT_SYNTAX_TREE_H

#include <numeric>
#include <unordered_set>
#include "ast_node.h"
#include "ast_converter.h"
#include "AE_parser.h"
#include <exception>
#include "../parse_tree.h"

using namespace pbl_utility;

class abstract_syntax_tree {
    ast_node root;

public:
    explicit abstract_syntax_tree(parse_tree &parse_tree) {
        ast_converter converter{parse_tree.get_root(), parse_tree, this->root};
        root = converter.convert();
    }
};

#endif //LEXER_TEST_ABSTRACT_SYNTAX_TREE_H
