//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_BINARYEXPRIR_H
#define LEXER_TEST_BINARYEXPRIR_H

#include "ExprIR.h"

class BinaryExprIR: public ExprIR {
    AST_NODE_TYPE op;

public:
    std::unique_ptr<ExprIR> LHS, RHS;

    BinaryExprIR(std::shared_ptr<ast_node> &node){
        op = node->node_type;
    }
    llvm::Value *codegen() override;

    BinaryExprIR();
};


#endif //LEXER_TEST_BINARYEXPRIR_H
