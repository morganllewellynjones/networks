#include "io.h"

/*	Author: Morgan Jones
 *	10/29/2023
 *
 *	I am the sole author of this work, which contains a number
 *	of string functions which I found helpful in parsing http blocks,
 *	and read/write wrappers and operations.
 *	None of the string operations created here modify the original, 
 *	they all return a duplicate. The user must clean up the duplicate.
 * */

char* arrayToString(char** strings, char* delim, size_t strcount)
{
	/* Takes and array of strings and a delimiter, and merges the strings into a single string 
	 * joined on the delimiter */
	size_t joinsize = 1;
	for (int i = 0; i < strcount; ++i)
	{
		joinsize += strlen(strings[i]);
		joinsize += strlen(delim);
	}
	char* joined = (char*)Malloc(joinsize);
	memset(joined, 0, joinsize);
	for (int i = 0; i < strcount; ++i)
	{
		if (i != 0) {
			joined = strcat(joined, delim);
		}
		joined = strcat(joined, strings[i]);
	}
	joined[joinsize-1] = '\0';
	return joined;
}

int wordCount(char* string, char* word)
{
	/* Looks through a string for every instance of a given word and returns the count */
	int count = 0;
	for (char* p = string; *p != '\0'; ++p)
	{
		if (*p == *word && strncmp(p, word, strlen(word)) == 0) {
			++count;
		}
	}
	return count;
}

char** stringToArray(char* string, char* delim)
{
	/* Intakes a string and splits it into an array on the delimiter */
	int elements = wordCount(string, delim) + 1;
	char** array = (char**)Malloc(sizeof(char*) * elements);
	size_t distance = 0;
	size_t delimlen = strlen(delim);
	for (int i = 0; i < elements; ++i) {
		array[i] = strsplit(string, delim, &distance);
		distance += delimlen;
	}
	return array;
}

char* strsplit(char* string, char* delim, size_t* distance)
{
	/*Searches for a delimiter and returns a substring of the input up to that delimiter
	 * Function does not modify original string, it returns a duplicate, which needs to be cleaned up
	 * Distance is passed in as a save point for future string operations to know where the first delimiter was found
	 * If distance is returned as 0 then no delimiter was found */
	if (*distance > strlen(string))
	{
		return string;
	}
	char* start = string + *distance;
	for (char* p = start; *p != '\0'; ++p)
	{
		if (*p == *delim && (strncmp(p, delim, strlen(delim)) == 0))
		{
			char* split = strndup(start, p - start);
			*distance = p - string;
			return split;
		}
	}
	/*If no delimiter found, just return a copy of the original string*/
	return strdup(start);
}

char* strreplace(char* string, char* find, char* replace)
{
	/* Returns a duplicate string with the first instance of find switched to replace */
	size_t distance = 0;
	char* beginRegion = strsplit(string, find, &distance);

	/* Pattern not found, return original string */
	if (distance == 0)
	{
		return strdup(string);
	}

	char* endRegion = string + distance + strlen(find);
	size_t joinsize = strlen(beginRegion) + strlen(replace) + strlen(endRegion) + 1;
	char* newString = (char*)Malloc(joinsize);
	memset(newString, 0, joinsize);
	newString = strcat(newString, beginRegion);
	newString = strcat(newString, replace);
	newString = strcat(newString, endRegion);
	free(beginRegion);
	return newString;
}

char* strextract(char* string, char* begin, char* end, enum Crop crop)
{
	/*Finds the first instance in a string which begins with 'begin' and ends with 'end'
	 * If the region begin-end is not found return NULL
	 * CROP settings determine which delimiters the returned substring includes, if any */
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

size_t writeToSocket(int sourcefd, int destfd, char* buffer, size_t buffsize)
{
	/* Reads from sourcefd into a buffer, and then writes the buffer to the given dest
	 * Can technically be used for any two files, not just sockets */
	size_t bytes = 0;
	size_t accum = 0;
	while ((bytes = Read(sourcefd, buffer, buffsize)) > 0)
	{
		Write(destfd, buffer, buffsize);
		accum += bytes;
		memset(buffer, 0, bytes);
	}
	return accum;
}


/* A series of io wrappers to provide built in error handling on call */

int Open (char* pathname, int flags, mode_t mode)
{
	int fd = open(pathname, flags, mode);
	if (fd < 0) {
		perror("(opening file error)");
		exit(EXIT_FAILURE);
	}
	return fd;
}

ssize_t Write (int fd, char* buffer, size_t buffsize)
{
	ssize_t bytes = write(fd, buffer, buffsize);
	if (bytes < 0) {
		perror("(write to file error)");
		exit(EXIT_FAILURE);
	}
	return bytes;
}

ssize_t Read (int fd, char* buffer, size_t buffsize)
{
	ssize_t bytes = read(fd, buffer, buffsize);
	if (bytes < 0) {
		perror("(read from file error)");
		exit(EXIT_FAILURE);
	}
	return bytes;
}

int Close (int fd)
{
	if (fd < 0) {
		return 0;
	}
	int status = close(fd);
	if (status < 0) {
		perror("(error closing file)");
	}
	return status;
}

int Remove (char* pathname)
{
	int status = remove(pathname);
	if (status < 0)
	{
		perror("(failed to remove file)");
		exit(EXIT_FAILURE);
	}
	return status;
}

void* Malloc(size_t size)
{
	void* memory = malloc(size);
	if (memory == NULL) {
		perror("(malloc failed)");
		exit(EXIT_FAILURE);
	}
	return memory;
}
