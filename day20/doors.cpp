#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>
#include "../functional.hpp"

struct room {
    int distance = -1;
    bool east_door = false;
    bool south_door = false;
};

struct sequence;

struct group : std::vector<sequence> {
    using std::vector<sequence>::vector;
};

using token = std::variant<std::string_view, group>;

struct sequence : std::vector<token> {
    using std::vector<token>::vector;
};

struct iterator_element {
    typename group::const_iterator g;
    typename group::const_iterator g_end;
    typename sequence::const_iterator s;
    typename sequence::const_iterator s_end;

    bool exhausted() const { return g == g_end; }
    bool end() const { return s == s_end; }
};

using iterator = std::stack<iterator_element>;

int main() {
    auto regex = []() -> std::string {
                     std::ifstream is{"input"};
                     return {std::istream_iterator<char>{is},
                             std::istream_iterator<char>{}};
                 }();
    // regex = "^WSSEESWWWNW(S|NENNEEEENN(ESSSSW(NWSW|SSEN)|WSWWN(E|WWS(E|SS))))$";
    // regex = "^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$";
    // regex = "^N(W|E)N$"; // throws

    std::cout << "Regex: " << regex << "\n";

    auto tokenize = Y{[](auto self, std::string_view re) -> sequence {
        size_t i = 0;
        size_t group_level = 0;
        sequence seq;
        group gr;
        for (size_t j = 0; j < re.size(); ++j) {
            switch (re[j]) {
            case '(':
                if (group_level == 0) {
                    seq.push_back(re.substr(i, j - i));
                    i = j + 1;
                }
                ++group_level;
                break;
            case '|':
                if (group_level == 1) {
                    gr.push_back(self(re.substr(i, j - i)));
                    i = j + 1;
                }
                break;
            case ')':
                if (group_level == 1) {
                    gr.push_back(self(re.substr(i, j - i)));
                    i = j + 1;
                    seq.push_back(std::move(gr));
                    gr = {};
                }
                --group_level;
                break;
            default:
                break;
            }
        }
        if (i < re.size())
            seq.push_back(re.substr(i, std::string_view::npos));
        return seq;
    }};

    std::string_view full_view{regex};
    full_view.remove_prefix(1);
    full_view.remove_suffix(1);
    group regex_tree{tokenize(full_view)};

    std::deque<std::deque<room>> rooms = {{{0}}};
    std::pair<size_t, size_t> dim = {1, 1};
    std::pair<int, int> offset = {0, 0};
    auto get_room = [&](int x, int y) -> room& {
                        return rooms[y + offset.second][x + offset.first];
                    };
    auto expand = [&](char dir) {
                      switch (dir) {
                      case 'E':
                          for (auto & row : rooms)
                              row.emplace_back();
                          ++(dim.first);
                          break;
                      case 'S':
                          rooms.emplace_back(dim.first);
                          ++(dim.second);
                          break;
                      case 'W':
                          for (auto & row : rooms)
                              row.emplace_front();
                          ++(offset.first);
                          ++(dim.first);
                          break;
                      case 'N':
                          rooms.emplace_front(dim.first);
                          ++(offset.second);
                          ++(dim.second);
                          break;
                      default:
                          throw std::invalid_argument("unknown dir");
                      }
                  };
    auto move = [&](char dir, int & x, int & y) {
                    switch (dir) {
                    case 'E':
                        get_room(x, y).east_door = true;
                        if (++x + offset.first == dim.first)
                            expand(dir);
                        break;
                    case 'S':
                        get_room(x, y).south_door = true;
                        if (++y + offset.second == dim.second)
                            expand(dir);
                        break;
                    case 'W':
                        if (x + offset.first == 0)
                            expand(dir);
                        --x;
                        get_room(x, y).east_door = true;
                        break;
                    case 'N':
                        if (y + offset.second == 0)
                            expand(dir);
                        --y;
                        get_room(x, y).south_door = true;
                        break;
                    default:
                        throw std::invalid_argument("unknown dir");
                    }
                };

    size_t i = 0;
    auto traverse = Y{[&](auto self, iterator it, int x, int y, int d)
                      -> std::tuple<int, int, int>
    {
        bool inequiv = false;
        while (it.top().s != it.top().s_end) {
            if (inequiv)
                throw std::runtime_error("inequivalent branches");
            std::visit(overload{
                    [&](std::string_view sv) -> void {
                        for (char c : sv) {
                            move(c, x, y);
                            ++d;
                            get_room(x, y).distance = get_room(x, y).distance == -1
                                ? d : std::min(get_room(x, y).distance, d);
                        }
                    },
                    [&](group const& gr) -> void {
                        it.push({gr.begin(), gr.end(),
                                 gr.front().begin(), gr.front().end()});
                        int xx = -1, yy = -1, dd = -1;
                        for (; it.top().g != it.top().g_end; ++it.top().g) {
                            it.top().s = it.top().g->begin();
                            it.top().s_end = it.top().g->end();

                            int xxx, yyy, ddd;
                            std::tie(xxx, yyy, ddd) = self(it, x, y, d);
                            if (dd >= 0) {
                                if (xx != xxx || yy != yyy)
                                    inequiv = true;
                                dd = std::min(dd, ddd);
                            } else {
                                xx = xxx;
                                yy = yyy;
                                dd = ddd;
                            }
                        }
                        it.pop();
                        x = xx;
                        y = yy;
                        d = dd;
                    }}, *it.top().s);

            // next in sequence
            ++it.top().s;
        }
        return std::make_tuple(x, y, d);
    }};

    iterator it;
    it.push({regex_tree.cbegin(), regex_tree.cend(),
             regex_tree.front().cbegin(), regex_tree.front().cend()});
    traverse(it, 0, 0, 0);

    // find furthest room
    int max_d = 0;
    for (auto const& row : rooms)
        for (auto const& r : row)
            max_d = std::max(max_d, r.distance);
    std::cout << "Furthest room requires passing " << max_d << " doors\n\n";

    // find number of rooms at least 1000 doors away
    std::cout << std::accumulate(rooms.begin(), rooms.end(), 0,
            [](int acc, auto const& row) {
                return acc + std::accumulate(row.begin(), row.end(), 0,
                                             [](int acc, auto const& r) {
                                                 return acc + (r.distance >= 1000);
                                             });
            })
              << " rooms are at least 1000 doors away.\n";

    // draw the map
    const size_t width = dim.first;
    const size_t height = dim.second;
    std::string map((2*width+2) * (2*height+1), '#');
    auto mit = map.begin();
    for (auto const& row : rooms) {
        mit += 2*width+1;
        *mit = '\n';
        mit += 2;
        for (auto const& r : row) {
            *mit = '.';
            if (r.east_door)
                mit[1] = '|';
            if (r.south_door)
                mit[2*width+2] = '-';
            mit += 2;
        }
        *mit = '\n';
        ++mit;
    }
    mit += 2*width+1;
    *mit = '\n';
    std::cout << map << "\n";

    // print distances
    for (auto const& row : rooms) {
        for (auto const& r : row) {
            std::cout << r.distance << '\t';
        }
        std::cout << '\n';
    }
}
