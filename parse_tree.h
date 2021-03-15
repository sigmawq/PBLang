//
// Created by swql on 3/5/21.
//

#ifndef LEXER_TEST_PARSE_TREE_H
#define LEXER_TEST_PARSE_TREE_H

#include "grammar.h"

struct bound_token{
    const token &token_value;
    const grammar_unit *gu;
};

struct parse_node {
    const grammar_unit &gu;

    // Optional value to hold data (numbers, identifiers)
    std::string opt_val;
    std::vector<size_t> children;

    void add_child_node(size_t another_node){
        children.push_back(another_node);
    }

    void set_opt_val(std::string const& opt_val){
        this->opt_val = opt_val;
    }
};

class parse_tree{
    std::vector<parse_node> nodes;

    void _D_out(parse_node &croot, int tabs){
        auto tabs_out = [](int tabs_count){
            for (int i = 0; i < tabs_count; i++){
                std::cout << ' ';
            }
        };

        tabs_out(tabs);
        std::cout << croot.gu.string_representation << std::endl;

        tabs += 2;
        for (int i = croot.children.size() - 1; i >= 0; i--){
            _D_out(this->get_node(croot.children[i]), tabs);
        }

        if (!croot.gu.terminal && croot.children.size() == 0){
            tabs_out(tabs);
            std::cout << "epsilon" << std::endl;
        }
    }

public:
    explicit parse_tree(const grammar_unit &root_gu) {
        nodes.push_back({root_gu});
    }

    [[nodiscard]] parse_node& get_root() { return nodes.at(0); }

    void D_out() {
        _D_out(get_root(), 0);
    }

    // Returns reference to newly appended parse node
    size_t add_grammar_unit_to_parse_node(const size_t source_index, const grammar_unit &new_child, std::string opt_val = ""){
        nodes.push_back({new_child, opt_val});
        nodes[source_index].add_child_node(nodes.size() - 1);
        return nodes.size() - 1;
    }

    parse_node& get_node(size_t node_id) { return nodes[node_id]; }
};

#endif //LEXER_TEST_PARSE_TREE_H
