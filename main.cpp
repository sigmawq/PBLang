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
#include "pbl_utility.h"
#include "parse_operations.h"

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

    // Table for comments
    rule d1 { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'/'} };
    rule d2 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ZERO_OR_MORE , {0, 0x0A - 1, 0x0A + 1, INT8_MAX - 1} };
    rule d3 { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {0x0A} };
    jump_table comments;
    comments.add_sequence_of_rules(std::vector{d1, d1, d2, d3}, TOKEN_TYPE::COMMENT_LINE);

    // Table for whitespaces
    rule ws { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ZERO_OR_MORE , {' '} };;
    jump_table whitespace;
    whitespace.add_sequence_of_rules(std::vector{ws}, TOKEN_TYPE::PURE_WS);

    std::string test_string {"01 //aa\n"
                             "for 111 Abracadabra 1.1"};

    std::vector all_tables {whitespace, comments, keywords, identifiers, floats, integers};
    //token_reader tr { test_string, all_tables};


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

/*void T_grammar_test(){
    std::vector<grammar_unit> universe;

    // Non terminals
    universe.push_back({false, "STMT"});
    universe.push_back({false, "EXPR"});
    universe.push_back({false, "TERM"});
    universe.push_back({false, "EXPR_P"});
    universe.push_back({false, "TERM_P"});
    universe.push_back({false, "FACTOR"});

    // Terminals
    universe.push_back({true, "+"});
    universe.push_back({true, "-"});
    universe.push_back({true, "*"});
    universe.push_back({true, "/"});

    universe.push_back({true, "<NUMBER>"});
    universe.back().mark_as_number();

    universe.push_back({true, "<IDENTIFIER>"});
    universe.back().mark_as_identifier();


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

            {"FACTOR",         {"<IDENTIFIER>"}},
            {"FACTOR",         {"<NUMBER>"}}
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
            {TOKEN_TYPE::OPERATOR, "+"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "1"},
            {TOKEN_TYPE::OPERATOR, "*"},
            {TOKEN_TYPE::CONSTANT_INTEGER, "3"},
    };

    auto tok_stream_gu = bind_token_to_universe(universe, tok_stream);

    auto tree = parse_string(tok_stream_gu, start_symbol, pt);
    tree.D_out();
}*/

int main() {
    std::string source = "1+1";

    auto tokenizer_data = prepare_tokenizer();

    parse_data pd;
    prepare_parse(pd);

    auto parse_tree = parse_source(tokenizer_data, pd, source);
    tree_out(parse_tree, parse_tree.get_root());

    //T_lexer_test();
    //T_grammar_test();
    return 0;
}

