//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_BINARYEXPRIR_H
#define LEXER_TEST_BINARYEXPRIR_H

#include "ExprIR.h"

class BinaryExprIR: public ExprIR {
    AST_NODE_TYPE op;
    std::unique_ptr<ExprIR> LHS, RHS;

public:
    BinaryExprIR(std::shared_ptr<ast_node> node){

    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_BINARYEXPRIR_H
