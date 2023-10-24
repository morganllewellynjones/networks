#include "io.h"

//Inspired by https://stackoverflow.com/questions/29616543/c-function-to-return-concatenated-string
//Because I wanted a non-destructive strcat function
char* concat(char* string1, char* string2)
{
	char* newString = (char*)malloc(strlen(string1) + strlen(string2) + 1);
	newString[0] = 0;
	strcat(newString, string1);
	strcat(newString, string2);
	return newString;
}

bool cacheFile(char* filename, int fd)
{
	int cachefd = open(filename, O_WRONLY | O_CREAT, 0666);
	if (cachefd < 0) {
		perror("(failed to open cache)");
		return false;
	}

	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	size_t bytes = 0;
	while ((bytes = read(fd, buffer, sizeof(buffer)-1)) > 0)
	{
		write(cachefd, buffer, bytes);
		memset(buffer, 0, bytes);
	}
	close(cachefd);
	return true;
}

size_t writeToSocket(char* filename, int socketfd)
{
	int fd = open(filename, 0);
	if (fd < 0) {
		perror("{failed to open disk file)");
		exit(EXIT_FAILURE);
	}
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	size_t bytes = 0;
	size_t written = 0;
	size_t accum = 0;
	while ((bytes = read(fd, buffer, sizeof(buffer)-1)) > 0)
	{
		if ((written = write(socketfd, buffer, bytes)) < 0) {
			perror("(writing to socket)");
		}
		memset(buffer, 0, bytes);
		accum += written;
	}
	return accum;
}
