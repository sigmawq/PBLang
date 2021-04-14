//
// Created by swqd on 3/31/21.
//

#ifndef LEXER_TEST_AE_PARSER_H
#define LEXER_TEST_AE_PARSER_H

#include <stack>
#include <optional>
#include <vector>
#include "ast_node.h"

// Used to convert arithmetic sequences to AST.
class AE_parser{
    std::vector<std::shared_ptr<ast_node>> arithmetic_sequence;

    struct rpn_resume_data{
        std::vector<std::shared_ptr<ast_node>> returned_rpn;
        size_t point_to_resume_from;

        std::string rpn_to_string(){
            std::string result;
            for (auto &thing : returned_rpn){
                result += thing->optional_value.value().value;
                result += " ";
            }
            return result;
        }
    };

    static
    rpn_resume_data form_reverse_polish_notation(std::vector<std::shared_ptr<ast_node>> &arithmetic_sequence,
                                                 size_t start_from = 0){

        std::vector<std::shared_ptr<ast_node>> rpn;
        std::stack<std::shared_ptr<ast_node>> operator_stack;

        auto pop_till_operator_of_same_precedence_found = [&](AST_NODE_TYPE current_operator){
            while (!operator_stack.empty() ||
                    operator_stack.top()->node_type >= current_operator){
                rpn.push_back(operator_stack.top());
                operator_stack.pop();
            }
        };

        auto pop_all_from_stack = [&](){
            while (!operator_stack.empty()){
                rpn.push_back(operator_stack.top());
                operator_stack.pop();
            }
        };

        auto merge_arithmetic_sequences = [&](std::vector<std::shared_ptr<ast_node>> &destination,
                                                     std::vector<std::shared_ptr<ast_node>> &source){
            for (auto i = 0; i < source.size(); i++){
                destination.push_back(source.at(i));
            }
        };

        for (size_t i = start_from; i < arithmetic_sequence.size();){
            auto &next_node = arithmetic_sequence[i];

            if (next_node->node_type == PAR_CLOSE){
                pop_all_from_stack();
                return {rpn, ++i};
            }

            else if (next_node->node_type == PAR_OPEN){
                rpn_resume_data resume_data = form_reverse_polish_notation(arithmetic_sequence, ++i);
                merge_arithmetic_sequences(rpn, resume_data.returned_rpn);
                i = resume_data.point_to_resume_from;
                continue;
            }

            else if (next_node->is_factor()){
                rpn.push_back(next_node);
            }
            else if (next_node->is_operator()){
                if (operator_stack.empty() || next_node->node_type > operator_stack.top().get()->node_type){
                    operator_stack.push(next_node);
                }
                else {
                    pop_till_operator_of_same_precedence_found(next_node->node_type);
                    operator_stack.push(next_node);
                }
            }
            else{
                std::string err = "AE_parser failed to parse next ast_node: next node is not an operator or factor or parenthesis, it is of type ";
                pbl_utility::str_compose(err, next_node->node_type);
                throw std::runtime_error(err);
            }
            ++i;
        }

        pop_all_from_stack();
        return {rpn, SIZE_MAX};
    }

    struct ast_resume_data{
        std::shared_ptr<ast_node> returned_node;
        size_t resume_from;
    };

    // @Start_from should point to the end of @rpn
    static ast_resume_data R_convert_rpn_to_ast(std::vector<std::shared_ptr<ast_node>> &rpn, size_t start_from){
        std::shared_ptr<ast_node> current_node = rpn[start_from];

        auto assert_current_position_validity = [&](){
            return start_from > 0;
        };

        enum CHILD_SIDE { LEFT, RIGHT };

        auto fullfill_right_child = [&](){
            ast_resume_data result = R_convert_rpn_to_ast(rpn, start_from);
            current_node->add_child(result.returned_node);
        };

        auto fullfill_left_child = [&](){
            ast_resume_data result = R_convert_rpn_to_ast(rpn, start_from);
            current_node.get()->add_child_front(result.returned_node);
        };

        auto fullfill_child = [&](CHILD_SIDE side){
            if (!assert_current_position_validity()) throw std::runtime_error("rpn to ast: no more nodes left, but node is not fullfilied");
            --start_from;
            if (side == LEFT){
                fullfill_left_child();
            }
            else if (side == RIGHT){
                fullfill_right_child();
            }
        };

        if (current_node->is_factor()){
            return { current_node, --start_from };
        }
        else if (current_node->is_operator()){
            fullfill_child(RIGHT);
            fullfill_child(LEFT);
        }
        else{
            throw std::runtime_error("Error while converting rpn to ast: current node is not an operator or factor");
        }
        return {current_node, start_from};
    }

    static std::shared_ptr<ast_node> convert_rpn_to_ast(std::vector<std::shared_ptr<ast_node>> &rpn){
        return R_convert_rpn_to_ast(rpn, rpn.size() - 1).returned_node;
    }

public:
    AE_parser(std::vector<std::shared_ptr<ast_node>> &ast_array){
        for (auto &thing : ast_array){
            arithmetic_sequence.push_back(thing);
        }
    }

    std::shared_ptr<ast_node> convert_to_ast(){
        rpn_resume_data rpn = form_reverse_polish_notation(arithmetic_sequence);
        pbl_utility::debug_print_nl("RPN: ", rpn.rpn_to_string(), "\n");
        return convert_rpn_to_ast(rpn.returned_rpn);
    }
};

#endif //LEXER_TEST_AE_PARSER_H
