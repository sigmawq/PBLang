#include <iostream>
#include <vector>
#include <unordered_map>

#include "token.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg)
#endif

#include "jump_table.h"
#include "token_reader.h"
#include "grammar.h"
#include "parse_table.h"
#include "parse_string.h"
#include "parse_tree.h"

void T_lexer_test(){
    jump_table keywords;
    keywords.add_keyword("for", KEYWORD);
    keywords.add_keyword("while", KEYWORD);
    keywords.add_keyword("if", KEYWORD);
    keywords.add_keyword("else", KEYWORD);
    keywords.add_keyword("lambda", KEYWORD);
    keywords.add_keyword("=", OPERATOR);
    keywords.add_keyword("<", OPERATOR);
    keywords.add_keyword("<=", OPERATOR);
    keywords.add_keyword(">=", OPERATOR);
    keywords.add_keyword("==", OPERATOR);
    keywords.add_keyword("!=", OPERATOR);
    keywords.add_keyword("(", SPECIAL_CHARACTER);
    keywords.add_keyword(")", SPECIAL_CHARACTER);
    keywords.add_keyword("foreach", KEYWORD);

    // Table for identifiers
    rule a1 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ONE_TIME, { 'a', 'z', 'A', 'Z' } };
    rule a2 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ZERO_OR_MORE,
              { 'a', 'z', 'A', 'Z', '1', '9' } };
    jump_table identifiers; // TODO
    identifiers.add_sequence_of_rules(std::vector{a1, a2}, TOKEN_TYPE::IDENTIFIER);

    // Table for integers
    rule b1 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ONE_TIME, {'1', '9'} };
    rule b2 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };
    rule zero_rule { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'0'} };

    jump_table integers;
    integers.add_sequence_of_rules(std::vector{b1, b2}, TOKEN_TYPE::CONSTANT_INTEGER);
    integers.add_sequence_of_rules(std::vector{zero_rule}, TOKEN_TYPE::CONSTANT_INTEGER);

    // Table for floats
    rule c2 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };
    rule c3 { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'.'} };
    rule c4 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };

    jump_table floats;
    floats.add_sequence_of_rules(std::vector{c2, c3, c4}, TOKEN_TYPE::CONSTANT_FP);

    std::string test_string {"0 #This is a comment\n"
                             "for 111 Abracadabra 1.1"};

    std::vector all_tables {keywords, identifiers, floats, integers};
    token_reader tr { test_string, all_tables};

    while (!tr.string_eos_reached()){
        auto result = tr.read_next_token();
        if (!result.has_value()){
            auto pos = tr.get_position();
            std::cout << "Unrecognized token at " << pos.first << ", " << pos.second << std::endl;
            break;
        }
        else{
            if (result.value().type != TOKEN_TYPE::PURE_WS && result.value().type != TOKEN_TYPE::COMMENT_LINE){
                std::cout << result.value().type << ' ' << result.value().attribute << std::endl;
            }
        }
    }
}

void tabs_out(int tabs){
    for (int i = 0; i < tabs; i++){
        std::cout << ' ';
    }
}

void tree_out(parse_tree &tree, parse_node &root, int tabs = 0){
    tabs_out(tabs);
    std::cout << root.gu.string_representation << std::endl;
    tabs += 2;
    for (int i = 0; i < root.children.size(); i++){
        tree_out(tree, tree.get_node(root.children[i]), tabs);
    }
}

std::vector<const grammar_unit*> tok_stream_to_gu(std::vector<grammar_unit> &universe, std::vector<token> &tok_stream){
    std::vector<const grammar_unit*> gu_tok_stream;

    for (const auto &el : tok_stream) {
        auto result = std::find_if(universe.begin(), universe.end(), [&](const grammar_unit &gu) {
            return (gu.string_representation == el.attribute);
        });
        if (result == universe.end()) throw std::runtime_error("One token has not been found in universe set");
        gu_tok_stream.push_back(&*result.base());
    }

    return gu_tok_stream;
}

void T_grammar_test(){
    std::vector<grammar_unit> universe;
    universe.push_back({false, "STMT"});
    universe.push_back({false, "DIGIT"});
    universe.push_back({true, "1"});
    universe.push_back({true, "2"});
    universe.push_back({true, "3"});
    universe.push_back({true, "4"});
    universe.push_back({true, "5"});
    universe.push_back({true, "6"});
    universe.push_back({true, "7"});
    universe.push_back({true, "8"});
    universe.push_back({true, "9"});
    universe.push_back({true, "+"});
    universe.push_back({true, "-"});
    universe.push_back({true, "*"});
    universe.push_back({true, "/"});
    universe.push_back({false, "EXPR"});
    universe.push_back({false, "TERM"});
    universe.push_back({false, "EXPR_P"});
    universe.push_back({false, "TERM_P"});
    universe.push_back({false, "FACTOR"});


    std::vector<std::pair<std::string, std::vector<std::string>>> productions_raw{
            {"STMT",         {" "}},
            {"STMT",         {"EXPR"}},

            // Arithmetic expr group
            {"EXPR",         {"TERM", "EXPR_P"}},
            {"EXPR_P",         {"+", "TERM", "EXPR_P"}},
            {"EXPR_P",         {" "}},
            {"TERM",          {"FACTOR", "TERM_P"}},
            {"TERM_P",         {"*", "FACTOR", "TERM_P"}},
            {"TERM_P",         {" "}},
            {"FACTOR",         {"DIGIT"}},

            {"DIGIT",        {"1"}},
            {"DIGIT",        {"2"}},
            {"DIGIT",        {"3"}},
            {"DIGIT",        {"4"}},
            {"DIGIT",        {"5"}},
            {"DIGIT",        {"6"}},
            {"DIGIT",        {"7"}},
            {"DIGIT",        {"8"}},
            {"DIGIT",        {"9"}},
    };

    auto productions = grammar::parse_productions(universe, productions_raw);
    for (auto &thing : productions){
        thing.second.D_out(thing.first->string_representation);
    }

    const grammar_unit *start_symbol = grammar::find_gu_by_name(universe, "STMT");
    std::cout << "FIRST set: " << std::endl;
    auto first_set = grammar::get_first_set(universe, productions, start_symbol);
    grammar::D_first_set_out(first_set);
    std::cout << std::endl;

    std::cout << "FOLLOW set: " << std::endl;
    auto follow_set = grammar::get_follow_set(universe, productions, start_symbol, first_set);
    grammar::D_follow_set_out(follow_set);

    std::cout << "PREDICT set: " << std::endl;
    auto predict_set = grammar::get_predict_set(universe, productions, start_symbol, first_set);
    grammar::D_predict_out(predict_set);

    std::vector<const grammar_unit*> non_terminals;
    std::vector<const grammar_unit*> terminals;
    for (const auto &gu : universe){
        if (gu.terminal) terminals.push_back(&gu);
        else non_terminals.push_back(&gu);
    }

    parse_table pt { terminals, non_terminals, predict_set, follow_set};

    std::cout << "Parse table: " << std::endl;
    pt.D_out();

    std::vector<token> tok_stream{
            {TOKEN_TYPE::CONSTANT_INTEGER, "2"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "+"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "1"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "*"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "3"},
    };

    auto tok_stream_gu = tok_stream_to_gu(universe, tok_stream);

    auto tree = parse_string(tok_stream_gu, start_symbol, pt);
    tree.D_out();
}

int main() {
    T_lexer_test();
    //T_grammar_test();
    return 0;
}

