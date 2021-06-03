//
// Created by foxfurry on 6/3/21.
//

#ifndef LEXER_TEST_VARIABLEEXPRIR_H
#define LEXER_TEST_VARIABLEEXPRIR_H

#include "ExprIR.h"

class VariableExprIR: public ExprIR {
    std::string var_name = "";
    llvm::Type *var_type = nullptr;

public:
    std::unique_ptr<ExprIR> body;

    VariableExprIR(std::shared_ptr<ast_node> node){
        if(auto variable_signature = node->get_child(VARIABLE_SIGNATURE)){
            if (auto type_spec = variable_signature->get_child(TYPE_SPEC)){
                var_type = str_to_type(type_spec->optional_value->value);
            }
        }
        if(auto ast_just_text = node->get_child(AST_JUST_TEXT)){
            var_name = ast_just_text->optional_value->value;
        }
    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_VARIABLEEXPRIR_H
