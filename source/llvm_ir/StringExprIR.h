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
        value = value.substr(1,value.size()-2);

    }
    llvm::Value *codegen() override;

};


#endif //LEXER_TEST_STRINGEXPRIR_H
