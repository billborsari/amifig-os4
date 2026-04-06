sed -i 's/if ((Lpixel \& 0xFF000000) != 0)/printf("DRAWING ANGLE x_rel=%d y_rel=%d\\n", x_rel, y_rel);\n\t\t\t\t\t\t\tif ((Lpixel \& 0xFF000000) != 0)/g' u_fonts.c
