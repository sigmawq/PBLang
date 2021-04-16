//
// Created by swql on 2/12/21.
//

#ifndef LEXER_TEST_PARSE_OPERATIONS_H
#define LEXER_TEST_PARSE_OPERATIONS_H

#include "grammar.h"
#include "parse_string.h"
#include "jump_table.h"
#include "token_reader.h"

struct tokenizer_data{
    jump_table keywords;
    jump_table identifiers;
    jump_table strings;
    jump_table integers;
    jump_table floats;
    jump_table comments;
    jump_table whitespace;

    std::vector<std::reference_wrapper<jump_table>> get_in_array_form(){
        return { whitespace, comments, floats, integers, keywords,  identifiers, strings};
    }
};

tokenizer_data prepare_tokenizer(){
    tokenizer_data tokenizer_data;

    tokenizer_data.keywords.add_keyword("for", KEYWORD);
    tokenizer_data.keywords.add_keyword("while", KEYWORD);
    tokenizer_data.keywords.add_keyword("if", KEYWORD);
    tokenizer_data.keywords.add_keyword("else", KEYWORD);
    tokenizer_data.keywords.add_keyword("var", KEYWORD);
    tokenizer_data.keywords.add_keyword("val", KEYWORD);
    tokenizer_data.keywords.add_keyword("struct", KEYWORD);
    tokenizer_data.keywords.add_keyword("string", KEYWORD);
    tokenizer_data.keywords.add_keyword("def", KEYWORD);
    tokenizer_data.keywords.add_keyword("return", KEYWORD);
    tokenizer_data.keywords.add_keyword("bool", KEYWORD);
    tokenizer_data.keywords.add_keyword("char", KEYWORD);
    tokenizer_data.keywords.add_keyword("int", KEYWORD);
    tokenizer_data.keywords.add_keyword("arr", KEYWORD);
    tokenizer_data.keywords.add_keyword("void", KEYWORD);
    tokenizer_data.keywords.add_keyword("float", KEYWORD);
    tokenizer_data.keywords.add_keyword("double", KEYWORD);
    tokenizer_data.keywords.add_keyword("overload", KEYWORD);
    tokenizer_data.keywords.add_keyword("unsigned", KEYWORD);
    tokenizer_data.keywords.add_keyword("|cast|", KEYWORD);
    tokenizer_data.keywords.add_keyword("=", OPERATOR);
    tokenizer_data.keywords.add_keyword("<", OPERATOR);
    tokenizer_data.keywords.add_keyword("<=", OPERATOR);
    tokenizer_data.keywords.add_keyword(">=", OPERATOR);
    tokenizer_data.keywords.add_keyword("==", OPERATOR);
    tokenizer_data.keywords.add_keyword("!=", OPERATOR);
    tokenizer_data.keywords.add_keyword("+", OPERATOR);
    tokenizer_data.keywords.add_keyword("-", OPERATOR);
    tokenizer_data.keywords.add_keyword("*", OPERATOR);
    tokenizer_data.keywords.add_keyword("/", OPERATOR);
    tokenizer_data.keywords.add_keyword("^", OPERATOR);
    tokenizer_data.keywords.add_keyword("**", OPERATOR);
    tokenizer_data.keywords.add_keyword("%", OPERATOR);
    tokenizer_data.keywords.add_keyword("||", OPERATOR);
    tokenizer_data.keywords.add_keyword("&&", OPERATOR);
    tokenizer_data.keywords.add_keyword("+=", OPERATOR);
    tokenizer_data.keywords.add_keyword("-=", OPERATOR);
    tokenizer_data.keywords.add_keyword("*=", OPERATOR);
    tokenizer_data.keywords.add_keyword("/=", OPERATOR);
    tokenizer_data.keywords.add_keyword("(", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword(")", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword(";", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword(":", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword(",", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword(".", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword("{", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword("}", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword("[", SPECIAL_CHARACTER);
    tokenizer_data.keywords.add_keyword("]", SPECIAL_CHARACTER);

    // Table for string literals
    rule z1 {  RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME, { {'\"'} }};
    rule z2 {  RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE, { {0, INT8_MAX - 1} }};
    tokenizer_data.strings.add_sequence_of_rules(std::vector {z1, z2, z1}, TOKEN_TYPE::STRING);

    // Table for identifiers
    rule a1 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ONE_TIME, { 'a', 'z', 'A', 'Z', '_', '_'} };
    rule a2 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ZERO_OR_MORE,
              { 'a', 'z', 'A', 'Z', '1', '9', '_', '_' } };
    tokenizer_data.identifiers.add_sequence_of_rules(std::vector{a1, a2}, TOKEN_TYPE::IDENTIFIER);

    // Table for integers
    rule b1 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ONE_TIME, {'1', '9'} };
    rule b2 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };
    rule zero_rule { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'0'} };

    tokenizer_data.integers.add_sequence_of_rules(std::vector{b1, b2}, TOKEN_TYPE::CONSTANT_INTEGER);
    tokenizer_data.integers.add_sequence_of_rules(std::vector{zero_rule}, TOKEN_TYPE::CONSTANT_INTEGER);

    // Table for floats
    rule c2 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };
    rule c3 { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'.'} };
    rule c4 { RULE_TYPE::ANY_OF_SEQUENCE, RULE_QUALIFIER::ZERO_OR_MORE , {'0', '9'} };

    tokenizer_data.floats.add_sequence_of_rules(std::vector{c2, c3, c4}, TOKEN_TYPE::CONSTANT_FP);

    // Table for comments
    rule d1 { RULE_TYPE::ONE_CHAR, RULE_QUALIFIER::ONE_TIME , {'/'} };
    rule d2 { RULE_TYPE::ANY_OF_SEQUENCE_AND, RULE_QUALIFIER::ZERO_OR_MORE , {0, 0x0A - 1, 0x0A + 1, INT8_MAX - 1} };

    // new line & eos chars
    rule d3 { RULE_TYPE::ANY_OF, RULE_QUALIFIER::ONE_TIME , {0x0A} };

    tokenizer_data.comments.add_sequence_of_rules(std::vector{d1, d1, d2, d3}, TOKEN_TYPE::COMMENT_LINE);

    // Table for whitespaces
    rule ws { RULE_TYPE::ANY_OF, RULE_QUALIFIER::ZERO_OR_MORE , {' ', '\n'} };;

    tokenizer_data.whitespace.add_sequence_of_rules(std::vector{ws}, TOKEN_TYPE::PURE_WS);

    return tokenizer_data;
}

// Produces token stream from source string.
// Comments and whitespaces are ignored
std::optional<std::vector<token>> tokenize(tokenizer_data &tokenizer_data, std::string const& source){
    token_reader token_reader { source, tokenizer_data.get_in_array_form() };
    std::vector<token> token_stream;

    pbl_utility::debug_print_nl("Tokens parsed: ");
    size_t token_index = 0;
    while (!token_reader.string_eos_reached()){
        auto pos = token_reader.get_position();
        auto result = token_reader.read_next_token();
        if (!result.has_value()){
            std::cout << "Unrecognized token at " << pos.first << ", " << pos.second << std::endl;
            highlight_error_point(source, pos.first, pos.second);
            return {};
        }
        else{
            result.value().line = pos.first;
            result.value().col = pos.second;
            if (result.value().type == TOKEN_TYPE::PURE_WS || result.value().type == TOKEN_TYPE::COMMENT_LINE) continue;
            token_stream.push_back(result.value());
            pbl_utility::debug_print_nl(std::to_string(token_index), '.', " ", result.value().attribute, " (", std::to_string(result.value().type), ')');
            ++token_index;
        }
    }
    return {token_stream};
}

std::vector<bound_token> bind_token_to_universe(std::vector<grammar_unit> &universe, std::vector<token> &tok_stream){
    std::vector<bound_token> bound_token_stream;

    std::vector<grammar_unit>::iterator result;
    for (const auto &el : tok_stream) {
        if (el.type == TOKEN_TYPE::IDENTIFIER){
            result = std::find_if(universe.begin(), universe.end(), [&](const grammar_unit &gu) {
                return (gu.is_identifier);
            });
        }
        else if (el.type == TOKEN_TYPE::STRING){
            result = std::find_if(universe.begin(), universe.end(), [&](const grammar_unit &gu) {
                return (gu.is_string);
            });
        }
        else if (el.type == TOKEN_TYPE::CONSTANT_FP || el.type == TOKEN_TYPE::CONSTANT_INTEGER){
            result = std::find_if(universe.begin(), universe.end(), [&](const grammar_unit &gu) {
                return (gu.is_number);
            });
        }
        else{
            result = std::find_if(universe.begin(), universe.end(), [&](const grammar_unit &gu) {
                return (gu.string_representation == el.attribute);
            });
        }

        if (result == universe.end()) {
            std::string err;
            pbl_utility::str_compose(err, "Token ", el.attribute, " of type ", el.type, " has not been "
                                                                                        "found in "
                                                                                        "universe set");
            throw std::runtime_error(err);
        }
        else{
            bound_token_stream.push_back({el, &*result});
        }
    }

    return bound_token_stream;
}

struct parse_data{
    std::vector<grammar_unit> universe;
    std::optional<parse_table> pt;
    const grammar_unit *start_symbol;
    std::unordered_map<const grammar_unit *, production_group> productions;
    std::unordered_map<const grammar_unit *, first_set> first_set_val;
    std::unordered_map<const grammar_unit *, follow_set> follow_set_val;
    std::vector<predict_set_record> predict_set_val;
};

void prepare_parse(parse_data &pd){
    std::vector<grammar_unit> &universe = pd.universe;

    // ** NON-TERMINALS **
        universe.push_back({false, "STMT"});
        universe.push_back({false, "A_Es"});
        universe.push_back({false, "A_E"});
        universe.push_back({false, "A_E_scope"});
        universe.push_back({false, "F"});
        universe.push_back({false, "BLOCK_SEGMENT"});
        universe.push_back({false, "ASSIGNMENT"});
        universe.push_back({false, "OPT_ASSIGNMENT"});
        universe.push_back({false, "VAR_DECL"});
        universe.push_back({false, "OPT_NEXT_VAR_DECL"});
        universe.push_back({false, "CONST_DECL"});
        universe.push_back({false, "ARG_DECL"});
        universe.push_back({false, "TYPE_SPEC"});
        universe.push_back({false, "F_DECL"});
        universe.push_back({false, "F_CALL"});
        universe.push_back({false, "OPT_ARG_START"});
        universe.push_back({false, "OPT_ARG_TAIL"});
        universe.push_back({false, "F_DECL_OVERLOAD"});
        universe.push_back({false, "ARG_CALL_s"});
        universe.push_back({false, "ARG_CALL"});
        universe.push_back({false, "OPT_F_CALL"});
        universe.push_back({false, "ARR_ACCESS"});
        universe.push_back({false, "OPT_ARRAY_DECL"});
        universe.push_back({false, "STRUCT_DECL"});
        universe.push_back({false, "OPT_EXPLICIT_BYTE_ALLOC"});
        universe.push_back({false, "ARR_DECL"});
        universe.push_back({false, "OPT_ARG_DECL"});
        universe.push_back({false, "OPT_TYPE_SPEC" });
        universe.push_back({false, "STRUCT_VAR_DECL"});
        universe.push_back({false, "OPT_STRUCT_VAR_DECL"});
        universe.push_back({false, "RETURN_STMT"});
        universe.push_back({false, "OPT_F_TRAIL"});
        universe.push_back({false, "F_TRAIL"});
        universe.push_back({false, "OPT_ARR"});
        universe.push_back({false, "OPT_UNARY_OP"});
        universe.push_back({false, "OPT_COMPLEX_ASSIGNMENT"});
        universe.push_back({false, "COMPLEX_ASSIGNMENT"});
        universe.push_back({false, "F_INPUT"});
        universe.push_back({false, "F_OUTPUT"});
        universe.push_back({false, "VOID_TYPE"});
        universe.push_back({false, "OPT_TYPE_MOD"});
        universe.push_back({false, "VAR_SIGN"});
        universe.push_back({false, "ARR_SIGN"});
        universe.push_back({false, "CONST_SIGN"});
        universe.push_back({false, "NEXT_OPT_VAR_SIGN"});
        universe.push_back({false, "OPT_STRUCT_ACCESS"});
        universe.push_back({false, "STRUCT_ACCESS"});
        universe.push_back({false, "OPT_DIM_DECL"});
        universe.push_back({false, "DIM_DECL"});
        universe.push_back({false, "NEXT_DIM_DECL"});
        universe.push_back({false, "IF_STMT"});
        universe.push_back({false, "WHILE_STMT"});
        universe.push_back({false, "FOR_STMT"});
        universe.push_back({false, "OPT_ELSE"});
        universe.push_back({false, "OPT_IF_STMT"});

    // ** TERMINALS **
        // Arithmetic
        universe.push_back({true, "+"});
        universe.push_back({true, "-"});
        universe.push_back({true, "*"});
        universe.push_back({true, "/"});
        universe.push_back({true, "("});
        universe.push_back({true, ")"});
        universe.push_back({true, "%"});
        universe.push_back({true, "++"});
        universe.push_back({true, "--"});

        // Relational
        universe.push_back({true, "!="});
        universe.push_back({true, "=="});
        universe.push_back({true, ">"});
        universe.push_back({true, "<"});
        universe.push_back({true, ">="});
        universe.push_back({true, "<="});

        // Assignment
        universe.push_back({true, "="});
        universe.push_back({true, "+="});
        universe.push_back({true, "-="});
        universe.push_back({true, "*="});
        universe.push_back({true, "/="});

        // Logical
        universe.push_back({true, "||"});
        universe.push_back({true, "&&"});
        universe.push_back({true, "!"});

        // Keywords
        universe.push_back({true, "|cast|"});
        universe.push_back({true, "if"});
        universe.push_back({true, "for"});
        universe.push_back({true, "while"});
        universe.push_back({true, "struct"});
        universe.push_back({true, "def"});
        universe.push_back({true, "arr"});
        universe.push_back({true, "void"});
        universe.push_back({true, "int"});
        universe.push_back({true, "float"});
        universe.push_back({true, "double"});
        universe.push_back({true, "bool"});
        universe.push_back({true, "char"});
        universe.push_back({true, "string"});
        universe.push_back({true, "import"});
        universe.push_back({true, "struct"});
        universe.push_back({true, "unsigned"});
        universe.push_back({true, "var"});
        universe.push_back({true, "val"});
        universe.push_back({true, "overload"});
        universe.push_back({true, "return"});
        universe.push_back({true, "if"});
        universe.push_back({true, "else"});


    // Other
        universe.push_back({true, "{"});
        universe.push_back({true, "}"});
        universe.push_back({true, "["});
        universe.push_back({true, "]"});
        universe.push_back({true, ";"});
        universe.push_back({true, ":"});
        universe.push_back({true, ","});
        universe.push_back({true, "."});

        // Dynamic terminals
        universe.push_back({true, "<NUMBER>"});
        universe.back().mark_as_number();

        universe.push_back({true, "<IDENTIFIER>"});
        universe.back().mark_as_identifier();

        universe.push_back({true, "<STRING>"});
        universe.back().mark_as_string();


    std::vector<std::pair<std::string, std::vector<std::string>>> productions_raw{

            // General
            {"STMT",         {" "}},
            {"STMT",         {"A_Es", ";", "STMT"}},
            {"STMT",         {";", "STMT"}},
            {"STMT",         {"VAR_DECL", ";", "STMT"}},
            {"STMT",         {"CONST_DECL", ";", "STMT"}},
            {"STMT",         {"ARR_DECL", ";", "STMT"}},
            {"STMT",         {"F_DECL", "STMT"}},
            {"STMT",         {"F_DECL_OVERLOAD", "STMT"}},
            {"STMT",         {"STRUCT_DECL", "STMT"}},
            {"STMT",         {"RETURN_STMT", ";", "STMT"}},
            {"STMT",         {"IF_STMT"}},
            {"STMT",         {"WHILE_STMT", "STMT"}},
            {"STMT",         {"FOR_STMT", "STMT"}},

            // Flow control
            {"FOR_STMT",         {"for" ,"(", "VAR_DECL", ";", "A_Es", ";", "A_Es", ")", "BLOCK_SEGMENT"}},
            {"WHILE_STMT",         {"while", "(", "A_Es", ")", "BLOCK_SEGMENT"}},
            {"IF_STMT",         {"if", "(", "A_Es", ")", "BLOCK_SEGMENT", "OPT_ELSE"}},

            {"OPT_IF_STMT",         {"if", "(", "A_Es", ")", "BLOCK_SEGMENT", "OPT_ELSE"}},
            {"OPT_IF_STMT",         {" "}},

            {"OPT_ELSE",         {"else", "OPT_IF_STMT"}},
            {"OPT_ELSE",         {" "}},

            // Return statement
            {"RETURN_STMT",         {"return", "A_Es"}},

            // Block segment is defined by opening '{' and closing '}'
            {"STMT",         {"BLOCK_SEGMENT"}},
            {"BLOCK_SEGMENT", {"{", "STMT", "}"}},

            // Arithmetic expr group
            {"A_Es",         {"F", "A_E"}},
            {"A_E",         {"+", "F", "A_E"}},
            {"A_E",         {"-", "F", "A_E"}},
            {"A_E",         {"*", "F", "A_E"}},
            {"A_E",         {"/", "F", "A_E"}},
            {"A_E",         {"||", "F", "A_E"}},
            {"A_E",         {"&&", "F", "A_E"}},
            {"A_E",         {"==", "F", "A_E"}},
            {"A_E",         {">=", "F", "A_E"}},
            {"A_E",         {"<=", "F", "A_E"}},
            {"A_E",         {">", "F", "A_E"}},
            {"A_E",         {"<", "F", "A_E"}},
            {"A_E",         {" "}},

            // Constant declaration
            {"CONST_DECL", {"val", "VAR_SIGN", "<IDENTIFIER>", "COMPLEX_ASSIGNMENT"}},

            // Variable declaration
            {"VAR_DECL", {"var", "VAR_SIGN", "<IDENTIFIER>", "OPT_COMPLEX_ASSIGNMENT", "OPT_NEXT_VAR_DECL"}},
            {"OPT_NEXT_VAR_DECL", {",", "<IDENTIFIER>", "OPT_COMPLEX_ASSIGNMENT", "OPT_NEXT_VAR_DECL"}},
            {"OPT_NEXT_VAR_DECL", {" "}},

            // Array declaration
            {"ARR_DECL", {"ARR_SIGN", "<IDENTIFIER>", "OPT_ASSIGNMENT"}},

            // Struct declaration
            {"STRUCT_DECL", {"struct", "<IDENTIFIER>", "OPT_EXPLICIT_BYTE_ALLOC", "{", "OPT_STRUCT_VAR_DECL","}" }},
                // Optional explicit byte allocation
                {"OPT_EXPLICIT_BYTE_ALLOC", {"(", "<NUMBER>", ")" }},
                {"OPT_EXPLICIT_BYTE_ALLOC", {" " }},

                // Struct variable declaration
                {"STRUCT_VAR_DECL", {"VAR_SIGN", "<IDENTIFIER>", "OPT_COMPLEX_ASSIGNMENT", ";"}},
                {"OPT_STRUCT_VAR_DECL", {"STRUCT_VAR_DECL", "OPT_STRUCT_VAR_DECL"}},
                {"OPT_STRUCT_VAR_DECL", {" "}},


            // Grammar for array ACCESS
            {"ARR_ACCESS", {"[", "A_Es", "]"}},

            // Type Specifier
            {"TYPE_SPEC",         {"int"}},
            {"TYPE_SPEC",         {"double"}},
            {"TYPE_SPEC",         {"float"}},
            {"TYPE_SPEC",         {"bool"}},
            {"TYPE_SPEC",         {"char"}},
            {"TYPE_SPEC",         {"string"}},
            {"TYPE_SPEC",         {"<IDENTIFIER>"}},
            {"OPT_TYPE_SPEC",         {",", "TYPE_SPEC", "OPT_TYPE_SPEC"}},
            {"OPT_TYPE_SPEC",         {" "}},

            {"VOID_TYPE", {"void"}},

            // Type Modifier
            {"OPT_TYPE_MOD", {"unsigned"}},
            {"OPT_TYPE_MOD", {" "}},

            // Function declaration
            {"F_DECL",         {"def", "<IDENTIFIER>", "(", "F_INPUT", ":", "F_OUTPUT", ")", "BLOCK_SEGMENT"}},
            {"F_DECL_OVERLOAD",         {"overload", "F_DECL"}},

            {"F_INPUT",         {"ARG_DECL", "OPT_ARG_DECL"}},
            {"F_INPUT",         {"VOID_TYPE"}},

            {"F_OUTPUT",         {"VAR_SIGN"}},
            {"F_OUTPUT",         {"ARR_SIGN"}},
            {"F_OUTPUT",         {"VOID_TYPE"}},

            {"ARG_DECL", {"VAR_SIGN", "<IDENTIFIER>"}},
            {"ARG_DECL", {"ARR_SIGN", "<IDENTIFIER>"}},

            {"OPT_ARG_DECL", {",", "ARG_DECL", "OPT_ARG_DECL"}},
            {"OPT_ARG_DECL", {" "}},

            // Arguments for function call
            {"ARG_CALL_s",         {" "}},
            {"ARG_CALL_s",         {"A_Es", "ARG_CALL"}},
            {"ARG_CALL",         {",", "A_Es", "ARG_CALL"}},
            {"ARG_CALL",         {" "}},

            // Assignment
                // Mandatory
                {"ASSIGNMENT", {"="}},
                {"ASSIGNMENT", {"+="}},
                {"ASSIGNMENT", {"-="}},
                {"ASSIGNMENT", {"*="}},
                {"ASSIGNMENT", {"/="}},

                // Optional
                {"OPT_ASSIGNMENT", {"ASSIGNMENT"}},
                {"OPT_ASSIGNMENT", {" "}},

                // Complex + Optional
                {"OPT_COMPLEX_ASSIGNMENT", {"COMPLEX_ASSIGNMENT"}},
                {"OPT_COMPLEX_ASSIGNMENT", {" "}},
                {"COMPLEX_ASSIGNMENT", {"=", "A_Es"}},

                // Different signatures
                // Variable signature
            {"VAR_SIGN", {"OPT_TYPE_MOD", "TYPE_SPEC"}},
            {"NEXT_OPT_VAR_SIGN", {",", "VAR_SIGN", "NEXT_OPT_VAR_SIGN"}},
            {"NEXT_OPT_VAR_SIGN", {" "}},

                // Array signature
            {"ARR_SIGN", {"arr", "[", "OPT_DIM_DECL", "]", "VAR_SIGN"}},
            {"OPT_DIM_DECL", {"DIM_DECL"}},
            {"OPT_DIM_DECL", {" "}},

            {"DIM_DECL", {"F", "NEXT_DIM_DECL"}},
            {"NEXT_DIM_DECL", {",", "F", "NEXT_DIM_DECL"}},
            {"NEXT_DIM_DECL", {" "}},

            {"F",         {"OPT_UNARY_OP", "<IDENTIFIER>", "OPT_F_TRAIL"}},
            {"F",         {"<NUMBER>"}},
            {"F",         {"<STRING>"}},
            {"F",         {"(", "A_Es", ")"}},

            {"OPT_F_TRAIL",         {"F_TRAIL", "OPT_F_TRAIL"}},
            {"OPT_F_TRAIL",         {" "}},

            {"F_TRAIL",         {"F_CALL"}},
            {"F_TRAIL",         {"OPT_ARR"}},
            {"F_TRAIL",         {"STRUCT_ACCESS"}},

            {"OPT_STRUCT_ACCESS",         {"STRUCT_ACCESS", "OPT_STRUCT_ACCESS"}},
            {"OPT_STRUCT_ACCESS",         {" "}},
            {"STRUCT_ACCESS",         {".", "<IDENTIFIER>"}},

            {"F_CALL",         {"(" ,"ARG_CALL_s", ")"}},

            {"OPT_ARR",         {"[" ,"A_Es", "]"}},

            {"OPT_UNARY_OP",         {"-"}},
            {"OPT_UNARY_OP",         {"+"}},
            {"OPT_UNARY_OP",         {"|cast|"}},
            {"OPT_UNARY_OP",         {"!"}},
            {"OPT_UNARY_OP",         {" "}},
    };

    pd.productions = grammar::parse_productions(universe, productions_raw);

    // Just alias, as for universe above
    auto &productions = pd.productions;

    for (auto &thing : productions){
        thing.second.D_out(thing.first->string_representation);
    }

    using namespace pbl_utility;

    const grammar_unit *start_symbol = grammar::find_gu_by_name(universe, "STMT");

    pd.first_set_val = grammar::get_first_set(universe, productions, start_symbol);
    grammar::D_first_set_out(pd.first_set_val);
    std::cout << std::endl;

    debug_print_nl("Follow set: ");
    pd.follow_set_val = grammar::get_follow_set(universe, productions, start_symbol, pd.first_set_val);
    grammar::D_follow_set_out(pd.follow_set_val);

    debug_print_nl("Predict set: ");
    pd.predict_set_val = grammar::get_predict_set(universe, productions, start_symbol, pd.first_set_val);
    grammar::D_predict_out(pd.predict_set_val);

    pd.pt.emplace(pd.universe, pd.predict_set_val, pd.follow_set_val);
    pd.start_symbol = start_symbol;
}

// Builds parse tree from source string
parse_tree parse_source(tokenizer_data &td, parse_data &pd, std::string &source){
    auto tokens = tokenize(td, source);

    // Error checking
    if (!tokens.has_value()) {
        throw std::runtime_error("Parser stops due to tokenizer failure"); }

    auto bound_token_stream = bind_token_to_universe(pd.universe, tokens.value());
    auto tree = parse_string(bound_token_stream, pd.start_symbol, pd.pt.value(), source);

    return tree;
}

#endif //LEXER_TEST_PARSE_OPERATIONS_H
