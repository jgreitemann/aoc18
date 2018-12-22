#include <iostream>
#include <limits>

int main(int argc, char **argv) {
    auto exec = [] (size_t reg0, size_t N) {
        size_t reg[] = {reg0, 0, 123, 0, 0, 0};
        size_t n_instr = 1;

        do {
            reg[2] &= 456;
            n_instr += 4;
        } while (reg[2] != 72);
        n_instr--;

        reg[5] = 65536;
        reg[2] = 5234604;
        n_instr += 3;

        while (n_instr < N) {
            reg[3] = reg[5] & 255;
            reg[2] += reg[3];
            reg[2] &= 16777215;
            reg[2] *= 65899;
            reg[2] &= 16777215;
            n_instr += 5;

            if (reg[5] < 256) {
                n_instr += 5;
                if (reg[2] == reg[0]) {
                    return n_instr;
                }
                n_instr += 3;
                reg[5] = reg[2] | 65536;
                reg[2] = 5234604;
                continue;
            }

            reg[5] /= 256;
            n_instr += (reg[5] + 1) * 7 + 1 + 3;
        }
        return std::numeric_limits<size_t>::max();
    };

    // Part 1
    size_t min = 1000000000;
    for (size_t r = 0; r < 100000000; ++r) {
        size_t res = exec(r, min);
        if (res < min) {
            std::cout << r << '\t' << res << '\n';
            min = res;
        }
    }

    // Part 2
    size_t max = 100000000;
    for (size_t r = 0; r < 1000000; ++r) {
        size_t res = exec(r, 100 * max);
        if (res != std::numeric_limits<size_t>::max()) {
            std::cout << r << '\t' << res << '\n';
            if (res > max)
                max = res;
        }
    }
}
