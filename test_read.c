#include <stdio.h>
#include <stdlib.h>
#include "fig.h"
#include "object.h"

extern F_compound fobjects;
extern int readfp_fig(FILE *fp, F_compound *obj, Boolean merge, int xoff, int yoff);

int main(int argc, char **argv) {
    if(argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    int status = readfp_fig(fp, &fobjects, False, 0, 0);
    printf("Status: %d\n", status);
    fclose(fp);
    return 0;
}
