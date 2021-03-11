//
// Created by swqd on 3/11/21.
//

#ifndef LEXER_TEST_PBL_UTILITY_H
#define LEXER_TEST_PBL_UTILITY_H

#include <string>

namespace pbl_utility{

    // Returns position of @look_for in @str. Looks from @start_at to @end_at
    // Empty optional if not found
    std::optional<size_t> find_str_symbol(std::string const& str, size_t start_at, size_t end_at, char look_for)
    {
        for (size_t i = start_at; i <= end_at; i++){
            if (str[i] == look_for){
                return i;
            }
        }
        return {};
    }
}

#endif //LEXER_TEST_PBL_UTILITY_H
