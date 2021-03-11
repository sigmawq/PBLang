//
// Created by swql on 2/12/21.
//

#ifndef LEXER_TEST_TOKEN_H
#define LEXER_TEST_TOKEN_H


enum TOKEN_TYPE : int64_t {
    INVALID, KEYWORD, IDENTIFIER, SPECIAL_SYMBOL, OPERATOR, STRING, CONSTANT, SPECIAL_CHARACTER
};

struct token{
    TOKEN_TYPE type;
    std::string attribute;
};

#endif //LEXER_TEST_TOKEN_H
