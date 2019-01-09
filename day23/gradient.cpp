#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <regex>
#include <string>

template <typename T = long>
struct pos : std::array<T, 3> {
    T manhattan() const {
        return abs((*this)[0]) + abs((*this)[1]) + abs((*this)[2]);
    }

    template <typename S>
    pos<S> as() const {
        return {
            static_cast<S>((*this)[0]),
            static_cast<S>((*this)[1]),
            static_cast<S>((*this)[2])
        };
    }
};

template <typename T>
pos<T> operator+(pos<T> lhs, pos<T> const& rhs) {
    lhs[0] += rhs[0];
    lhs[1] += rhs[1];
    lhs[2] += rhs[2];
    return lhs;
}

template <typename T>
pos<T> operator-(pos<T> lhs, pos<T> const& rhs) {
    lhs[0] -= rhs[0];
    lhs[1] -= rhs[1];
    lhs[2] -= rhs[2];
    return lhs;
}

int main() {
    auto str = []() -> std::string {
                   std::ifstream is("input");
                   return {std::istreambuf_iterator<char>{is},
                           std::istreambuf_iterator<char>{}};
               }();

    std::multimap<long, pos<>> bots;
    std::regex re{"pos=<(-?[0-9]+),(-?[0-9]+),(-?[0-9]+)>, r=([0-9]+)"};
    std::transform(std::sregex_iterator{str.begin(), str.end(), re},
                   std::sregex_iterator{},
                   std::inserter(bots, bots.begin()),
                   [](std::smatch const& m) -> std::pair<long, pos<>> {
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
    auto n_in_range = [&](pos<> o) {
        return std::count_if(bots.begin(), bots.end(),
                             [&](auto const& p) {
                                 return (p.second - o).manhattan() <= p.first;
                             });
    };
    // too low: 110670411
    // origin = {21622252, 45010141, 44681690};
    pos<double> w = origin.as<double>();
    // double eta = 1e34;
    double eta = 1e7;
    size_t n_best = n_in_range(origin);
    pos<long> best = origin;

    for (size_t k = 0; ; ++k) {
        // eta = 1e14 * pow(0.5 * (1 + tanh((750.-n_best)/20.)), -4);
        std::array<double, 3> delta{};
        for (auto [r, x] : bots) {
            pos<double> loc{};
            double prefac = 1. / pow(r / 1e6, 3);
            double expo = 0;
            for (size_t j = 0; j < 3; ++j) {
                loc[j] += -std::abs(w[j] - x[j]) * (w[j] - x[j]) / r / r;
                expo -= std::pow(1. * (w[j] - x[j]) / r, 2);
            }
            expo = exp(expo);
            for (size_t j = 0; j < 3; ++j) {
                delta[j] += prefac * loc[j] * expo;
            }
        }
        for (size_t j = 0; j < 3; ++j) {
            w[j] += eta * delta[j];
        }

        size_t n = n_in_range(w.as<long>());
        if (n >= n_best) {
            if (n > n_best || (n == n_best && w.as<long>().manhattan() < best.manhattan())) {
                n_best = n;
                best = w.as<long>();
            }
        }
        if (k > 10000 && k % 10000 == 0) {
            auto best = w.as<long>();
            std::cout << "iteration: " << best.manhattan()
                      << " = ||" << best[0] << ", " << best[1] << ", " << best[2] << "||_1"
                      << " (n = " << n_best << ", eta = " << eta << ")\n";
            std::cout << eta * delta[0] << "\n";
        }
    }
}
