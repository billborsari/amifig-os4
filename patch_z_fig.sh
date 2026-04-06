sed -i 's/if ((char \*)temp_val != buffer) strncpy(buffer, (char \*)temp_val, maxlen-1);/\t\t\t\tstrncpy(buffer, (char *)temp_val, maxlen-1);/g' z_fig.c
sed -i 's/if ((char \*)temp_val != buffer)          ;/\t\t\t\tbuffer[maxlen-1] = '"'"'\\0'"'"';/g' z_fig.c
