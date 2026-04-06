#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char cmd[2048];
    if (argc < 2) {
        fprintf(stderr, "Usage: ppmquant <ncolors> [file]\n");
        return 1;
    }
    const char *infile = (argc > 2) ? argv[2] : "-";
    
    // Step 1: Generate colormap
    snprintf(cmd, sizeof(cmd), "pnmcolormap %s %s > T:netpbm_colormap.tmp", argv[1], infile);
    if (system(cmd) != 0) return 1;
    
    // Step 2: Remap
    snprintf(cmd, sizeof(cmd), "pnmremap -mapfile=T:netpbm_colormap.tmp %s", infile);
    if (system(cmd) != 0) return 1;
    
    return 0;
}
