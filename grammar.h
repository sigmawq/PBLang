//
// Created by swql on 2/12/21.
//

#ifndef LEXER_TEST_GRAMMAR_H
#define LEXER_TEST_GRAMMAR_H

#include <vector>
#include <set>
#include <memory>
#include <variant>

// Non terminal or terminal
struct grammar_unit{
    const bool terminal;
    const std::string string_representation;

    bool is_identifier;
    bool is_number;
    bool is_string;

    [[nodiscard]] bool is_term() const { return terminal; }
    void out() const { std::cout << string_representation; }

    inline void mark_as_identifier() { is_identifier = true; }
    inline void mark_as_number() { is_number = true; }
    inline void mark_as_string() { is_string = true; }
};

/*struct parse_node{
    grammar_unit const& value;
    std::vector<std::shared_ptr<parse_node>> children;

    explicit parse_node(const grammar_unit &gu) : value(gu) {}

    // Append a grammar unit by creating a new node with it
    std::shared_ptr<parse_node> add_child(const grammar_unit &gu){
        children.emplace_back(std::make_shared<parse_node>(gu));
        return children.back();
    }

    // Append existing node, possibly containing multiple children
    void append_node(std::shared_ptr<parse_node> &node){
        children.push_back(node);
    }
}; */

struct production {
    std::vector<const grammar_unit*> content;
};

struct first_set {
    bool has_epsilon;
    std::set<const grammar_unit*> content;
};

struct follow_set{
    bool has_dollar;
    std::set<const grammar_unit*> content;
};

struct production_group{
private:
    std::vector<production> rhs;
    bool has_epsilon;

public:
    void D_out(const std::string &lhs) const {
#ifdef DEBUG_MODE
        std::cout << lhs << " -> ";
        for (const production &p : rhs){
            for (const grammar_unit *gu : p.content){
                std::cout << " " << gu->string_representation;
            }
            std::cout << " | ";
        }
        if (has_epsilon) { std::cout << "ε"; }
        std::cout << std::endl;
    }
#endif
public:
    [[nodiscard]] std::vector<production> const& get_all_productions() const { return rhs; }
    void add_production(const production &prod){
        rhs.push_back(prod);
    }
    void activate_epsilon_production() { has_epsilon = true; }
    [[nodiscard]] bool has_epsilon_production() const { return has_epsilon; }
};

struct predict_set_record{
    const grammar_unit *lhs;
    std::optional<std::reference_wrapper<const production>> prod;
    first_set fs;

    void add_to_first(std::set<const grammar_unit*> &to_add){
        fs.content.merge(to_add);
    }

    void add_to_first(const grammar_unit *gu){
        fs.content.insert(gu);
    }

    bool is_epsilon_prod() const{
        return !prod.has_value();
    }

    void prod_out(){
        std::cout << lhs->string_representation << " -> ";
        if (prod.has_value()){
            for (const auto &el : prod.value().get().content){
                std::cout << el->string_representation;
            }
        }
        else{
            std::cout << " epsilon" << std::endl;
        }
        std::cout << std::endl;
    }

    [[nodiscard]] std::string to_string() const {
        std::string result;
        result += this->lhs->string_representation;
        result += " -> ";
        if (this->prod.has_value()){
            for (auto &thing : this->prod.value().get().content){
                result += thing->string_representation;
                result += " ";
            }
        }
        else {
            result += " epsilon ";
        }
        return result;
    }
};

class grammar{

public:
    // Return nullptr if grammar unit has not been found
    static const grammar_unit* find_gu_by_name(const std::vector<grammar_unit> &universe,
                                               const std::string &name){
        auto res = std::find_if(universe.begin(), universe.end(), [&name](const grammar_unit &gu){
             return (gu.string_representation == name);
        });
        if (res == universe.end()) return nullptr;
        else return res.base();
    }

    typedef std::unordered_map<const grammar_unit*, production_group> production_array_t;

    static production_array_t parse_productions(
            std::vector<grammar_unit> &universe,
            std::vector<std::pair<std::string, std::vector<std::string>>> &productions_raw
            ) {

        production_array_t parsed_prods;

        // Create keys for @parsed_prods map, using provided universe container
        for (const grammar_unit &gu : universe){
            if (gu.terminal) continue;
            std::pair<const grammar_unit*, production_group> insertable { &gu, {} };
            parsed_prods.insert(insertable);
        }


        for (const auto& raw_production : productions_raw){
            try{
                auto gu = find_gu_by_name(universe, raw_production.first);
                if (!gu) throw std::runtime_error("GU not found");
                auto &production_group = parsed_prods.at(gu);
                production new_prod;

                if (raw_production.second[0] == " ") {
                    production_group.activate_epsilon_production();
                    continue;
                }

                for (int i = 0; i < raw_production.second.size(); i++){
                    auto found = find_gu_by_name(universe, raw_production.second[i]);
                    if (!found){
                        std::string err = "Grammar unit not found in universe container: ";
                        err += raw_production.second[i];
                        throw std::runtime_error(err);
                    }
                    new_prod.content.push_back(found);
                }
                production_group.add_production(new_prod);
            }
            catch (std::out_of_range &e){
                std::cout << "One of productions has an undefined grammar unit" << std::endl;
            }

        }
        return parsed_prods;
    }

