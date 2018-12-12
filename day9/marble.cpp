#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

#include "rotate.hpp"

size_t game(size_t n_players, size_t max_marble) {
    std::deque<size_t> circle {0};
    std::vector<size_t> scores(n_players);

    size_t p = 0;
    for (size_t m = 1; m <= max_marble; ++m, p = (p + 1) % n_players) {
        if (m % 23 == 0) {
            scores[p] += m + *rotate(circle, -7);
            circle.pop_front();
        } else {
            circle.insert(rotate(circle, 2), m);
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
