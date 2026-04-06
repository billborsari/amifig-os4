#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gfx.h>
#include <stdio.h>

int main() {
    struct BitMap *TempBM = AllocBitMap(100, 20, 32, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, NULL);
    if (!TempBM) {
        printf("AllocBitMap failed!\n");
    } else {
        printf("AllocBitMap success! depth=%d\n", GetBitMapAttr(TempBM, BMA_DEPTH));
        FreeBitMap(TempBM);
    }
    return 0;
}
