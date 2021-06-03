//
// Created by swqd on 4/8/21.
//

#ifndef LEXER_TEST_OPT_AST_NODE_CONTAINER_H
#define LEXER_TEST_OPT_AST_NODE_CONTAINER_H

#include <vector>
#include "ast_node.h"

struct opt_ast_node_container{
private:

    std::vector<std::reference_wrapper<ast_node>> container;

public:

    void add_node(ast_node &node);

    void merge(opt_ast_node_container const& other);

    [[nodiscard]] bool has_nodes() const { return !container.empty(); }

};


#endif //LEXER_TEST_OPT_AST_NODE_CONTAINER_H