    typedef std::unordered_map<const grammar_unit*, first_set> first_set_t;

    static first_set& _get_first_set(
            std::vector<grammar_unit> &universe,
            production_array_t &productions,
            const grammar_unit *current_pg_lhs,
            production_group &current_pg_rhs,
            first_set_t &first_set_val,
            std::unordered_map<const grammar_unit*, bool> &first_set_calc_status, size_t recursion_depth) {

        if (recursion_depth > 1000){
            std::string err;
            pbl_utility::str_compose(err, "Grammar is most probably left-recursive (Recursion depth > 1000), current LHS is ", current_pg_lhs->string_representation);
            throw std::runtime_error(err);
        }

        // Returns if current production group is already calculated
        if (!current_pg_lhs->is_term() && first_set_calc_status.at(current_pg_lhs)){
            return first_set_val.at(current_pg_lhs);
        }

        first_set &position_in_first_set = first_set_val.at(current_pg_lhs);

        // For all productions in current non terminal
        for (const production &p : current_pg_rhs.get_all_productions()) {

            bool current_nt_has_epsilon = true;
            for (int i = 0; i < p.content.size(); i++) {
                const grammar_unit *next_gu = p.content[i];

                // Same gu case
                if (next_gu == current_pg_lhs) {
                    if (!current_pg_rhs.has_epsilon_production()) {
                        current_nt_has_epsilon = false;
                        break;
                    }
                    else continue;
                }

                // Terminal case
                if (next_gu->terminal) {
                    position_in_first_set.content.insert(next_gu);
                    current_nt_has_epsilon = false;
                    break;
                }

                // Non terminal case
                else {
                    first_set first_down = _get_first_set(universe, productions,
                                                          next_gu, productions.at(next_gu), first_set_val, first_set_calc_status, recursion_depth++);
                    position_in_first_set.content.merge(first_down.content);
                    if (!first_down.has_epsilon) {
                        current_nt_has_epsilon = false;
                        break;
                    }
                }
            }
            if (current_nt_has_epsilon) position_in_first_set.has_epsilon = true;
        }
        if (current_pg_rhs.has_epsilon_production()) position_in_first_set.has_epsilon = true;
        auto & val = first_set_calc_status.at(current_pg_lhs); val = true;
        return position_in_first_set;
    }

    static void D_first_set_out(std::unordered_map<const grammar_unit*, first_set> &first_set_val){
#ifdef DEBUG_MODE
        for (const auto &pair : first_set_val){
            std::cout << pair.first->string_representation << " = " << "{";
            for (auto el : pair.second.content){
                std::cout << el->string_representation << ", ";
            }
            if (pair.second.has_epsilon) std::cout << "ε";
            std::cout << "}" << std::endl;
        }
    }
#endif
    static void D_follow_set_out(std::unordered_map<const grammar_unit*, follow_set> &set){
#ifdef DEBUG_MODE
        for (const auto &pair : set){
            std::cout << pair.first->string_representation << " = " << "{";
            for (auto el : pair.second.content){
                std::cout << el->string_representation << ", ";
            }
            if (pair.second.has_dollar) std::cout << "$";
            std::cout << "}" << std::endl;
        }
    }
#endif
    // Returns pair <first_set_t, bool>, where first_set_t indicates first array for every non terminal GU.
    // and bool map represents if that non terminal got an epsilon production.
    static std::unordered_map<const grammar_unit*, first_set>
    get_first_set(std::vector<grammar_unit> &universe,
                  production_array_t &productions,
                  const grammar_unit *start_symbol){

        production_group &start_pg = productions.at(start_symbol);
        // Fill all existing left-hand-sides of given productions;
        std::unordered_map<const grammar_unit*, first_set> first_set_val;
        std::unordered_map<const grammar_unit*, bool> first_set_calc_status;
        for (auto &pg : productions){
            first_set_val.emplace(pg.first, first_set{});
            first_set_calc_status.emplace(pg.first, false);
        }

        for (auto &pg : productions){
            _get_first_set(universe, productions, pg.first, pg.second,
                           first_set_val, first_set_calc_status, 1);
        }

        return first_set_val;
    }

