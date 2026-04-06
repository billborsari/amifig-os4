#include <stdio.h>
#include <stdlib.h>
void put_msg(char *format, ...) { printf("ERROR: %s\n", format); }
void file_msg(char *format, ...) { printf("FILE MSG: %s\n", format); }
void doMessage(char *format, ...) {}
// dummy out all
