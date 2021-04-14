//
// Created by swqd on 4/4/21.
//

#include "abstract_syntax_tree.h"

ast_node &abstract_syntax_tree::add_node(AST_NODE_TYPE type) {
    auto &node = nodes.emplace_back();
    node.node_type = type;
    return node;
}

ast_node &abstract_syntax_tree::add_node() {
    return nodes.emplace_back();
}

