#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <set>
#include <string>

template <typename It>
int env_key(It it, size_t len = 5) {
    return std::accumulate(it, it + len, 0,
                           [] (int acc, char c) {
                               return acc << 1 | (c == '#');
                           });
}

int main() {
    auto str = [] () -> std::string {
                   std::ifstream is{"input"};
                   return {std::istreambuf_iterator<char>{is},
                           std::istreambuf_iterator<char>{}};
               }();

    std::set<int> rules;
    {
        std::regex re{"([\\.#]{5}) => #"};
        std::transform(std::sregex_iterator{str.begin(), str.end(), re},
                       std::sregex_iterator{},
                       std::inserter(rules, rules.end()),
                       [] (std::smatch const& match) {
                           return env_key(match[1].str().begin());
                       });
    }

    auto board = [&] () -> std::deque<char> {
        std::regex re{"initial state: ([\\.#]+)"};
        std::smatch match;
        std::regex_search(str, match, re);
        auto mstr = match[1].str();
        return {mstr.begin(), mstr.end()};
    }();
    std::ptrdiff_t l_pad = 0;
    auto pad = [&] {
                   const size_t min_pad = 4;
                   auto first = std::find(board.begin(), board.end(), '#');
                   if (first - board.begin() < min_pad) {
                       l_pad -= min_pad - (first - board.begin());
                       std::fill_n(std::front_inserter(board),
                                   min_pad - (first - board.begin()), '.');
                   }
                   auto last = std::find(board.rbegin(), board.rend(), '#');
                   if (last - board.rbegin() < min_pad)
                       std::fill_n(std::back_inserter(board),
                                   min_pad - (last - board.rbegin()), '.');
               };
    std::deque<char> work;
    for (size_t n = 1; n <= 1000; ++n) {
        pad();
        work.resize(board.size(), '.');
        auto it = board.begin();
        auto oit = work.begin() + 2;
        for (; it < board.end() - 5; ++it, ++oit)
            *oit = rules.find(env_key(it)) != rules.end() ? '#' : '.';
        board.swap(work);

        // std::copy(board.begin(), board.end(),
        //           std::ostream_iterator<char>{std::cout});
        // std::cout << std::endl;

        std::ptrdiff_t total = 0;
        std::ptrdiff_t i = l_pad;
        for (char c : board) {
            if (c == '#')
                total += i;
            ++i;
        }

        std::cout << n << '\t' << total << std::endl;
    }

    // linear fit ("glider" cycle)
    size_t n = 50'000'000'000;
    std::ptrdiff_t total = 67 * (n - 99) + 6633;
    std::cout << n << '\t' << total << std::endl;
}
