#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

enum Crop {
	NOCROP = 0,
	CROPBEGIN = 1,
	CROPEND = 3,
	CROPBOTH = 7,
};

char* arrayToString(char** strings, char* delim, size_t strcount);
int wordCount(char* string, char* word);
int countLines(char* str);
char** stringToArray(char* string, char* delim);
char* strsplit(char* string, char* delim, size_t* distance);
char* strreplace(char* string, char* find, char* replace);
char* strextract(char* string, char* begin, char* end, enum Crop crop);
size_t writeToSocket(int sourcefd, int destfd, char* buffer, size_t buffsize);
ssize_t Read (int fd, char* buffer, size_t buffsize);
ssize_t Write (int fd, char* buffer, size_t buffsize);
int Open (char* pathname, int flags, mode_t mode);
int Close (int fd);
int Socket(int domain, int type, int protocol);
int Bind(int fd, struct sockaddr* hostaddr, size_t addrlen);
int Listen(int fd, int backlog);
int Connect(int fd, struct sockaddr* addr, size_t addrlen);
int Remove (char* pathname);
void* Malloc(size_t size);

#endif
