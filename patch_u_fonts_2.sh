sed -i 's/if (strcmp(string," ")!=0)/if (strcmp(string," ")!=0) printf("pw_text len: %d asc: %d des: %d str: %s\\n", length, ascent, descent, string);/g' u_fonts.c
