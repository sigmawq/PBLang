//
// Created by swql on 3/4/21.
//

#ifndef LEXER_TEST_PARSE_STRING_H
#define LEXER_TEST_PARSE_STRING_H

#include <list>
#include <stack>
#include "grammar.h"
#include "parse_tree.h"

// Returns a parse tree
parse_tree parse_string(std::vector<bound_token> &token_stream,
                        const grammar_unit *start_symbol, parse_table &pt){

    parse_tree tree { *start_symbol };
    std::stack<size_t> stack;

    stack.emplace(0);

    size_t token_stream_index = 0;

    bool out_of_range_tok_stream = false;
    while (!stack.empty()){
        size_t top_stack = stack.top();
        stack.pop();
        parse_node &top_stack_node = tree.get_node(top_stack);

        if (token_stream_index >= token_stream.size()){
            out_of_range_tok_stream = true;
        }

        if (top_stack_node.gu.terminal){
            if (out_of_range_tok_stream) throw std::runtime_error("Token stream out of range, but another terminal encountered");

            if (&(top_stack_node.gu) == token_stream[token_stream_index].gu){
                ++token_stream_index;
                continue;
            }
            else{
                throw std::runtime_error("Next terminal failed to compare");
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

            if (!rec.has_value()) throw std::runtime_error("Syntax error");
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
