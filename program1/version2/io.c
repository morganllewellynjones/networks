#include "io.h"

char* concat_reentrant(char* str1, char* str2)
{
	size_t joinsize = strlen(str1) + strlen(str2) + 1;
	char* new = (char*)malloc(joinsize);
	new = strcat(new, str1);
	new = strcat(new, str2);
	new[joinsize - 1] = 0;
	return new;
}

char* strsplit(char* string, char* delim, size_t* distance)
{
	/*Searches for a delimiter and returns a substring of the input up to that delimiter
	 * Function does not modify original string, it returns a duplicate, which needs to be cleaned up
	 * Distance is passed in as a save point for future string operations to know where the first delimiter was found
	 * If distance is returned as 0 then no delimiter was found */
	char* p = string;
	for (p = string; *p != '\0'; ++p)
	{
		if (*p == *delim && (strncmp(p, delim, strlen(delim)) == 0))
		{
			*distance = p - string;
			return strndup(string, p - string);
		}
	}
	/*If no delimiter found, just return a copy of the original string*/
	return strdup(string);
}

char* strreplace(char* string, char* find, char* replace)
{
	size_t distance = 0;
	char* beginRegion = strsplit(string, find, &distance);

	/*Pattern not found, return original string*/
	if (distance == 0)
	{
		return strdup(string);
	}

	char* endRegion = string + distance + strlen(find);
	size_t joinsize = strlen(beginRegion) + strlen(replace) + strlen(endRegion) + 1;
	char* newString = (char*)malloc(joinsize);
	memset(newString, 0, joinsize);
	newString = strcat(newString, beginRegion);
	newString = strcat(newString, replace);
	newString = strcat(newString, endRegion);
	free(beginRegion);
	return newString;
}

char* strextract(char* string, char* begin, char* end, enum Crop crop)
{
	char* startp = NULL;
	char* endp = NULL;
	for (startp = string; *startp != '\0'; ++startp)
	{
		if (*startp == *begin && (strncmp(startp, begin, strlen(begin)) == 0)) {
			break;
		}
	}
	if (*startp == '\0') {return NULL;}
	for (endp = startp + strlen(begin); *endp != '\0'; ++endp)
	{
		if (*endp == *end && (strncmp(endp, end, strlen(end)) == 0))
		{
			break;
		}
	}
	char* extractStart = (crop == NOCROP || crop == CROPEND) ? startp : startp + strlen(begin);
	char* extractEnd = (crop == NOCROP || crop == CROPBEGIN) ? endp + strlen(end) : endp;
	return strndup(extractStart, extractEnd - extractStart);
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

ssize_t readToBuffer(char* filename, char* buffer, size_t buffsize)
{
	ssize_t bytes = 0;
	int fd = open(filename, 0, 0666);
	if (fd < 0) {
		perror("(failed to open disk file)");
	}
	if ((bytes = read(fd, buffer, buffsize)) < 0) {
		perror("(failed to read to buffer)");
	}
	return bytes;
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
