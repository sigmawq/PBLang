//
// Created by swqd on 3/11/21.
//

#ifndef LEXER_TEST_PBL_UTILITY_H
#define LEXER_TEST_PBL_UTILITY_H

#include <string>
#include <fstream>
#include <sstream>

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

    void _str_compose(std::string& str) { }

    template<typename Head, typename ...Tail>
    void _str_compose(std::string& str, Head h, Tail... t){
        str += h;
        _str_compose(str, t...);
    }

    template<typename ...Args>
    void str_compose(std::string& str, Args... args){
        _str_compose(str, args...);
    }

    template<typename ...Args>
    void debug_print(std::string& str, Args... args){
#ifdef DEBUG_MODE
        std::string message;
        str_compose(message, args...);
        std::cout << message;
#endif
    }

    template<typename ...Args>
    void debug_print_nl(Args... args){
#ifdef DEBUG_MODE
        std::string message;
        str_compose(message, args...);
        std::cout << message << std::endl;
#endif
    }

    // Read content of file at @path to string
    static std::string read_file(std::string const& path){
        std::ifstream file { path };

        if (!file.is_open()) throw std::runtime_error("Failed to open file");

        // Read file
        std::stringstream result;
        result << file.rdbuf();

        return result.str();
    }
}

#endif //LEXER_TEST_PBL_UTILITY_H
