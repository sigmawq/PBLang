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
        switch (node->node_type) {
            case VAL_INT: val = (int)std::stoi(node->optional_value->value); break;
            case VAL_FP: val = std::stod(node->optional_value->value); break;
        }
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_NUMBEREXPRIR_H
