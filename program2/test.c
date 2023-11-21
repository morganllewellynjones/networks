#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "io.h"

char* tagFileById(char* prefix, char id, char* suffix)
{
	char* fileName = malloc(strlen(prefix) + 1 + strlen(suffix) + 1);
	strcat(fileName, prefix);
	strcat(fileName, &id);
	strcat(fileName, suffix);
	return fileName;
}

int main()
{

	char outfile[] = "out_router_#.txt";
	*strchr(outfile, '#') = '1';
	printf("%s\n", outfile);

	return 0;
}
