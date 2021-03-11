//
// Created by swql on 2/12/21.
//

#ifndef LEXER_TEST_TOKEN_H
#define LEXER_TEST_TOKEN_H


enum TOKEN_TYPE : int64_t {
    INVALID, PURE_WS, COMMENT_LINE, KEYWORD, IDENTIFIER, SPECIAL_SYMBOL, OPERATOR, STRING, CONSTANT_INTEGER, CONSTANT_FP, SPECIAL_CHARACTER
};

struct token{
    TOKEN_TYPE type;
    std::string attribute;
};

#endif //LEXER_TEST_TOKEN_H
