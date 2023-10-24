#ifndef HTTP_H
#define HTTP_H

#include "http.h"

struct http {
	char* text;
	char* method;
	char* url;
	char** requestLines;
}

struct header {
	char* key;
	char* value;
	struct header* next;
}

struct header http_header_construct(struct http Http, char* line)
{
	struct header Header;
	char* savePoint;
	Header.key = strtok_r(line, ":", &savePoint);
	Header.value = strtok_r(NULL, "\r\n", &savePoint);
	Header.next = Http.header;
}

struct http http_construct(char* message)
{
	struct http Http;
	Http.header = NULL;

	char* savePoint;
	Http.request = sscanf(strtok_r(message, "\r\n", &savePoint));
	char* header;
	int i = 0;
	for (header = strtok_r(NULL, "\r\n", &savePoint; header != NULL; ++i))
	{

	}


}

#endif
