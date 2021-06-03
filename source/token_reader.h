//
// Created by swql on 2/13/21.
//

#ifndef LEXER_TEST_TOKEN_READER_H
#define LEXER_TEST_TOKEN_READER_H

#include <utility>
#include <algorithm>
#include "utility/pbl_utility.h"

// Uses an array of jump tables to tokenize input string.
// The longest lexeme is picked.
// If some jump tables produce token with the same length, the topmost will have priority

class token_reader{
    std::string const& str;
    std::vector<std::reference_wrapper<jump_table>> tables;
    size_t pos = 0;
    std::string err_msg;
    size_t line = 0;
    size_t line_index = 0;

    bool end_of_stream_reached = false;
public:
    token_reader(std::string const& str, std::vector<std::reference_wrapper<jump_table>> jump_tables) : str(str), tables(jump_tables) {}

    bool string_eos_reached() const { return end_of_stream_reached; }

    [[nodiscard]] std::pair<size_t, size_t> get_position()  const { return std::pair<size_t, size_t>{ line, line_index }; };

    // Return an empty optional if stream is invalid according to provided tables
    // Hardcoded to recognize comments & whitespaces and return them as tokens (user of this function should take care of ignoring them)
    std::optional<token> read_next_token(){

        // Goes through @str to increment reader's @line and @line_index
        auto line_go_through = [&](size_t from, size_t to){
            for (size_t i = from; i < to; i++){
                if (str[i] == '\n') {
                    ++line;
                    line_index = 0;
                }
                else ++line_index;
            }
        };

        if (end_of_stream_reached) throw std::runtime_error("String EOS already reached.");

        token resulting_token {};
        size_t next_stream_pos = 0;
        size_t max_token_length = 0;

        bool got_token = false;
        for (const auto &t : tables){
            auto next_table_output = t.get().analyze_string(str, pos);
            if (next_table_output.has_value()){
                got_token = true;
                size_t token_length = next_table_output.value().stream_ended_at - pos;
                if (token_length > max_token_length){
                    resulting_token = next_table_output.value().resulting_token;
                    max_token_length = token_length;
                    next_stream_pos = next_table_output.value().stream_ended_at;
                }
            }
        }



        if (got_token){
            size_t old_pos = pos;
            pos = next_stream_pos;
            if (pos > str.size() - 1) end_of_stream_reached = true;
            line_go_through(old_pos, pos);
            return resulting_token;
        }
        else return {};
    }
};

#endif //LEXER_TEST_TOKEN_READER_H
