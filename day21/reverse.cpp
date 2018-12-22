int main() {
    int reg[] = {0, 0, 0, 0, 0, 0};
l0:
    reg[2] = 123;
l1:
    reg[2] &= 456;
l2:
    reg[2] = (reg[2] == 72);
l3:
    reg[4] += reg[2];
l4:
    reg[4] = 0;
l5:
    reg[2] = 0;
l6:
    reg[5] = reg[2] | 65536;
l7:
    reg[2] = 5234604;
l8:
    reg[3] = reg[5] & 255;
l9:
    reg[2] += reg[3];
l10:
    reg[2] &= 16777215;
l11:
    reg[2] *= 65899;
l12:
    reg[2] &= 16777215;
l13:
    reg[3] = (256 > reg[5]);
l14:
    reg[4] += reg[3];
l15:
    ++reg[4];
l16:
    reg[4] = 27;
l17:
    reg[3] = 0;
l18:
    reg[1] = reg[3] + 1;
l19:
    reg[1] *= 256;
l20:
    reg[1] = (reg[1] > reg[5]);
l21:
    reg[4] += reg[1];
l22:
    ++reg[4];
l23:
    reg[4] = 25;
l24:
    ++reg[3];
l25:
    reg[4] = 17;
l26:
    reg[5] = reg[3];
l27:
    reg[4] = 7;
l28:
    reg[3] = (reg[2] == reg[0]);
l29:
    reg[4] += reg[3];
l30:
    reg[4] = 5;
}