    static std::unordered_map<const grammar_unit*, follow_set> get_follow_set(std::vector<grammar_unit> &universe, production_array_t &productions,
                                     const grammar_unit *start_symbol,
                                     std::unordered_map<const grammar_unit*, first_set> &first_set){

        // Add one follow set to another. Returns true if receiver has been modified
        auto add_follow = [](follow_set &receiver, follow_set &dealer) -> bool {
            bool eof_sign_added = false;
            auto old_size = receiver.content.size();
            if (dealer.has_dollar && !receiver.has_dollar){
                receiver.has_dollar = true;
                eof_sign_added = true;
            }
            receiver.content.merge(dealer.content);
            return (old_size != receiver.content.size() || eof_sign_added);
        };

        // Add first set to follow set. Returns true if set has been modified
        auto add_first = [](follow_set &receiver, std::set<const grammar_unit*> &first_set) -> bool {
            auto old_size = receiver.content.size();
            receiver.content.merge(first_set);
            return (old_size != receiver.content.size());
        };

        // Same as in #1 and #2, but adds a terminal
        auto add_terminal = [](follow_set &receiver, const grammar_unit *gu) -> bool {
            auto old_size = receiver.content.size();
            receiver.content.insert(gu);
            return (old_size != receiver.content.size());
        };

        std::unordered_map<const grammar_unit*, follow_set> follow_set_map;
        for (const grammar_unit &gu : universe){
            if (gu.terminal) continue;
            follow_set_map.try_emplace(&gu, follow_set{false});
        }
        follow_set_map.at(start_symbol).has_dollar = true;

        bool has_changes;
        do {
            has_changes = false;
            for (const auto &pg : productions){
                for (const production &prod : pg.second.get_all_productions()){
                    for (size_t i = 0; i < prod.content.size(); i++){
                        const grammar_unit *gu = prod.content[i];
                        if (gu->terminal) continue;

                        if (i == prod.content.size() - 1){
                            if (add_follow(follow_set_map.at(gu), follow_set_map.at(pg.first)))
                                has_changes = true;
                            break;
                        }
                        else if (!prod.content[i+1]->terminal){
                            auto first_of_next_symbol = first_set.at(prod.content[i+1]);
                            if (add_first(follow_set_map.at(gu), first_of_next_symbol.content))
                                has_changes = true;
                            if (first_of_next_symbol.has_epsilon){
                                if (add_follow(follow_set_map.at(gu), follow_set_map.at(pg.first)))
                                    has_changes = true;
                            }
                        }
                        else{
                            if (add_terminal(follow_set_map.at(gu), prod.content[i+1]))
                                has_changes = true;
                        }
                    }
                }
            }
        } while (has_changes);
        return follow_set_map;
    }


    static std::vector<predict_set_record>
    get_predict_set(std::vector<grammar_unit> &universe, production_array_t &productions,
                                const grammar_unit *start_symbol,
                                std::unordered_map<const grammar_unit*, first_set> &first_set){

        std::vector<predict_set_record> predict_set;
        for (const auto &pg : productions){
            for (const auto &prod : pg.second.get_all_productions()){
                predict_set_record temp {pg.first, prod};
                predict_set.push_back(temp);
                predict_set_record &current = predict_set.back();

                bool all_nt_derive_epsilon = true;
                for (const grammar_unit *gu : prod.content){
                    if (gu->terminal){
                        current.add_to_first(gu);
                        all_nt_derive_epsilon = false;
                        break;
                    }
                    else{
                        auto non_term_first = first_set.at(gu);
                        current.add_to_first(non_term_first.content);
                        if (!non_term_first.has_epsilon) {
                            all_nt_derive_epsilon = false;
                            break;
                        }
                    }
                }
                if (all_nt_derive_epsilon) current.fs.has_epsilon = true;
            }
            if (pg.second.has_epsilon_production()) {
                predict_set_record temp {pg.first, {}};
                predict_set.push_back(temp);
            }
        }
        return predict_set;
    }

    static void D_predict_out(std::vector<predict_set_record> &predict_set){
#ifdef DEBUG_MODE
        for (auto &el : predict_set){
            if (el.prod.has_value()){
                std::cout << "[";
                std::cout << el.lhs->string_representation << " -> ";
                for (const auto &el_2 : el.prod->get().content){
                    std::cout << el_2->string_representation;
                }
                std::cout << "]: {";
                for (const auto &el_2 : el.fs.content){
                    std::cout << el_2->string_representation << ", ";
                }
                if (el.fs.has_epsilon) std::cout << " epsilon";

            }
            else {
                std::cout << "[" << el.lhs->string_representation << " -> epsilon" <<  "]: {"
                << "epsilon";
            }
            std::cout << "}";
            std::cout << std::endl;
        }
    }
#endif
};

#endif //LEXER_TEST_GRAMMAR_H
