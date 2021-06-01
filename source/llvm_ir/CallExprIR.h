//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_CALLEXPRIR_H
#define LEXER_TEST_CALLEXPRIR_H

#include "ExprIR.h"

class CallExprIR: public ExprIR {
    std::string callee;
    std::vector<std::unique_ptr<ExprIR>> args;

public:
    CallExprIR(std::shared_ptr<ast_node> node) {

    }
    llvm::Value *codegen() override;
};


#endif //LEXER_TEST_CALLEXPRIR_H
