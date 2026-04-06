sed -i 's/if (ret)/printf("d_text GetStringFromUser returned %d, buffer is %s\\n", ret, buffer);\n\tif (ret)/g' d_text.c
