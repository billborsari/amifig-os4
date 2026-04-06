#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_ext(char* filename)
{
	static char extension[32];
	int position=strlen((char *)filename)-1;

	strcpy(extension,"");
	
	while(position > -1 && filename[position] != '.') position--;

	if (position > -1)
	{
		strncpy(extension,(const char *)&filename[position+1],32);
	}

	return (char*)extension;
}

int main() {
    char *ext = get_ext("test.fig");
    printf("Ext: %s\n", ext);
    printf("strnicmp result: %d\n", strnicmp(ext, "fig", 3));
    return 0;
}
