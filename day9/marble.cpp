#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

size_t game(size_t n_players, size_t max_marble) {
    std::deque<size_t> circle {0};
    std::vector<size_t> scores(n_players);

    auto current = circle.begin();
    using it_t = typename decltype(circle)::iterator;
    using diff_t = typename decltype(circle)::difference_type;
    auto next = [&] (it_t it, diff_t n = 1) {
                    diff_t i = it - circle.begin();
                    while (n < 0)
                        n += circle.size();
                    i += n;
                    return circle.begin() + (i % circle.size());
                };

    size_t p = 0;
    for (size_t m = 1; m <= max_marble; ++m, p = (p + 1) % n_players) {
        if (m % 23 == 0) {
            scores[p] += m + *(current = next(current, -7));
            current = circle.erase(current);
        } else {
            current = circle.insert(next(current, 1) + 1, m);
        }
    }
    
    return *std::max_element(scores.begin(), scores.end());
}

int main() {
    std::ifstream is{"input"};
    std::string str{std::istreambuf_iterator<char>{is},
                    std::istreambuf_iterator<char>{}};

    std::regex re{"([0-9]+) players; last marble is worth ([0-9]+) points"};
    std::for_each(std::sregex_iterator{str.begin(), str.end(), re},
                  std::sregex_iterator{},
                  [] (std::smatch const& match) {
                      std::cout << match.str() << ": high score is "
                                << game(std::stoul(match[1]), std::stoul(match[2]))
                                << '\n';
                  });
}
