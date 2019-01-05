#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "../functional.hpp"

struct coord {
    int x, y;
    size_t w;

    coord() : x{}, y{}, w{} {}
    coord(int x, int y, size_t w) : x(x), y(y), w(w) {}
    coord(size_t i, size_t w) : x(i % w), y(i / w), w(w) {}

    operator size_t() const {
        return y * w + x;
    }

    coord up() const { return {x, y - 1, w}; }
    coord down() const { return {x, y + 1, w}; }
    coord left() const { return {x - 1, y, w}; }
    coord right() const { return {x + 1, y, w}; }
};

std::ostream & operator<<(std::ostream & os, coord const& c) {
    return os << '{' << c.x << ',' << c.y << '}';
}

struct player {
    char type;
    int health;
};

std::ostream & operator<<(std::ostream & os, player const& p) {
    return os << p.type << '(' << p.health << ')';
}

int main() {
    auto str = [] () -> std::string {
                   std::ifstream is("input");
                   return {std::istreambuf_iterator<char>{is},
                           std::istreambuf_iterator<char>{}};
               }();
    size_t width = std::find(str.begin(), str.end(), '\n') - str.begin();
    std::vector<char> board;
    using namespace std::placeholders;
    std::copy_if(str.begin(), str.end(), std::back_inserter(board),
                 std::bind(std::not_equal_to<>{}, _1, '\n'));

    std::map<coord, player> players;
    for (size_t i = 0; i < board.size(); ++i) {
        char & c = board[i];
        if (c == 'E' || c == 'G') {
            players[{i, width}] = {c, 200};
            c = '.';
        }
    }

    auto total_hp = [](std::map<coord, player> const& players) {
        return std::accumulate(players.begin(), players.end(), 0,
                               [] (int acc, auto p) { return acc + p.second.health; });
    };
    auto elf_count = [](std::map<coord, player> const& players) {
        return std::count_if(players.begin(), players.end(),
                             [](auto p) { return p.second.type == 'E'; });
    };

    auto play_game = [&] (std::map<coord, player> players, int elf_attack = 3) {
        auto game_over = [&] {
            char t = players.begin()->second.type;
            return std::all_of(std::next(players.begin()), players.end(),
                               [t] (auto p) { return p.second.type == t; });
        };
        auto print_health = [&] {
            std::transform(players.begin(), players.end(),
                           std::ostream_iterator<player>{std::cout, ", "},
                           [] (auto p) { return p.second; });
            std::cout << '\n';
        };
        auto print_board = Y{overload{
                [&] (auto, std::vector<char> const& board) {
                    auto board_it = board.begin();
                    for (; board_it != board.end(); board_it += width) {
                        std::copy_n(board_it, width,
                                    std::ostream_iterator<char>{std::cout});
                        std::cout << '\n';
                    }
                    std::cout << '\n';
                },
                [&] (auto self) {
                    self(board);
                }
            }};
        auto print_board_with_players = [&] {
            std::vector<char> b(board);
            for (auto const& [c, p] : players)
                b[c] = p.type;
            print_board(b);
        };
        auto print_prox = [&](std::vector<size_t> const& prox) {
            auto prox_it = prox.begin();
            for (; prox_it != prox.end(); prox_it += width) {
                std::for_each(prox_it, prox_it + width,
                              [] (size_t p) {
                                  if (p == std::numeric_limits<size_t>::max())
                                      std::cout << 'M';
                                  else if (p > 9)
                                      std::cout << 'm';
                                  else
                                      std::cout << p;
                              });
                std::cout << '\n';
            }
            std::cout << '\n';
        };

        // print_board();
        // print_board_with_players();

        size_t r, last_round = 0;
        for (r = 0; !game_over(); ++r) {
            // std::cout << "Round " << r + 1 << " *ding* *ding*\n\n";

            std::map<coord, player> cplayers(players);
            for (auto const& [c, p] : cplayers) {
                if (game_over()) {
                    last_round = r;
                }
                std::vector<size_t> prox(board.size(),
                                         std::numeric_limits<size_t>::max());
                Y update_prox{[&](auto u, coord c, size_t v = 0) -> void {
                        char x = board[c];
                        if (board[c] == '.'
                            && (!v || players.find(c) == players.end())
                            && prox[c] > v)
                        {
                            prox[c] = v;
                            u(c.up(), v + 1);
                            u(c.down(), v + 1);
                            u(c.left(), v + 1);
                            u(c.right(), v + 1);
                        }
                    }};

                update_prox(c);

                coord target;
                size_t target_prox = std::numeric_limits<size_t>::max();
                std::set<coord> attackable;
                for (auto const& [cc, pp] : players) {
                    if (p.type == pp.type)
                        continue;
                    auto check_target = [&](coord ccc) {
                        if (c == ccc)
                            attackable.insert(cc);
                        if (prox[ccc] < target_prox)
                            target_prox = prox[target = ccc];
                    };
                    check_target(cc.up());
                    check_target(cc.left());
                    check_target(cc.right());
                    check_target(cc.down());
                }

                coord current = c;
                if (attackable.empty()
                    && target_prox != std::numeric_limits<size_t>::max())
                {
                    std::fill(prox.begin(), prox.end(),
                              std::numeric_limits<size_t>::max());
                    update_prox(target);

                    coord shortest;
                    size_t shortest_prox = std::numeric_limits<size_t>::max();

                    auto check_move = [&](coord c) {
                        if (prox[c] < shortest_prox)
                            shortest_prox = prox[shortest = c];
                    };
                    check_move(c.up());
                    check_move(c.left());
                    check_move(c.right());
                    check_move(c.down());

                    player pp = players[c];
                    players.erase(c);
                    players.insert({shortest, pp});
                    current = shortest;
                }

                attackable.clear();
                for (auto const& [cc, pp] : players) {
                    if (p.type == pp.type)
                        continue;
                    auto check_target = [&](coord ccc) {
                        if (current == ccc)
                            attackable.insert(cc);
                    };
                    check_target(cc.up());
                    check_target(cc.left());
                    check_target(cc.right());
                    check_target(cc.down());
                }

                if (!attackable.empty()) {
                    coord weakest_coord;
                    int weakest_hp = std::numeric_limits<int>::max();
                    for (coord a : attackable)
                        if (players[a].health < weakest_hp)
                            weakest_hp = players[weakest_coord = a].health;
                    players[weakest_coord].health -=
                        (p.type == 'E') ? elf_attack : 3;
                    // std::cout << current << " attacks " << weakest_coord << '\n';
                    if (players[weakest_coord].health <= 0) {
                        players.erase(weakest_coord);
                        cplayers.erase(weakest_coord);

                    }
                }
            }
            // print_board_with_players();
            // print_health();
            // std::cout << "-------------------------\n";
        }
        if (last_round == 0)
            last_round = r;
        return std::make_pair(last_round, players);
    };

    /* Part 1 */ {
        auto [last_round, remaining_players] = play_game(players);

        int rhp = total_hp(remaining_players);
        std::cout << "\nGAME OVER\n\nFinal round: " << last_round
                  << "\nRemaining HP on board: " << rhp
                  << "\nGame outcome: " << last_round * rhp
                  << "\n\n";
    }

    /**/ {
        size_t n_elf = elf_count(players);
        int d = 0, outcome;
        size_t last_round;
        auto remaining_players = players;
        do {
            ++d;
            std::tie(last_round, remaining_players) = play_game(players, d);
            outcome = last_round * total_hp(remaining_players);
        } while (elf_count(remaining_players) < n_elf);
        std::cout << "All elves survive at " << d << " attack strength.\n"
                  << "The game's outcome is " << outcome << ".\n";
    }
}
