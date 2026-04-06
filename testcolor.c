#include <stdio.h>
int main() {
    unsigned int rgb = 0x00FF0000;
    unsigned int r = ((rgb & 0x00FF0000) >> 16) * 0x01010101;
    unsigned int g = ((rgb & 0x0000FF00) >>  8) * 0x01010101;
    unsigned int b = ((rgb & 0x000000FF)      ) * 0x01010101;
    printf("r: %08X, g: %08X, b: %08X\n", r, g, b);
    return 0;
}
