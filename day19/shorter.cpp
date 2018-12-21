#include <iostream>

int main() {
    // Part 1:
    int reg[] = {0, 1, 0, 1, 875, 39};
    // Part 2:
    // int reg[] = {0, 1, 0, 1, 10551275, 10550400};
    while (true) {
        if(reg[1] * reg[3] == reg[4]) {
            reg[0] += reg[1];
        }

        ++reg[3];
        if (reg[3] > reg[4]) {
            ++reg[1];

            if (reg[5] = (reg[1] > reg[4])) {
                std::cout << reg[0] << ", " << reg[1] << ", " << reg[2] << ", "
                          << reg[3] << ", " << reg[4] << ", " << reg[5] << "\n";
                return 0;
            } else {
                reg[3] = 1;
            }
        }
    }
}
