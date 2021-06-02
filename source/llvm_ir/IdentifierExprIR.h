//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_IDENTIFIEREXPRIR_H
#define LEXER_TEST_IDENTIFIEREXPRIR_H

#include "ExprIR.h"

class IdentifierExprIR: public ExprIR {
    std::string name;
public:
    IdentifierExprIR(std::shared_ptr<ast_node> node) {
        name = node->optional_value->value;

    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_IDENTIFIEREXPRIR_H
