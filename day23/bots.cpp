#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <regex>
#include <string>

struct pos {
    long x, y, z;

    long manhattan() const {
        return abs(x) + abs(y) + abs(z);
    }
};

pos operator+(pos lhs, pos const& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

pos operator-(pos lhs, pos const& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

int main() {
    auto str = []() -> std::string {
                   std::ifstream is("input");
                   return {std::istreambuf_iterator<char>{is},
                           std::istreambuf_iterator<char>{}};
               }();

    std::multimap<long, pos> bots;
    std::regex re{"pos=<(-?[0-9]+),(-?[0-9]+),(-?[0-9]+)>, r=([0-9]+)"};
    std::transform(std::sregex_iterator{str.begin(), str.end(), re},
                   std::sregex_iterator{},
                   std::inserter(bots, bots.begin()),
                   [](std::smatch const& m) -> std::pair<long, pos> {
                       return {std::stoi(m[4]),
                               {std::stoi(m[1]), std::stoi(m[2]), std::stoi(m[3])}};
                   });

    auto [rmax, origin] = *bots.rbegin();

    // part 1
    std::cout << "number of bots in range: "
              << std::count_if(bots.begin(), bots.end(),
                               [&](auto const& p) {
                                   return (p.second - origin).manhattan() <= rmax;
                               })
              << "\n\n";

    // part 2
    auto n_in_range = [&](pos o) {
        return std::count_if(bots.begin(), bots.end(),
                            [&](auto const& p) {
                                return (p.second - o).manhattan() <= p.first;
                            });
    };
    origin = {21620363, 45010237, 44677673};
    pos best = origin;
    size_t n_best = n_in_range(best);

    while (true) {
        [&] {
            for (long r = 1; ; ++r) {
                for (long y = -r; y <= r; ++y) {
                    for (long x = -r; x <= r; ++x) {
                        auto test = [&](pos o) {
                            size_t n = n_in_range(o);
                            if (n > n_best
                                || (n == n_best && o.manhattan() < best.manhattan()))
                            {
                                best = o;
                                n_best = n;
                                std::cout << "new best: " << best.manhattan()
                                          << " (n = " << n_best << ", r = "
                                          << r << ")\n";
                                return true;
                            }
                            return false;
                        };

                        if (test(best + pos{-r, x, y})) return;
                        if (test(best + pos{+r, x, y})) return;
                        if (test(best + pos{x, -r, y})) return;
                        if (test(best + pos{x, +r, y})) return;
                        if (test(best + pos{x, y, -r})) return;
                        if (test(best + pos{x, y, +r})) return;
                    }
                }
            }
        }();
    }
}
