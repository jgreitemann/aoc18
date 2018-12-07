#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

struct point {
    int x, y;
};

std::istream & operator>>(std::istream & is, point & p) {
    char comma;
    is >> p.x >> comma >> p.y;
    if (comma != ',')
        is.setstate(std::ios::failbit);
    return is;
}

int manhattan(point a, point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int main() {
    auto pts = [] () -> std::vector<point> {
        std::ifstream is("input");
        return {std::istream_iterator<point>{is},
                std::istream_iterator<point>{}};
    }();

    int max_x = std::max_element(pts.begin(), pts.end(),
                                 [] (point l, point r) { return l.x < r.x; })->x;
    int max_y = std::max_element(pts.begin(), pts.end(),
                                 [] (point l, point r) { return l.y < r.y; })->y;
    size_t none = std::numeric_limits<size_t>::max();
    std::vector<size_t> map((max_x + 1) * (max_y + 1));
    point p;
    auto m_it = map.begin();
    for (p.y = 0; p.y <= max_y; ++p.y) {
        for (p.x = 0; p.x <= max_x; ++p.x, ++m_it) {
            size_t closest = none;
            int closest_dist = std::numeric_limits<int>::max();
            for (size_t k = 0; k < pts.size(); ++k) {
                int d = manhattan(p, pts[k]);
                if (d < closest_dist) {
                    closest = k;
                    closest_dist = d;
                } else if (d == closest_dist) {
                    closest = none;
                }
            }
            *m_it = closest;
        }
    }

    std::vector<size_t> counts(pts.size());
    for (size_t k : map) {
        if (k != none)
            counts[k]++;
    }

    auto reset = [&] (size_t k) {
                     if (k != none) {
                         counts[k] = 0;
                     }
                 };
    for (int x = 0; x <= max_x; ++x) {
        reset(map[x]);
        reset(map[max_y * (max_x + 1) + x]);
    }
    for (int y = 0; y <= max_y; ++y) {
        reset(map[y * (max_x + 1)]);
        reset(map[y * (max_x + 1) + max_x]);
    }

    auto max_it = std::max_element(counts.begin(), counts.end());
    std::cout << "max index: " << max_it - counts.begin()
              << "\nmax count: " << *max_it
              << std::endl;

    size_t region = 0;
    int thres = 10000;
    for (p.y = -thres; p.y <= max_y + thres; ++p.y) {
        for (p.x = -thres; p.x <= max_x + thres; ++p.x) {
            int total = 0;
            for (point q : pts)
                total += manhattan(p, q);
            if (total < thres)
                region++;
        }
    }
    std::cout << "\nregion size: " << region << std::endl;
}
