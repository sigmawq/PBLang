//
// Created by swqd on 3/24/21.
//

#ifndef LEXER_TEST_TESTS_H
#define LEXER_TEST_TESTS_H

#include "parse_string.h"

namespace pblang_lexer_parser_tests{
    void T_line_from_string () {
        std::string str { "aaa\n"
                          "bbb" };
        std::cout << get_line_in_string(str, 1);
    }

    void T_main_test_1(){

    }

    void T_hightlight_point_in_string(){
        std::string a { "aaaa\n bbb_point" };
        highlight_error_point(a, 1, 4);
    }
}

#endif //LEXER_TEST_TESTS_H
