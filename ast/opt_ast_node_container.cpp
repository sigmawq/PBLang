//
// Created by swqd on 4/8/21.
//

#include "opt_ast_node_container.h"

void opt_ast_node_container::add_node(ast_node &node) {
    this->container.emplace_back(node);
}

void opt_ast_node_container::merge(const opt_ast_node_container &other) {
    for (const auto& thing : other.container){
        container.emplace_back(thing);
    }
}
