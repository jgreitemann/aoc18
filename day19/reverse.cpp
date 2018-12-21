#include <iostream>

int main() {
    int reg[] = {0, 0, 0, 0, 0, 0};
l0:
    // reg[2] += 16;
    goto l17;
l1:
    reg[1] = 1;
l2:
    reg[3] = 1;
l3:
    reg[5] = reg[1] * reg[3];
l4:
    if(reg[5] = (reg[5] == reg[4])) {
l7:
        reg[0] += reg[1];
    }

// l5:
//     reg[2] += reg[5];
// l6:
    // ++reg[2];
    // goto l8;
l8:
    ++reg[3];
l9:
    if (reg[3] > reg[4]) {
        goto l12;
    } else {
        goto l3;
    }
// l10:
//     reg[2] += reg[5];
// l11:
//     reg[2] = 2;
l12:
    ++reg[1];

l13:
    if (reg[5] = (reg[1] > reg[4])) {
// 14:
//     reg[2] += reg[5];
// 16:
//         reg[2] *= reg[2];
        std::cout << reg[0] << ", " << reg[1] << ", " << reg[2] << ", "
                  << reg[3] << ", " << reg[4] << ", " << reg[5] << "\n";
        return 0;
    } else {
// 15:
//     reg[2] = 1;
        goto l2;
    }
l17:
    reg[4] += 2;
l18:
    reg[4] *= reg[4];
l19:
    // reg[4] *= reg[2];
    reg[4] *= 19;
l20:
    reg[4] *= 11;
l21:
    ++reg[5];
l22:
    // reg[5] *= reg[2];
    reg[5] *= 22;
l23:
    reg[5] += 17;
l24:
    reg[4] += reg[5];
l25:
    // reg[2] += reg[0];
    if (reg[0]) {
// 26:
//         reg[2] = 0;
l27:
        // reg[5] = reg[2];
        reg[5] = 27;
l28:
        // reg[5] *= reg[2];
        reg[5] *= 28;
l29:
        // reg[5] += reg[2];
        reg[5] += 29;
l30:
        // reg[5] *= reg[2];
        reg[5] *= 30;
l31:
        reg[5] *= 14;
l32:
        // reg[5] *= reg[2];
        reg[5] *= 32;
l33:
        reg[4] += reg[5];
l34:
        reg[0] = 0;
    }
l35:
    // reg[2] = 0;
    goto l1;
}
