#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

enum Crop {
	NOCROP = 0,
	CROPBEGIN = 1,
	CROPEND = 3,
	CROPBOTH = 7,
};

char* concat_reentrant(char* string1, char* string2);
bool cacheFile(char* filename, int fd);
ssize_t readToBuffer(char* filename, char* buffer, size_t buffsize);
size_t writeToSocket(char* filename, int socketfd);
char* strsplit(char* string, char* delim, size_t* distance);
char* strreplace(char* string, char* find, char* replace);
char* strextract(char* string, char* begin, char* end, enum Crop crop);

#endif
