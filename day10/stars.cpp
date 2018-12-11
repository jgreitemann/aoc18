#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <regex>
#include <string>
#include <vector>

struct pos {
    long x, y;

    friend pos operator-(pos p) {
        p.x = -p.x;
        p.y = -p.y;
        return p;
    }

    pos & operator+=(pos const& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    pos & operator-=(pos const& other) {
        return *this += -other;
    }
};

struct star {
    pos loc;
    pos vel;

    star & operator++() {
        loc += vel;
        return *this;
    }

    star & operator--() {
        loc -= vel;
        return *this;
    }
};

int main() {
    std::ifstream is{"input"};
    std::string str{std::istreambuf_iterator<char>{is},
                    std::istreambuf_iterator<char>{}};

    std::regex re{"position=< *(-?[0-9]+), *(-?[0-9]+)> "
                  "velocity=< *(-?[0-9]+), *(-?[0-9]+)>"};
    std::vector<star> stars;
    std::transform(std::sregex_iterator{str.begin(), str.end(), re},
                   std::sregex_iterator{},
                   std::back_inserter(stars),
                   [] (std::smatch const& match) -> star {
                       return {
                           {std::stoi(match[1]), std::stoi(match[2])},
                           {std::stoi(match[3]), std::stoi(match[4])}
                       };
                   });

    long min_area = std::numeric_limits<long>::max();
    size_t n_cooldown = 10;
    size_t cooldown;
    size_t min_area_time;
    auto get_bbox = [&stars] {
                        std::array<long, 4> bbox = {
                            std::numeric_limits<long>::max(),
                            std::numeric_limits<long>::min(),
                            std::numeric_limits<long>::max(),
                            std::numeric_limits<long>::min()
                        };
                        for (auto [l, _] : stars) {
                            bbox[0] = std::min(l.x, bbox[0]);
                            bbox[1] = std::max(l.x, bbox[1]);
                            bbox[2] = std::min(l.y, bbox[2]);
                            bbox[3] = std::max(l.y, bbox[3]);
                        }
                        return bbox;
                    };
    for (size_t t = 0;; ++t) {
        // get the bounding box
        auto bbox = get_bbox();
        long area = (bbox[1] - bbox[0] + 1) * (bbox[3] - bbox[2] + 1);

        if (area < min_area) {
            min_area = area;
            min_area_time = t;
            cooldown = 0;
        } else {
            ++cooldown;
        }

        if (cooldown >= n_cooldown)
            break;

        // move forward
        for (auto & s : stars)
            ++s;
    }

    for (size_t i = 0; i < 2 * n_cooldown; ++i) {
        // get the bounding box
        auto bbox = get_bbox();
        long area = (bbox[1] - bbox[0] + 1) * (bbox[3] - bbox[2] + 1);

        // print
        std::vector<bool> matrix(area, false);
        for (auto [l, _] : stars) {
            matrix[l.x - bbox[0] + (l.y - bbox[2]) * (bbox[1] + 1 - bbox[0])] = true;
        }
        auto m_it = matrix.begin();
        for (long y = bbox[2]; y <= bbox[3]; ++y) {
            for (long x = bbox[0]; x <= bbox[1]; ++x, ++m_it) {
                if (*m_it)
                    std::cout << '#';
                else
                    std::cout << '.';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        
        // move backward
        for (auto & s : stars)
            --s;
    }

    std::cout << "Needed to wait " << min_area_time << " seconds.\n";
}
