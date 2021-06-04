//
// Created by foxfurry on 6/3/21.
//

#ifndef LEXER_TEST_VARIABLEEXPRIR_H
#define LEXER_TEST_VARIABLEEXPRIR_H

#include "ExprIR.h"

class VariableExprIR: public ExprIR {
    std::string var_name = "";

public:
    std::unique_ptr<ExprIR> body;

    VariableExprIR(std::shared_ptr<ast_node> node){
        if(auto ast_just_text = node->get_child(AST_JUST_TEXT)){
            var_name = ast_just_text->optional_value->value;
        }
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_VARIABLEEXPRIR_H
