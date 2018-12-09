#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <regex>
#include <set>
#include <string>

int main() {
    auto str = [] () -> std::string {
                   std::ifstream is("input");
                   return {
                       std::istreambuf_iterator<char>{is},
                       std::istreambuf_iterator<char>{}
                   };
               }();
    std::regex re{"Step ([A-Z]) must be finished before step ([A-Z]) can begin"};

    std::map<char, std::set<char>> rules;
    std::for_each(std::sregex_iterator{str.begin(), str.end(), re},
                  std::sregex_iterator{},
                  [&] (std::smatch const& match) {
                      auto it = rules.insert({match[2].str()[0], {}}).first;
                      rules.insert({match[1].str()[0], {}});
                      it->second.insert(match[1].str()[0]);
                  });

    while (!rules.empty()) {
        auto it = rules.begin();
        while (!it->second.empty())
            ++it;

        std::cout << it->first;

        for (auto & [c, d] : rules) {
            d.erase(it->first);
        }

        rules.erase(it);
    }
}
