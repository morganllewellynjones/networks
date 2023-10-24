#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

char* concat(char* string1, char* string2);
bool cacheFile(char* filename, int fd);
bool readToSocket(char* filename, int socketfd);

#endif
