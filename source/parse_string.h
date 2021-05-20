//
// Created by swql on 3/4/21.
//

#ifndef LEXER_TEST_PARSE_STRING_H
#define LEXER_TEST_PARSE_STRING_H

#include <list>
#include <stack>
#include <string_view>
#include "grammar.h"
#include "parse_tree.h"
#include "parse_table.h"

void derivation_sequence_out(std::stack<size_t> stack, parse_tree &pt){
    std::cout << " -> ";
    while (!stack.empty()){
        std::cout << pt.get_node(stack.top()).gu.string_representation << " ";
        stack.pop();
    }
    std::cout << "\n";
}

std::string_view get_line_in_string(std::string const& str, int line){
    size_t leftmost_index = 0;

    // Find start of the desired line
    for (int i = 0; i < line; i++){
        while (str[leftmost_index] != '\n'){
            ++leftmost_index;
        }
        ++leftmost_index;
    }

    // Find end of desired line
    size_t rightmost_index = leftmost_index;
    while (str[rightmost_index] != '\n' && rightmost_index < str.size()){
        ++rightmost_index;
    }

    std::string_view desired_line { str.data() + leftmost_index, rightmost_index - leftmost_index};

    return desired_line;
}

void output_spaces(int count){
    for (int i = 0; i < count; i++){
        std::cout << ' ';
    }
}

void highlight_error_point(std::string const& str, int line, int col){
    std::string_view str_line = get_line_in_string(str, line);
    std::cout << str_line << std::endl;
    output_spaces(col);
    std::cout << '^' << std::endl;
}

// Returns a parse tree
parse_tree parse_string(std::vector<bound_token> &token_stream,
                        const grammar_unit *start_symbol, parse_table &pt, std::string const& source_str){

    parse_tree tree { *start_symbol };
    std::stack<size_t> stack;

    stack.emplace(0);

    size_t token_stream_index = 0;

    bool out_of_range_tok_stream = false;
    while (!stack.empty()){
#ifdef DEBUG_MODE_POUT
        derivation_sequence_out(stack, tree);
#endif
        size_t top_stack = stack.top();
        stack.pop();
        parse_node &top_stack_node = tree.get_node(top_stack);

        if (token_stream_index >= token_stream.size()){
            out_of_range_tok_stream = true;
        }

        if (top_stack_node.gu.terminal){
            if (out_of_range_tok_stream) {
                std::string err = "Unexpected end of stream, expected: ";
                pbl_utility::str_compose(err, top_stack_node.gu.string_representation);
                throw std::runtime_error(err);
            }

            if (&(top_stack_node.gu) == token_stream[token_stream_index].gu){
                if ((&top_stack_node)->gu.is_number || (&top_stack_node)->gu.is_identifier || (&top_stack_node)->gu.is_string){
                    top_stack_node.set_token(token_stream[token_stream_index].token_value);
                }
                ++token_stream_index;
                continue;
            }
            else {
                std::string err = "Unexpected token at ";

                pbl_utility::str_compose(err,  "(", std::to_string(token_stream[token_stream_index].token_value.line),
                                                        ",",
                                         std::to_string(token_stream[token_stream_index].token_value.col), ")",
                                         " expected: ",top_stack_node.gu.string_representation, " but found ",
                                         token_stream[token_stream_index].gu->string_representation);
                highlight_error_point(source_str, token_stream[token_stream_index].token_value.line, token_stream[token_stream_index].token_value.col);
                throw std::runtime_error(err);
            }
        }

        else {
            std::optional<predict_set_record> rec;

            if (out_of_range_tok_stream){
                rec = pt.at_dollar(&top_stack_node.gu);
            }
            else {
                rec = pt.at(&top_stack_node.gu, token_stream[token_stream_index].gu);
            }

            if (!rec.has_value()) {
                std::string err;
                auto allowed_derivations = pt.get_all_possible_derivations(&top_stack_node.gu);

                pbl_utility::str_compose(err, "Unexpected token at (");
                if (out_of_range_tok_stream) {
                    pbl_utility::str_compose(err, "EOS", ")");
                }
                else {
                    pbl_utility::str_compose(err, std::to_string(token_stream[token_stream_index].token_value.line), ",",
                                             std::to_string(token_stream[token_stream_index].token_value.col), ") ", "Found ",
                                             token_stream[token_stream_index].token_value.attribute);
                }

                pbl_utility::str_compose(err, ", but expected one of the following {");
                int i = 0;
                for (i; i < allowed_derivations.first.size() - 1; i++){
                    pbl_utility::str_compose(err, allowed_derivations.first[i]->string_representation, ", ");
                }
                pbl_utility::str_compose(err, allowed_derivations.first[i]->string_representation);
                if (allowed_derivations.second) pbl_utility::str_compose(err, ", ", "end of stream");
                pbl_utility::str_compose(err, "}\n");

                if (out_of_range_tok_stream) throw std::runtime_error(err);

                pbl_utility::str_compose(err, " (token index: ", std::to_string(token_stream_index), ")");
                highlight_error_point(source_str, token_stream[token_stream_index].token_value.line, token_stream[token_stream_index].token_value.col);
                throw std::runtime_error(err);
            }
            else {
                if (rec.value().is_epsilon_prod()) continue;

                for (long long i = rec.value().prod->get().content.size() - 1; i >= 0; i--){
                    const grammar_unit *gu = rec.value().prod->get().content[i];

                    size_t this_node = tree.add_grammar_unit_to_parse_node(top_stack, *gu);
                    stack.push(this_node);
                }
            }
        }
    }

    if (token_stream_index < token_stream.size() - 1)
        throw std::runtime_error("Grammar failed to parse this string entirely!");
    return tree;
}

#endif //LEXER_TEST_PARSE_STRING_H
