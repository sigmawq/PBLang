//
// Created by swql on 2/13/21.
//

#ifndef LEXER_TEST_JUMP_TABLE_H
#define LEXER_TEST_JUMP_TABLE_H

enum RULE_TYPE {

    // Single char - for example: p will be accepted by state machine
    ONE_CHAR, // [a]

    // Any of provided chars in container will be accepted by state machine, but in sequence
    ANY_OF, // [a, b, c]

    // Same as in previous, but sequence is defined by two limits: a, b; a < b;
    ANY_OF_SEQUENCE, // [a-b]

    // Same as in previous, but multiple sequences are allowed
    ANY_OF_SEQUENCE_AND // [a-b AND c-d AND e-f AND...]
};

enum RULE_QUALIFIER{
    ONE_TIME, // (a)^1
    ZERO_OR_MORE, // (a)*
};

struct rule {
    RULE_TYPE rule;
    RULE_QUALIFIER qualifier;
    std::vector<char> container;
};

class jump_table{
public:

    std::unordered_map<char, std::vector<int64_t>> jump_map;
    std::unordered_map<int64_t, TOKEN_TYPE> accepting_state_token_type = {};
    std::vector<bool> if_is_accepting_state_array = {};
    int64_t state_count = 0;

    int64_t get_transition(const int64_t state, const char terminal) const{
        if (terminal == ' ') return -1;

        try{
            return jump_map.at(terminal)[state];
        }
        catch (std::out_of_range &e){
            return -1;
        }
    }

    TOKEN_TYPE get_accepting_state_token(const int64_t accepting_state) const{
        try{
            return accepting_state_token_type.at(accepting_state);
        }
        catch (std::out_of_range &e){
            throw std::runtime_error("State is not accepting");
        }
    }

    bool has_transition(const int64_t state, const char terminal) const{
        return (get_transition(state, terminal) >= 0);
    }

    bool has_terminal(const char terminal) const{
        try{
            auto res = jump_map.at(terminal);
            return true;
        }
        catch (std::out_of_range &e){
            return false;
        }
    }

    bool is_accepting(const size_t state) const{
        return if_is_accepting_state_array[state];
    }

    void set_transition(const int64_t state, const char terminal, const int64_t next_state){
        try{
            jump_map.at(terminal)[state] = next_state;
        }
        catch (std::out_of_range &e){
            DEBUG_LOG("jump_map out of range");
        }
    }

    void set_accepting(const int64_t state_id, TOKEN_TYPE to_return){
        if_is_accepting_state_array[state_id] = true;
        accepting_state_token_type.at(state_id) = to_return;
    }

    // Returns new state ID
    int64_t add_state(const bool is_accepting, TOKEN_TYPE token_type = INVALID){
        state_count++;
        if_is_accepting_state_array.push_back(is_accepting);
        for (auto& column : jump_map){
            column.second.push_back(-1);
        }

        if (is_accepting){
            if (token_type == INVALID)
                throw std::runtime_error("Error: INVALID (TOKEN_TYPE) may not be used for new"
                                         " accepting state");

            accepting_state_token_type.emplace(state_count - 1, token_type);
        }

        return state_count - 1;
    }

    void add_terminal(const char terminal){
        jump_map.emplace(terminal, std::vector<int64_t>{});
        auto& vec = jump_map.at(terminal);
        vec.resize(state_count);
        std::fill(vec.begin(), vec.end(), -1);

    }

    void add_keyword(const std::string &keyword, const TOKEN_TYPE token_type) {
        if (keyword.empty()) throw std::runtime_error("New lexeme size is 0");

        int64_t current_state = 0;

        // Iterate until the previous of the last char
        char letter;
        for (auto i = 0; i < keyword.size() - 1; i++) {
            letter = keyword[i];
            if (this->has_transition(current_state, letter)) {
                current_state = this->get_transition(current_state, letter);
                continue;
            } else {
                if (!(this->has_terminal(letter))) {
                    add_terminal(letter);
                }
                int64_t new_state_index = add_state(false);
                set_transition(current_state, letter, new_state_index);
                current_state = new_state_index;
            }
        }

        // Set last letter as accepting
        letter = keyword[keyword.size() - 1];
        if (!(this->has_terminal(letter))) {
            add_terminal(letter);
        }
        if (has_transition(current_state, letter)) {
            std::string msg = "Keyword: ";
            msg += keyword;
            msg += " exist";
            throw std::runtime_error(msg);
        }

        int64_t final_state = add_state(true, token_type);
        set_transition(current_state, letter, final_state);
    }

