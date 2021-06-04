//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_NUMBEREXPRIR_H
#define LEXER_TEST_NUMBEREXPRIR_H

#include "ExprIR.h"

class NumberExprIR : public ExprIR{
    double val;

public:
    NumberExprIR(std::shared_ptr<ast_node> node){
        val = std::stod(node->optional_value->value);
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_NUMBEREXPRIR_H
