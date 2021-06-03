//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_FUNCIR_H
#define LEXER_TEST_FUNCIR_H

#include "FuncProtoIR.h"
#include "ExprIR.h"

class FuncIR {
    std::unique_ptr<FuncProtoIR> proto;
    std::shared_ptr<ast_node> &node;
public:
    FuncIR(std::shared_ptr<ast_node> &_node): node(_node) {
        proto = std::make_unique<FuncProtoIR>();
    }
    llvm::Function *codegen();
};


#endif //LEXER_TEST_FUNCIR_H
