sed -i 's/refresh_text_area(cur_t, FALSE);/refresh_text_area(cur_t, FALSE);\n\t\t\tredisplay_canvas();/g' d_text.c
