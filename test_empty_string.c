#include <stdio.h>
#include <string.h>
int main() {
    char buffer[256];
    strncpy(buffer, "", 255);
    buffer[255] = '\0';
    printf("len: %lu\n", strlen(buffer));
    return 0;
}
