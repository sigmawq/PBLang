//
// Created by foxfurry on 6/4/21.
//

#ifndef LEXER_TEST_IFEXPRIR_H
#define LEXER_TEST_IFEXPRIR_H

#include "ExprIR.h"

class IfExprIR: ExprIR {

public:
    llvm::Value * if_cond,* if_true, * if_false;
    IfExprIR(std::shared_ptr<ast_node> node){

    }

    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_IFEXPRIR_H