    void add_sequence_of_rules(std::vector<rule> &rules, TOKEN_TYPE output){
        auto add_transition_with_char_group = [&](std::vector<char> &char_grp, int64_t source_state, int64_t target_state){
            for (char c : char_grp){
                this->set_transition(source_state, c, target_state);
            }
        };

        int64_t old_state = 0;
        int64_t current_state = 0;

        std::vector<int64_t> fallthrough_group;

        for (int i = 0; i < rules.size(); i++){
            auto r = rules[i];

            std::vector<char> char_group_allowed;

            current_state = this->add_state(false, INVALID);

            // Resolve allowed characters
            if (r.container.empty()) throw std::runtime_error("One or more rules has empty container");
            switch (r.rule) {
                case ONE_CHAR:
                    if (r.container.size() != 1) throw std::runtime_error("Rule that specified single char used more");
                    char_group_allowed = r.container;
                    break;
                case ANY_OF:
                    char_group_allowed = r.container;
                    break;
                case ANY_OF_SEQUENCE:
                    if (r.container.size() != 2) throw std::runtime_error("A rule is defined by sequence, but it's container"
                                                                          "size is != 2");
                    for (char i = r.container[0]; i < r.container[1]; i++){
                        char_group_allowed.push_back(i);
                    }
                    break;
                case ANY_OF_SEQUENCE_AND:
                    if ((r.container.size() % 2) != 0){
                        throw std::runtime_error("Incomplete sequence in rule that implies multiple sequences");
                    }
                    else{
                        int k = 0;
                        for (char left = r.container[k]; left < r.container[k+1]; left++){
                            char_group_allowed.push_back(left);
                            k += 2;
                        }
                    }
                    break;
                default:
                    throw std::runtime_error("Rule is undefined");
            }

            // Resolve current fallthrough
            // Fallthrough - is the process to going till the last before the last element in fallthrough
            // and connecting them one by
            // one to the current state. The last state is excluded because it's connection is handled in usual way.
            for (int i = 0; i < fallthrough_group.size() - 1; i++){
                add_transition_with_char_group(char_group_allowed, fallthrough_group[i], current_state);
            }

            // Add current state to fallthrough group if is followed by <zero_or_more> or is <zero_or_more>
            // *if next rule is EOS then add current state anyways (because fallthrough will be resolved anyway at
            // the end of the stream)

            if (r.qualifier == ZERO_OR_MORE){
                fallthrough_group.push_back(current_state);
            }
            else {
                if (i != rules.size() - 1){
                    if (rules[i+1].qualifier == ZERO_OR_MORE){
                        fallthrough_group.push_back(current_state);
                    }
                }
            }

            switch (r.qualifier) {
                case ONE_TIME:
                    fallthrough_group.clear();
                    add_transition_with_char_group(char_group_allowed, old_state, current_state);
                    break;
                case ZERO_OR_MORE:
                    add_transition_with_char_group(char_group_allowed, old_state, current_state);
                    add_transition_with_char_group(char_group_allowed, current_state, current_state);
                    break;
            }
            old_state = current_state;
        }

        // Resolve EOS fallthrough - mark every state present there as accepting, meaning any state can "fall" through
        // to the end of stream.
        for (auto state : fallthrough_group){
            this->set_accepting(state, output);
        }
    }

    jump_table() {
        // S0 - initial state
        add_state(false);
    }
};

#endif //LEXER_TEST_JUMP_TABLE_H
