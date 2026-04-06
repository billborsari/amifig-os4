sed -i 's/sourcex = (sourcex >= 0) ? sourcex : 0;//g' u_fonts.c
sed -i 's/sourcey = (sourcey >= 0) ? sourcey : 0;//g' u_fonts.c
sed -i 's/if( sourcex >= 0 && sourcex < PMW && sourcey >= 0 && sourcey < PMH )/if( sourcex >= 0 \&\& sourcex < PMW \&\& sourcey >= 0 \&\& sourcey < PMH )\n\t\t\t\t\t\t\tprintf("Drawing pixmap at X=%d %d\\n", sourcex, sourcey);/g' u_fonts.c
