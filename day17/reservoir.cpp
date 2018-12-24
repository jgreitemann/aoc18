#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

template <typename F>
struct Y {
    template <typename... X>
    decltype(auto) operator()(X &&... x) const &
    {
        return f(*this, std::forward<X>(x)...);
    }
    F f;
};

template <typename F>
Y(F) -> Y<F>;

template <typename... Bases>
struct overload : Bases... {
    using Bases::operator()...;
};
template <typename... Bases>
overload(Bases...) -> overload<Bases...>;

template <char c>
struct base_vein {
    int a;
    int b_min, b_max;
};

using vein = std::variant<base_vein<'x'>, base_vein<'y'>>;

template <char c, char cc, typename It>
void parse_veins(std::vector<vein> & v, It sbegin, It send) {
    std::regex re{[] {
            std::stringstream ss;
            ss << c << "=([0-9]+), " << cc << "=([0-9]+)..([0-9]+)";
            return ss.str();
        }()};
    std::transform(std::sregex_iterator{sbegin, send, re},
                   std::sregex_iterator{},
                   std::back_inserter(v),
                   [](std::smatch const& m) -> vein {
                       using std::stoi;
                       return base_vein<c>{stoi(m[1]), stoi(m[2]), stoi(m[3]) + 1};
                   });
}

int main() {
    auto str = []() -> std::string {
        std::ifstream is{"input"};
        return {std::istreambuf_iterator<char>{is},
                std::istreambuf_iterator<char>{}};
    }();

    std::vector<vein> veins;
    parse_veins<'x', 'y'>(veins, str.begin(), str.end());
    parse_veins<'y', 'x'>(veins, str.begin(), str.end());

    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::min();
    for (auto const& v : veins) {
        std::visit(overload{
                [&](base_vein<'x'> const& v) {
                    min_x = std::min(min_x, v.a);
                    max_x = std::max(max_x, v.a + 1);
                    min_y = std::min(min_y, v.b_min);
                    max_y = std::max(max_y, v.b_max);
                },
                [&](base_vein<'y'> const& v) {
                    min_x = std::min(min_x, v.b_min);
                    max_x = std::max(max_x, v.b_max);
                    min_y = std::min(min_y, v.a);
                    max_y = std::max(max_y, v.a + 1);
                }
            }, v);
    }
    --min_x;
    ++max_x;
    size_t width = max_x - min_x;


    std::vector<char> scan(max_y * width, '.');
    for (auto const& v : veins) {
        std::visit(overload{
                [&](base_vein<'x'> const& v) {
                    for (int y = v.b_min; y < v.b_max; ++y)
                        scan[y * width + v.a - min_x] = '#';
                },
                [&](base_vein<'y'> const& v) {
                    for (int x = v.b_min; x < v.b_max; ++x)
                        scan[v.a * width + x - min_x] = '#';
                }
            }, v);
    }

    auto avail = [&](size_t i) {
                     return scan[i] != '#' && scan[i] != '~';
                 };
    auto drop = Y{[&](auto self, size_t i) {
        scan[i] = '|';

        // drop
        for (i += width; i < scan.size() && avail(i); i += width) {
            scan[i] = '|';
        }
        if (i >= scan.size()) {
            return false;
        }
        i -= width;

        // spread
        bool contained = true;
        bool changed = false;
        auto spread = [&](size_t j, std::ptrdiff_t dir) {
                          for (j += dir; avail(j); j += dir) {
                              scan[j] = '|';
                              if (avail(j + width)) {
                                  changed |= self(j);
                                  contained = false;
                                  return 0ul;
                              }
                          }
                          return j;
                      };
        size_t right = spread(i, +1);
        size_t left = spread(i, -1);

        if (contained) {
            for (size_t j = left + 1; j < right; ++j) {
                scan[j] = '~';
            }
            return true;
        }
        return changed;
    }};

    for (size_t d = 0; drop(500-min_x); ++d) {}

    for (int y = 0; y < max_y; ++y) {
        for (size_t i = 0; i < width; ++i) {
            std::cout << scan[y * width + i];
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "Number of water tiles: "
              << std::count_if(scan.begin() + min_y * width, scan.end(),
                               [](char c) {
                                   return c == '~' || c == '|';
                               })
              << "\nNumber of permanent water tiles: "
              << std::count(scan.begin() + min_y * width, scan.end(), '~')
              << '\n';

}
