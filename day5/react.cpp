#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

constexpr char blank = ' ';

[[gnu::pure]] constexpr bool is_reactive(char a, char b) noexcept {
    auto dist =  a - b;
    return dist == ('A' - 'a') || dist == ('a' - 'A');
}

template <typename C>
void react(C & container) {
    auto prev = [begin=container.begin()] (auto it) {
                    while (*it == blank && it != begin) {
                        --it;
                    }
                    return it;
                };
    auto it_a = container.begin();
    auto it_b = std::next(it_a);
    for (; it_b != container.end(); it_a = prev(it_b), ++it_b) {
        if (is_reactive(*it_a, *it_b)) {
            *it_a = *it_b = blank;
        }
    }
}

int main() {
    auto str = [] () -> std::string {
                   std::ifstream is("input");
                   return {std::istream_iterator<char>{is},
                           std::istream_iterator<char>{}};
               }();

    react(str);

    auto count = [] (std::string const& str) {
                     return std::count_if(str.begin(), str.end(),
                                          [] (char c) { return c != blank; });
                 };

    std::cout << count(str)
              << std::endl;
    // std::cout << str << std::endl;

    // Part 2

    char best;
    size_t best_count = std::numeric_limits<size_t>::max();
    for (char c = 'a'; c <= 'z'; ++c) {
        char C = c + ('A' - 'a');
        std::string removed;
        std::copy_if(str.begin(), str.end(), std::back_inserter(removed),
                     [c, C] (char x) { return x != c && x != C; });
        react(removed);
        size_t this_count = count(removed);
        if (this_count < best_count) {
            best_count = this_count;
            best = c;
        }
    }

    std::cout << "Best result by removing \'" << best << "\' ("
              << best_count << ")\n";
}
