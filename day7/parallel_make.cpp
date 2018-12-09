#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <regex>
#include <set>
#include <string>

constexpr inline static int base_duration = 60;
constexpr inline static size_t n_workers = 4;

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

    auto yield = [&] {
                     auto it = rules.begin();
                     while (it != rules.end() && !it->second.empty())
                         ++it;
                     return it;
                 };
    std::map<char, int> running;
    std::string done;

    for (size_t s = 0; !rules.empty() || !running.empty(); ++s) {
        // log progress
        for (auto & [c, t] : running) {
            t--;
        }

        // clear done tasks
        for (auto it = running.begin(); it != running.end();) {
            if (it->second < 0) {
                // announce done
                done += it->first;

                // update dependencies
                for (auto & [c, d] : rules) {
                    d.erase(it->first);
                }

                // remove running task
                it = running.erase(it);
            } else {
                ++it;
            }
        }

        // check for idle workers and dispatch tasks
        decltype(yield()) task_it;
        while (running.size() < n_workers && (task_it = yield()) != rules.end()) {
            running[task_it->first] = task_it->first - 'A' + base_duration;
            rules.erase(task_it);
        }

        // print
        std::cout << s << '\t';
        std::transform(running.begin(), running.end(),
                       std::ostream_iterator<char>{std::cout, "\t"},
                       [] (auto const& p) { return p.first; });
        std::fill_n(std::ostream_iterator<char>{std::cout, "\t"},
                    n_workers - running.size(), '.');

        std::cout << done << std::endl;
    }
}
