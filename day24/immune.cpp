#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

enum struct type {
    bludgeoning,
    cold,
    fire,
    radiation,
    slashing
};

type stotype(std::string_view sv) {
    if (sv == "bludgeoning")
        return type::bludgeoning;
    if (sv == "cold")
        return type::cold;
    if (sv == "fire")
        return type::fire;
    if (sv == "radiation")
        return type::radiation;
    if (sv == "slashing")
        return type::slashing;
    throw std::runtime_error("invalid attack type");
    return type::bludgeoning;
}

std::set<type> typeset_from_sv(std::string_view sv) {
    std::set<type> s;
    if (sv.find("bludgeoning") != std::string_view::npos)
        s.insert(type::bludgeoning);
    if (sv.find("cold") != std::string_view::npos)
        s.insert(type::cold);
    if (sv.find("fire") != std::string_view::npos)
        s.insert(type::fire);
    if (sv.find("radiation") != std::string_view::npos)
        s.insert(type::radiation);
    if (sv.find("slashing") != std::string_view::npos)
        s.insert(type::slashing);
    return s;
}

template <typename V>
V & append_iters(V & its) {
    return its;
}

template <typename V, typename C, typename... Rest>
V & append_iters(V & its, C & c, Rest&... rest) {
    for (size_t i = 0; i < c.size(); ++i)
        its.push_back(c.begin() + i);
    return append_iters(its, rest...);
}

size_t total_size() {
    return 0;
}

template <typename C, typename... Rest>
size_t total_size(C const& c, Rest const&... rest) {
    return c.size() + total_size(rest...);
}

template <typename C, typename... Rest>
auto get_iters(C & c, Rest&... rest) {
    std::vector<typename C::iterator> its;
    its.reserve(total_size(c, rest...));
    append_iters(its, c, rest...);
    return its;
}

struct group {
    int n_units;
    int hp_each;
    int attack_damage;
    type attack;
    int initiative;
    std::set<type> immunities;
    std::set<type> weaknesses;
};

int main() {
    auto [friends, foes] = [] {
        std::regex re{"([0-9]+) units each with ([0-9]+) hit points "
                      "(?:\\((?:immune to ([a-z, ]+))?(?:; )?(?:weak to ([a-z, ]+))?\\) )?"
                      "with an attack that does ([0-9]+) ([a-z]+) damage "
                      "at initiative ([0-9]+)"};
        using svregex_iterator = std::regex_iterator<std::string_view::iterator>;
        using svmatch = std::match_results<std::string_view::iterator>;
        auto parse = [&re] (std::string_view sv) {
            std::vector<group> gr;
            std::transform(svregex_iterator{sv.begin(), sv.end(), re},
                           svregex_iterator{},
                           std::back_inserter(gr),
                           [&](svmatch const& m) -> group {
                               return {
                                   std::stoi(m[1]),
                                   std::stoi(m[2]),
                                   std::stoi(m[5]),
                                   stotype(m[6].str()),
                                   std::stoi(m[7]),
                                   typeset_from_sv(m[3].str()),
                                   typeset_from_sv(m[4].str())
                               };
                           });
            return gr;
        };
        auto str = []() -> std::string {
                       std::ifstream is{"example"};
                       return {std::istreambuf_iterator<char>{is},
                               std::istreambuf_iterator<char>{}};
                   }();
        std::string_view sv{str};
        size_t sep = sv.find("Infection");
        return std::pair{parse(sv.substr(0, sep)),
                         parse(sv.substr(sep))};
    }();

    // target selection
    auto all_attackers = get_iters(friends, foes);
    auto all_defenders = all_attackers;

    while (!all_attackers.empty())


}
