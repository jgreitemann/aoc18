#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

struct cart {
    size_t x, y;
    size_t t;

    cart moved(char dir) const {
        switch (dir) {
        case '<':
            return {x-1, y, t};
        case '>':
            return {x+1, y, t};
        case 'v':
            return {x, y+1, t};
        case '^':
            return {x, y-1, t};
        default:
            return *this;
        }
    }

    friend bool operator<(cart const& lhs, cart const& rhs) {
        return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
    }
};


int main() {
    std::vector<char> track;
    std::ptrdiff_t width = -1;
    {
        std::ifstream is{"input"};
        std::copy_if(std::istreambuf_iterator<char>{is},
                     std::istreambuf_iterator<char>{},
                     std::back_inserter(track),
                     [&] (char c) {
                         if (width < 0) {
                             --width;
                         }
                         if (c == '\n') {
                             if (width < 0) {
                                 width = -width - 2;
                             }
                             return false;
                         }
                         return true;
                     });
    }

    std::map<cart, char> carts;
    for (size_t i = 0; i < track.size(); ++i) {
        char c = track[i];
        switch (c) {
        case '<':
        case '>':
            track[i] = '-';
            carts.insert({{i % width, i / width}, c});
            break;
        case 'v':
        case '^':
            track[i] = '|';
            carts.insert({{i % width, i / width}, c});
            break;
        default:
            break;
        }
    }

    auto orient = [] (char dir, char track, size_t & t) {
                      char left_turn[] = {'^', 'v', '<'};
                      char right_turn[] = {'v', '^', '>'};
                      char down_turn[] = {'<', '>', 'v'};
                      char up_turn[] = {'>', '<', '^'};
                      switch (track) {
                      case '+':
                          switch (dir) {
                          case '<':
                              return left_turn[t = (t+1)%3];
                          case '>':
                              return right_turn[t = (t+1)%3];
                          case 'v':
                              return down_turn[t = (t+1)%3];
                          case '^':
                              return up_turn[t = (t+1)%3];
                          default:
                              return '\0';
                          }
                      case '\\':
                          switch (dir) {
                          case '<':
                              return '^';
                          case '>':
                              return 'v';
                          case 'v':
                              return '>';
                          case '^':
                              return '<';
                          default:
                              return '\0';
                          }
                      case '/':
                          switch (dir) {
                          case '<':
                              return 'v';
                          case '>':
                              return '^';
                          case 'v':
                              return '<';
                          case '^':
                              return '>';
                          default:
                              return '\0';
                          }
                      default:
                          return dir;
                      }
                  };

    auto to_index = [width] (cart const& c) -> size_t {
                        return c.y * width + c.x;
                    };

    std::map<cart, char> after_tick;
    while (carts.size() > 1) {
        after_tick = carts;
        for (auto [c, d] : carts) {
            // std::cout << c.x << ", " << c.y << ": " << d << '\n';
            after_tick.erase(c);
            cart cc = c.moved(d);
            auto [it, nocoll] = after_tick.insert(
                {cc, orient(d, track[to_index(cc)], cc.t)});
            if (!nocoll) {
                after_tick.erase(cc);
                carts.erase(cc);
                std::cout << "collision: " << cc.x << ", " << cc.y
                          << " (remaining: " << after_tick.size() << ")"<< "\n";
            }
        }
        // std::cout << '\n';
        carts = std::move(after_tick);

        std::vector<char> stt(track.begin(), track.end());
        for (auto [cc, dd] : carts)
            stt[to_index(cc)] = dd;
        for (size_t i = 0; i < stt.size(); ++i) {
            if (i % width == 0)
                std::clog << '\n';
            std::clog << stt[i];
        }
        std::clog << '\n';
    }

    std::cout << "Remaining cart:\n";
    for (auto [c, _] : carts) {
        std::cout << c.x << ", " << c.y << std::endl;
    }
}
