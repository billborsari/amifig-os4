#include <proto/exec.h>
#include <proto/intuition.h>
#include <intuition/pointerclass.h>
#include <stdio.h>

int main(void) {
    if (!IntuitionBase) return 0;
    
    // Test creating a pointer with WordWidth=2 and one with WordWidth=1
    printf("POINTERA_XOffset is %08x\n", POINTERA_XOffset);
    return 0;
}
