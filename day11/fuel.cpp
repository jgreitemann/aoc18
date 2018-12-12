#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <limits>
#include <vector>

static constexpr inline int serial = 7400;

struct sum_result {
    int x, y, val, size = std::numeric_limits<int>::min();
};

template <typename Array>
sum_result max_at_size(Array & power, int size) {
    int L = 301 - size;
    std::vector<int> sums_vec(L * L);
    auto sum = [&sums_vec, L] (size_t i, size_t j) -> int& {
                   return sums_vec[(i-1) + (j-1) * L];
               };
    for (int x = 1; x <= L; ++x) {
        for (int y = 1; y <= L; ++y) {
            sum(x, y) = 0;
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    sum(x, y) += power(x + i, y + j);
                }
            }
        }
    }

    sum_result best{};
    best.size = size;
    for (int x = 1; x <= L; ++x) {
        for (int y = 1; y <= L; ++y) {
            if (sum(x, y) > best.val) {
                best.x = x;
                best.y = y;
                best.val = sum(x, y);
            }
        }
    }
    return best;
}

int main() {
    std::vector<int> power_vec(300 * 300);
    auto power = [&power_vec] (size_t i, size_t j) -> int& {
                     return power_vec[(i-1) + (j-1) * 300];
                 };
    for (int x = 1; x <= 300; ++x) {
        for (int y = 1; y <= 300; ++y) {
            int & p = power(x, y);
            int rack_id = x + 10;
            p = rack_id * y + serial;
            p *= rack_id;
            p = (p / 100) % 10 - 5;
        }
    }

    // Part 1
    {
        sum_result best = max_at_size(power, 3);
        std::cout << best.x << ',' << best.y << '\n';
    }

    // Part 2
    {
        sum_result best{};
        for (size_t s = 1; s <= 300; ++s) {
            sum_result res = max_at_size(power, s);
            if (res.val > best.val)
                best = res;
        }
        std::cout << best.x << ',' << best.y << ',' << best.size << '\n';
    }

}
