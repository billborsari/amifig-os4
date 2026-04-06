#include <stdio.h>
#include <math.h>

int main() {
    float angle = 45.0 * 3.14159 / 180.0;
    float cosine = cos(angle);
    float sine = sin(angle);
    
    int minx = 0;
    int maxx = 100;
    int miny = -50;
    int maxy = 50;
    
    int PMW = 100;
    int PMH = 20;
    int ascent = 15;
    
    int x_rel = 50;
    int y_rel = 0;
    
    int sourcex = (int)roundf(x_rel*cosine + y_rel*sine  );
    int sourcey = (int)roundf(x_rel*sine   - y_rel*cosine) + (ascent+1);
    
    printf("srcx: %d srcy: %d\n", sourcex, sourcey);
    return 0;
}
