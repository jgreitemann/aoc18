#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

static inline char const * const file = "input";
static inline size_t const dim = 50;

struct location {
    int x, y;
    location(int x, int y) : x{x}, y{y} {}
    location(size_t i)
        : x{static_cast<int>(i % dim)}
        , y{static_cast<int>(i / dim)} {}
    operator size_t() const {
        return y * dim + x;
    }
    std::array<location, 8> neighbors() const {
        return {
            location{x+1, y},
            location{x+1, y+1},
            location{x, y+1},
            location{x-1, y+1},
            location{x-1, y},
            location{x-1, y-1},
            location{x, y-1},
            location{x+1, y-1}
        };
    }
    operator bool() const {
        return x >= 0 && x < dim && y >= 0 && y < dim;
    }
};

int main() {
    auto acres = []() -> std::vector<char> {
                     std::ifstream is{file};
                     return {std::istream_iterator<char>{is},
                             std::istream_iterator<char>{}};
                 }();
    std::vector<char> swap(acres.size());

    auto print = [&acres] {
                     size_t x = 0;
                     for (char c : acres) {
                         std::cout << c;
                         if ((++x, x = x % dim) == 0)
                             std::cout << '\n';
                     }
                     std::cout << '\n';
                 };

    auto n_trees = [&] (auto const& x) {
                       return std::count(x.begin(), x.end(), '|');
                   };
    auto n_yards = [&] (auto const& x){
                       return std::count(x.begin(), x.end(), '#');
                   };
    auto evolve = [&] {
                      for (size_t i = 0; i < acres.size(); ++i) {
                          auto neighbors = location{i}.neighbors();
                          std::array<char, 8> nstates;
                          std::transform(neighbors.begin(), neighbors.end(),
                                         nstates.begin(),
                                         [&] (location l) {
                                             if (l)
                                                 return acres[l];
                                             else
                                                 return '\0';
                                         });
                          switch (acres[i]) {
                          case '.':
                              swap[i] = n_trees(nstates) >= 3
                                  ? '|' : '.';
                              break;
                          case '|':
                              swap[i] = n_yards(nstates) >= 3
                                  ? '#' : '|';
                              break;
                          case '#':
                              swap[i] = (std::find(nstates.begin(),
                                                   nstates.end(),
                                                   '#') != nstates.end()
                                         && std::find(nstates.begin(),
                                                      nstates.end(),
                                                      '|') != nstates.end())
                                  ? '#' : '.';
                              break;
                          }
                      }
                      std::swap(acres, swap);
                  };

    // print();
    for (size_t i = 0; i < 1000000000; ++i) {
        evolve();
        std::cout << i << '\t' << n_trees(acres) * n_yards(acres) << '\n';
        // print();
    }

    std::cout << "Resource value: " << n_trees(acres) * n_yards(acres) << '\n';
}
