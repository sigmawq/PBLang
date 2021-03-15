#ifndef LEXER_TEST_AST_NODE_H
#define LEXER_TEST_AST_NODE_H

#include "parse_tree.h"

enum AST_NODE_TYPE{
    // For error checking
        AST_INVALID,

    // Math
        // Values
        VAL_INT, VAL_FP,

        // Operators (not unary)
        ADD, SUB, DIV, MUL, POW,
};

enum AST_NODE_TYPE_SPECIFIER{
    DEFAULT, HAS_UNARY_OP
};

struct ast_node{
    AST_NODE_TYPE node_type;
    AST_NODE_TYPE_SPECIFIER node_specifier;
    std::string value; // AST_node MAY not have a value (example: if statement node will have just a node_type, value is relevant only for some things)
    std::vector<std::shared_ptr<ast_node>> children;
};

#endif //LEXER_TEST_AST_NODE_H
