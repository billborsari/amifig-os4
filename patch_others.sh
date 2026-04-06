sed -i 's/get(STR_PICT_FILENAME, MUIA_String_Contents, &newpic);/{ IPTR _t; get(STR_PICT_FILENAME, MUIA_String_Contents, \&_t); newpic = (STRPTR)_t; }/g' z_pict.c
sed -i 's/get(STR_PRI_LEFT, MUIA_String_Contents, &tmp);/{ IPTR _t; get(STR_PRI_LEFT, MUIA_String_Contents, \&_t); tmp = (STRPTR)_t; }/g' z_print.c
sed -i 's/get(STR_PRI_WDTH, MUIA_String_Contents, &tmp);/{ IPTR _t; get(STR_PRI_WDTH, MUIA_String_Contents, \&_t); tmp = (STRPTR)_t; }/g' z_print.c
sed -i 's/get(STR_PRI_TOP,  MUIA_String_Contents, &tmp);/{ IPTR _t; get(STR_PRI_TOP,  MUIA_String_Contents, \&_t); tmp = (STRPTR)_t; }/g' z_print.c
sed -i 's/get(STR_PRI_HGHT, MUIA_String_Contents, &tmp);/{ IPTR _t; get(STR_PRI_HGHT, MUIA_String_Contents, \&_t); tmp = (STRPTR)_t; }/g' z_print.c
sed -i 's/get(STR_PRI_BUFFER, MUIA_String_Contents, &tmp);/{ IPTR _t; get(STR_PRI_BUFFER, MUIA_String_Contents, \&_t); tmp = (STRPTR)_t; }/g' z_print.c
