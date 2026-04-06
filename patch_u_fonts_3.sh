sed -i 's/sourcex = (int)roundf(x_rel\*cosine + y_rel\*sine  );/sourcex = (int)roundf(x_rel*cosine + y_rel*sine  );\n\t\t\t\t\t\tsourcex = (sourcex >= 0) ? sourcex : 0;/g' u_fonts.c
sed -i 's/sourcey = (int)roundf(x_rel\*sine   - y_rel\*cosine) + (ascent+1);/sourcey = (int)roundf(x_rel*sine   - y_rel*cosine) + (ascent+1);\n\t\t\t\t\t\tsourcey = (sourcey >= 0) ? sourcey : 0;/g' u_fonts.c
