//
// Created by foxfurry on 6/4/21.
//

#ifndef LEXER_TEST_STRINGEXPRIR_H
#define LEXER_TEST_STRINGEXPRIR_H

#include "ExprIR.h"

class StringExprIR: public ExprIR {
    std::string value;
public:
    StringExprIR(std::shared_ptr<ast_node> node){
        value = node->optional_value->value;
    }
    llvm::Value *codegen() override;

};


#endif //LEXER_TEST_STRINGEXPRIR_H
