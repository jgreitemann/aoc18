#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

enum class operation {
    addr,
    addi,
    mulr,
    muli,
    banr,
    bani,
    borr,
    bori,
    setr,
    seti,
    gtir,
    gtri,
    gtrr,
    eqir,
    eqri,
    eqrr
};

static const std::set<operation> all_ops = {
    operation::addr,
    operation::addi,
    operation::mulr,
    operation::muli,
    operation::banr,
    operation::bani,
    operation::borr,
    operation::bori,
    operation::setr,
    operation::seti,
    operation::gtir,
    operation::gtri,
    operation::gtrr,
    operation::eqir,
    operation::eqri,
    operation::eqrr
};

using registers = std::array<size_t, 4>;

struct instruction {
    operation op;
    size_t a, b, c;

    registers operator() (registers reg) const {
        switch(op) {
        case operation::addr:
            reg[c] = reg[a] + reg[b];
            return reg;
        case operation::addi:
            reg[c] = reg[a] + b;
            return reg;
        case operation::mulr:
            reg[c] = reg[a] * reg[b];
            return reg;
        case operation::muli:
            reg[c] = reg[a] * b;
            return reg;
        case operation::banr:
            reg[c] = reg[a] & reg[b];
            return reg;
        case operation::bani:
            reg[c] = reg[a] & b;
            return reg;
        case operation::borr:
            reg[c] = reg[a] | reg[b];
            return reg;
        case operation::bori:
            reg[c] = reg[a] | b;
            return reg;
        case operation::setr:
            reg[c] = reg[a];
            return reg;
        case operation::seti:
            reg[c] = a;
            return reg;
        case operation::gtir:
            reg[c] = static_cast<size_t>(a > reg[b]);
            return reg;
        case operation::gtri:
            reg[c] = static_cast<size_t>(reg[a] > b);
            return reg;
        case operation::gtrr:
            reg[c] = static_cast<size_t>(reg[a] > reg[b]);
            return reg;
        case operation::eqir:
            reg[c] = static_cast<size_t>(a == reg[b]);
            return reg;
        case operation::eqri:
            reg[c] = static_cast<size_t>(reg[a] == b);
            return reg;
        case operation::eqrr:
            reg[c] = static_cast<size_t>(reg[a] == reg[b]);
            return reg;
        default:
            return reg;
        }
    }
};

int main() {
    auto str = [] () -> std::string {
        std::ifstream is{"input"};
        return {std::istreambuf_iterator<char>{is},
                std::istreambuf_iterator<char>{}};
    }();

    std::regex re{"Before: \\[([0-9]), ([0-9]), ([0-9]), ([0-9])\\]\n"
                  "([0-9]+) ([0-9]) ([0-9]) ([0-9])\n"
                  "After:  \\[([0-9]), ([0-9]), ([0-9]), ([0-9])\\]"};

    std::map<size_t, std::set<operation>> compat;
    size_t three_or_more = 0;
    std::for_each(std::sregex_iterator{str.begin(), str.end(), re},
                  std::sregex_iterator{},
                  [&](std::smatch const& m) {
                      using std::stoul;
                      size_t n_compat = 0;
                      registers before = {stoul(m[1]), stoul(m[2]),
                                          stoul(m[3]), stoul(m[4])};
                      auto [opcode, a, b, c] = std::tuple{stoul(m[5]), stoul(m[6]),
                                                          stoul(m[7]), stoul(m[8])};
                      registers after = {stoul(m[9]), stoul(m[10]),
                                         stoul(m[11]), stoul(m[12])};
                      auto [it, _] = compat.insert({opcode, all_ops});
                      for (operation op : all_ops) {
                          instruction instr {op, a, b, c};
                          if (instr(before) != after)
                              it->second.erase(op);
                          else
                              ++n_compat;
                      }
                      if (n_compat >= 3)
                          ++three_or_more;
                  });

    // part 1
    std::cout << three_or_more << '\n';

    // ensure all opcodes are mapped to unique operation
    std::map<size_t, operation> lookup;
    while (!compat.empty()) {
        for (auto & [opcode, s] : compat) {
            if (s.size() == 1) {
                operation op = *s.begin();
                lookup.insert({opcode, op});
                compat.erase(opcode);
                for (auto & [_, ss] : compat) {
                    ss.erase(op);
                }
                break;
            }
        }
    }

    // find begin of the program
    auto prog_it = str.begin() + str.rfind("After");

    // part 2
    std::regex instr_re{"([0-9]+) ([0-9]) ([0-9]) ([0-9])\n"};
    auto final_reg = std::accumulate(
        std::sregex_iterator{prog_it, str.end(), instr_re},
        std::sregex_iterator{},
        registers{},
        [&] (registers reg, std::smatch const& m) {
            using std::stoul;
            instruction i{lookup[stoul(m[1])], stoul(m[2]), stoul(m[3]), stoul(m[4])};
            return i(reg);
        });
    std::cout << final_reg[0] << '\n';
}
