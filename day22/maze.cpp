#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <utility>
#include <vector>
#include "../functional.hpp"

struct coord {
    long x, y;
    constexpr operator size_t() const;
    explicit constexpr operator bool() const;

    constexpr coord left() const {
        return {x - 1, y};
    }

    constexpr coord right() const {
        return {x + 1, y};
    }

    constexpr coord top() const {
        return {x, y - 1};
    }

    constexpr coord bottom() const {
        return {x, y + 1};
    }

    constexpr std::array<coord, 4> neighbors() const {
        return {left(), right(), top(), bottom()};
    }
};

// static constexpr inline size_t depth = 510;
// static constexpr inline coord target = {10, 10};

static constexpr inline size_t depth = 3066;
static constexpr inline coord target = {13, 726};

static constexpr inline coord dim = {75, 1500};

constexpr coord::operator size_t() const {
    return y * (dim.x + 1) + x;
}

constexpr coord::operator bool() const {
    return x >= 0 && x < dim.x && y >= 0 && y < dim.y;
}

constexpr size_t erosion_level(size_t gi) {
    return (gi + depth) % 20183;
}

struct mode {
    constexpr operator size_t() {
        return static_cast<size_t>(x);
    }
    constexpr mode() : x(neither) {}
    constexpr mode(size_t x) : x(static_cast<mode_>(x)) {}
    enum mode_ {
        torch,
        climbing_gear,
        neither
    };
    constexpr static size_t N = 3;
private:
    mode_ x;
};

struct region {
    constexpr operator size_t() {
        return static_cast<size_t>(x);
    }
    constexpr region(size_t x) : x(static_cast<region_>(x)) {}
    constexpr std::array<mode, 2> allowed_tools() const {
        switch (x) {
        case rocky:
            return {mode::climbing_gear, mode::torch};
        case wet:
            return {mode::climbing_gear, mode::neither};
        case narrow:
            return {mode::torch, mode::neither};
        default:
            return {};
        }
    }
    constexpr bool can_enter(mode tool) const {
        auto t = allowed_tools();
        return t[0] == tool || t[1] == tool;
    }
    constexpr mode other_tool(mode tool) const {
        auto t = allowed_tools();
        if (tool == t[0])
            return t[1];
        else
            return t[0];
    }
    enum region_ {
        rocky,
        wet,
        narrow
    };
    constexpr static size_t N = 3;
private:
    region_ x;
};

int main() {
    std::vector<size_t> geo_idx(size_t(dim) + 1);
    for (long x = 1; x <= dim.x; ++x)
        geo_idx[x] = x * 16807;
    for (long y = 1; y <= dim.y; ++y)
        geo_idx[coord{0, y}] = y * 48271;
    for (long y = 1; y <= dim.y; ++y)
        for (long x = 1; x <= dim.x; ++x)
            geo_idx[coord{x,y}] = erosion_level(geo_idx[coord{x-1,y}])
                * erosion_level(geo_idx[coord{x,y-1}]);
    geo_idx[target] = 0;

    std::vector<region> maze;
    std::transform(geo_idx.begin(), geo_idx.end(), std::back_inserter(maze),
                   [](size_t gi) -> region {
                       return {erosion_level(gi) % 3};
                   });

    // Part 1

    std::cout << "Total risk level: "
              << std::accumulate(maze.begin(), maze.end(), 0ul)
              << "\n\n";

    // Part 2

    std::vector<size_t> dist[mode::N];
    for (auto & d : dist)
        d.resize(maze.size(), std::numeric_limits<size_t>::max());

    std::multimap<coord, std::pair<mode, size_t>> todo;
    todo.insert({coord{0, 0}, {mode::torch, 0}});

    while (!todo.empty()) {
        auto top = todo.begin();
        auto [c, p] = *top;
        auto [m, d] = p;

        todo.erase(top);

        if (!c || !maze[c].can_enter(m))
            continue;

        if (dist[m][c] > d) {
            dist[m][c] = d;
            todo.insert({c, {maze[c].other_tool(m), d + 7}});
            for (coord cc : c.neighbors()) {
                todo.insert({cc, {m, d + 1}});
            }
        }
    }

    std::cout << "Fastest way to reach the target: "
              << dist[mode::torch][target] << "\n";
}
