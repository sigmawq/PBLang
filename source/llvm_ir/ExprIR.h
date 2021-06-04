//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_EXPRIR_H
#define LEXER_TEST_EXPRIR_H

#include "ir_utility.h"

class ExprIR {
public:
    virtual ~ExprIR() {}
    virtual llvm::Value *codegen() = 0;
};


llvm::Type *str_to_type(std::string str_ty);
std::unique_ptr<ExprIR> LogError(const char *Str);
llvm::Value *LogErrorV(const char *Str);
llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                                std::string VarName, llvm::Type *VarType);


#endif //LEXER_TEST_EXPRIR_H
