#include <stdio.h>
#include <string.h>

int main() {
    char buffer[256];
    char* temp_val = "test input";
    if (temp_val != buffer) {
        strncpy(buffer, temp_val, 255);
        buffer[255] = '\0';
    }
    printf("buffer: %s\n", buffer);
    return 0;
}
