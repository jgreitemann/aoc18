#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include "../functional.hpp"

struct timestamp {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;

private:
    template <size_t... I>
    static timestamp from_regex_match_impl(std::smatch const& match,
                                           std::index_sequence<I...>)
    {
        return {static_cast<unsigned int>(std::stoul(match[I + 1].str()))...};
    }

public:
    static timestamp from_regex_match(std::smatch const& match) {
        using Indices = std::make_index_sequence<5>;
        return from_regex_match_impl(match, Indices{});
    }

    friend bool operator<(timestamp const& lhs, timestamp const& rhs) {
        return std::lexicographical_compare(&lhs.year, &lhs.year + 5,
                                            &rhs.year, &rhs.year + 5);
    }

    friend bool operator==(timestamp const& lhs, timestamp const& rhs) {
        return std::equal(&lhs.year, &lhs.year + 5, &rhs.year);
    }

    friend bool operator!=(timestamp const& lhs, timestamp const& rhs) {
        return !(lhs == rhs);
    }
};

struct wakes_up {};
struct falls_asleep {};
struct begins_shift {
    size_t guard_id;
};

using message = std::variant<wakes_up, falls_asleep, begins_shift>;

int main() {
    // read file into memory
    auto str = [] () -> std::string {
                   std::ifstream is("input");
                   return {std::istreambuf_iterator<char>{is},
                           std::istreambuf_iterator<char>{}};
               }();

    // parse log; std::map sorts entries chronologically
    std::map<timestamp, message> log;
    auto parse = [&] (std::string msg_regex, auto && get_msg) {
                     std::regex r("\\[([0-9]{4})-([0-9]{2})-([0-9]{2}) "
                                  "([0-9]{2}):([0-9]{2})\\] "
                                  + msg_regex);
                     std::transform(std::sregex_iterator{str.begin(), str.end(), r},
                                    std::sregex_iterator{},
                                    std::inserter(log, log.end()),
                                    [&] (std::smatch const& match)
                                        -> typename decltype(log)::value_type
                                    {
                                        return {timestamp::from_regex_match(match),
                                                get_msg(match, 6)};
                                    });
                 };
    parse("Guard #([0-9]+) begins shift",
          [] (std::smatch const& match, size_t offset) -> message {
              return begins_shift{std::stoul(match[offset])};
          });
    parse("falls asleep", [] (auto, auto) -> message { return falls_asleep{}; });
    parse("wakes up", [] (auto, auto) -> message { return wakes_up{}; });

    // accumulate minutes asleep
    std::map<size_t, std::array<int, 60>> guards;
    auto current_guard_it = guards.end();
    int current_sleep_begin;
    for (auto const& [ts, msg] : log) {
        std::visit(overload{
                [&] (begins_shift bs) {
                    current_guard_it = guards.insert({bs.guard_id, {}}).first;
                },
                [&] (falls_asleep a) {
                    current_sleep_begin = ts.minute;
                },
                [&] (wakes_up a) {
                    std::for_each(current_guard_it->second.begin() + current_sleep_begin,
                                  current_guard_it->second.begin() + ts.minute,
                                  [] (int & i) { ++i; });
                },
            }, msg);
    }

    // Strategy 1
    {
        std::cout << "Strategy 1:\n";
        std::map<int, size_t> total_asleep;
        std::transform(guards.begin(), guards.end(),
                       std::inserter(total_asleep, total_asleep.end()),
                       [] (auto const& g) -> std::pair<int, size_t> {
                           return {
                               std::accumulate(g.second.begin(),
                                               g.second.end(), 0),
                               g.first
                           };
                       });

        auto [total, id] = *(--total_asleep.end());
        auto const& minute = guards[id];
        size_t max_min = std::max_element(minute.begin(), minute.end())
            - minute.begin();
        std::cout << "#" << id << ":\t";
        std::copy(minute.begin(), minute.end(),
                  std::ostream_iterator<int>{std::cout});
        std::cout << " (max minute: " << max_min << ")\n";
        std::cout << "answer: " << id * max_min << std::endl;
    }

    // Strategy 2
    {
        std::cout << "\n\nStrategy 2:\n";
        std::map<int, size_t> most_asleep;
        std::transform(guards.begin(), guards.end(),
                       std::inserter(most_asleep, most_asleep.end()),
                       [] (auto const& g) -> std::pair<int, size_t> {
                           return {
                               *std::max_element(g.second.begin(), g.second.end()),
                               g.first
                           };
                       });
        size_t id = (--most_asleep.end())->second;
        auto const& minute = guards[id];
        size_t max_min = std::max_element(minute.begin(), minute.end())
            - minute.begin();
        std::cout << "#" << id << ": most asleep in minute " << max_min << "\n";
        std::cout << "answer: " << id * max_min << std::endl;
    }
}
