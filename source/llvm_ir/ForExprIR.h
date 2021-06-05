//
// Created by foxfurry on 6/4/21.
//

#ifndef LEXER_TEST_FOREXPRIR_H
#define LEXER_TEST_FOREXPRIR_H

#include "ExprIR.h"

class ForExprIR: public ExprIR {

public:
    std::string var_name = "";
    std::unique_ptr<ExprIR> start, end, step;

    ForExprIR(std::shared_ptr<ast_node> &node){
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_FOREXPRIR_H
