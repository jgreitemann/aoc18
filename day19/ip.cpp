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

static const std::map<std::string, operation> opnames = {
    {"addr", operation::addr},
    {"addi", operation::addi},
    {"mulr", operation::mulr},
    {"muli", operation::muli},
    {"banr", operation::banr},
    {"bani", operation::bani},
    {"borr", operation::borr},
    {"bori", operation::bori},
    {"setr", operation::setr},
    {"seti", operation::seti},
    {"gtir", operation::gtir},
    {"gtri", operation::gtri},
    {"gtrr", operation::gtrr},
    {"eqir", operation::eqir},
    {"eqri", operation::eqri},
    {"eqrr", operation::eqrr}
};

using registers = std::array<size_t, 6>;

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

    // find the ip register
    size_t ipi = [&str] {
        std::regex re{"#ip ([0-9])"};
        std::smatch match;
        if (!std::regex_search(str, match, re))
            throw std::runtime_error("no ip defined");
        return std::stoul(match[1]);
    }();

    // parse program
    std::regex instr_re{"([a-z]+) ([0-9]+) ([0-9]+) ([0-9]+)\n"};
    std::vector<instruction> program;
    std::transform(std::sregex_iterator{str.begin(), str.end(), instr_re},
                   std::sregex_iterator{},
                   std::back_inserter(program),
                   [] (std::smatch const& m) {
                       using std::stoul;
                       return instruction{opnames.at(m[1].str()), stoul(m[2]),
                               stoul(m[3]), stoul(m[4])};
                   });

    auto execute_program = [&] (registers reg) {
        size_t & ip = reg[ipi];
        while (ip < program.size()) {
            if (reg[0] == 0 && reg[1] == 1 && reg[2] == 5
                && reg[3] == 3 && reg[4] == 10551275 && reg[5] == 0)
            {
                reg = {0, 1, 5, 10551273, 10551275, 0};
            }
            if (reg[0] == 1 && reg[2] == 5 && reg[3] == 3
                && reg[4] == 10551275 && reg[5] == 0)
            {
                reg = {1, reg[1], 5, 10551273, 10551275, 0};
            }
            reg = program[ip](reg);
            std::copy(reg.begin(), reg.end(),
                      std::ostream_iterator<size_t>{std::cout, ", "});
            std::cout << '\n';
            ++ip;
        }
        --ip;
        std::copy(reg.begin(), reg.end(),
                  std::ostream_iterator<size_t>{std::cout, ", "});
        std::cout << '\n';
    };

    // Part 1
    execute_program({});
    std::cout << "---------------------------------\n";
    // Part 2
    // execute_program({1});
}
