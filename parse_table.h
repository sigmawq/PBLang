//
// Created by swql on 3/2/21.
//

#ifndef LEXER_TEST_PARSE_TABLE_H
#define LEXER_TEST_PARSE_TABLE_H
#include "grammar.h"

class parse_table{
    std::unordered_map<const grammar_unit*,
        std::unordered_map<const grammar_unit*, std::optional<predict_set_record>>> table;
        std::unordered_map<const grammar_unit*, std::optional<predict_set_record>> dollar_column;

public:
    // Return no value if cell has no valid production
    std::optional<predict_set_record>& at(const grammar_unit *row, const grammar_unit *column){
        try{
            return table.at(row).at(column);
        }
        catch (std::out_of_range &e) {
            throw std::runtime_error("Requested non-existing record from parse table");
        }
    }

    std::optional<predict_set_record>& at_dollar(const grammar_unit *gu){
        try{
            auto result = dollar_column.at(gu);
            return dollar_column.at(gu);
        }
        catch (std::out_of_range &e) {
            throw std::runtime_error("Requested non-existing record from parse table");
        }
    }

    parse_table(std::vector<grammar_unit> &universe,
                std::vector<predict_set_record> &predict_set,
                std::unordered_map<const grammar_unit*, follow_set> &follow_set_val){


        std::vector<const grammar_unit*> terminals;
        std::vector<const grammar_unit*> non_terminals;
        for (const auto &gu : universe){
            if (gu.terminal) terminals.push_back(&gu);
            else non_terminals.push_back(&gu);
        }

        for (int i = 0; i < non_terminals.size(); i++){
            auto inserted = table.insert({non_terminals[i], {}}).first;
            for (const auto &term : terminals){
                inserted->second.insert({term, {}});
            }
            dollar_column.insert({non_terminals[i], {}});
        }

        for (const predict_set_record &psr : predict_set){

            // Rule 1
            for (const grammar_unit *gu : psr.fs.content){
                this->at(psr.lhs, gu) = psr;
            }

            // Rule 2
            auto follow_set_used = follow_set_val.at(psr.lhs);
            if (psr.fs.has_epsilon || psr.is_epsilon_prod()){
                // Handle pure epsilon production\

                for (const grammar_unit *gu : follow_set_used.content){
                    this->at(psr.lhs, gu) = psr;
                }
                if (follow_set_used.has_dollar){
                    this->dollar_column.at(psr.lhs) = psr;
                }
            }

        }
    }

    void D_out(){
        for (const auto &row : table){
            std::cout << row.first->string_representation << std::endl;
            for (const auto &column_value : row.second){
                std::cout << "   ";
                std::cout << column_value.first->string_representation << ": ";
                if (column_value.second.has_value()){
                    auto psr = column_value.second.value();
                    psr.prod_out();
                }
                else {
                    std::cout << "ERROR_INPUT" << std::endl;
                }
            }
        }
    }
};

#endif //LEXER_TEST_PARSE_TABLE_H
