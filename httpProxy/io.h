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

char* arrayToString(char** strings, char* delim, size_t strcount);
int wordCount(char* string, char* word);
char** stringToArray(char* string, char* delim);
char* strsplit(char* string, char* delim, size_t* distance);
char* strreplace(char* string, char* find, char* replace);
char* strextract(char* string, char* begin, char* end, enum Crop crop);

bool cacheFile(char* filename, int fd);
size_t writeToSocket(int sourcefd, int destfd, char* buffer, size_t buffsize);
ssize_t Read (int fd, char* buffer, size_t buffsize);
ssize_t Write (int fd, char* buffer, size_t buffsize);
int Open (char* pathname, int flags, mode_t mode);
int Close (int fd);
int Remove (char* pathname);
void* Malloc(size_t size);

#endif
