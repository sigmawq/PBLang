//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_FUNCPROTOIR_H
#define LEXER_TEST_FUNCPROTOIR_H

#include "ExprIR.h"

class FuncProtoIR{
    std::string name;
    std::vector<std::string> args;

public:
    FuncProtoIR(const std::string &_name, std::vector<std::string> _args)
            : name(_name), args(std::move(_args)) {}

    llvm::Function *codegen();
    const std::string &getName() const { return name; }
};

std::unique_ptr<FuncProtoIR> LogErrorP(const char *Str);

#endif //LEXER_TEST_FUNCPROTOIR_H
