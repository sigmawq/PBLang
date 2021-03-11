//
// Created by swql on 2/13/21.
//

#ifndef LEXER_TEST_TOKEN_READER_H
#define LEXER_TEST_TOKEN_READER_H

#include <utility>
#include <algorithm>

class token_reader {
    std::string string_to_read;
    size_t position = 0;

    std::vector<jump_table> jump_table_group;

    // Returns token for provided jump table and first-after position in given string
    static std::pair<TOKEN_TYPE, int64_t>
    get_token_type(const jump_table &jt, const std::string &str, const size_t from){

        // Returns first accepting state found while iterating backwards.
        auto unwind_transition_sequence = [&jt](const std::vector<int64_t> &transition_sequence, size_t &index) -> int64_t{
            for (int64_t i = transition_sequence.size() - 1; i >= 0; i--){
                if (jt.is_accepting(transition_sequence[i])){
                    return transition_sequence[i];
                }
                else index--;
            }
            return -1;
        };

        std::vector<int64_t> transition_sequence;
        size_t index = from;
        int64_t current_state = 0;
        transition_sequence.push_back(current_state);

        while (index < str.size()){
            current_state = jt.get_transition(current_state, str[index]);
            if (current_state == -1){
                break;
            }
            transition_sequence.push_back(current_state);
            index++;
        }

        int64_t accepting_state = unwind_transition_sequence(transition_sequence, index);
        if (accepting_state == -1){
            return std::make_pair<TOKEN_TYPE, int64_t>(INVALID, index);
        }
        else{
            return std::make_pair<TOKEN_TYPE, int64_t>(jt.get_accepting_state_token(accepting_state), index);
        }
    }

    // Involve several jump tables to calculate next token. The longest lexeme will be chosen.
    // Returns new position in the string
    [[nodiscard]] std::pair<token, size_t> jump_table_compete(size_t start) const{

        token tok { INVALID, ""};
        size_t max_token_position = 0;

        for (const auto& jt : jump_table_group){
            auto next = get_token_type(jt, string_to_read, position);
            if (next.second > max_token_position){
                max_token_position = next.second;

                auto str_offset = (size_t)next.second - position;
                tok = {next.first, std::string { string_to_read, position, str_offset }};
            }
        }

        return std::pair<token, size_t> {tok, max_token_position};
    }

public:

    [[nodiscard]] bool if_still_got_stuff() const{
        return (position < string_to_read.size());
    }

    void reset_position() { position = 0; }

    // Throws std::runtime_error if an invalid token has been encountered
    token next_token() {
        auto result = jump_table_compete(position);

        if (result.first.type == INVALID) {
            std::string msg = "Invalid token at ";
            msg += std::to_string(position);
            msg += " (Start symbol ";
            msg += string_to_read[position];
            msg += ")";
            throw std::runtime_error(msg);
        }

        position = result.second;

        // Advance to next non-whitespace symbol
        bool still_has_symbols = false;
        for (position; position < string_to_read.size(); position++){
            if (string_to_read[position] != ' ') {
                still_has_symbols = true;
                break;
            }
        }
        if (!still_has_symbols) position = string_to_read.size();
        return result.first;
    }
public:
    token_reader(const std::string &str, const std::vector<jump_table> &jump_table_group)
    : string_to_read(str), jump_table_group(jump_table_group), position(0)
    {}

    // TODO: Remove comments
};

#endif //LEXER_TEST_TOKEN_READER_H
