//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_IDENTIFIEREXPRIR_H
#define LEXER_TEST_IDENTIFIEREXPRIR_H

#include "ExprIR.h"

class IdentifierExprIR: public ExprIR {
    std::string name;

public:
    bool isFuncCall = false;
    std::vector<std::unique_ptr<ExprIR>> args;
    IdentifierExprIR(std::shared_ptr<ast_node> node) {
        name = node->optional_value->value;
    }

    std::string getName() { return name; }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_IDENTIFIEREXPRIR_H
