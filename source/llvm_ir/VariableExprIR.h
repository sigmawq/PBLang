//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_VARIABLEEXPRIR_H
#define LEXER_TEST_VARIABLEEXPRIR_H

#include "ExprIR.h"

class VariableExprIR: public ExprIR {
    std::string name;
public:
    VariableExprIR(std::shared_ptr<ast_node> node) {
        name = "";
        if(auto text_node = node->get_child(AST_JUST_TEXT)){
            name = text_node->optional_value->value;
        }
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_VARIABLEEXPRIR_H
