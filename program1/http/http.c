#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http.h"
#include "io.h"

struct HttpHeader* http_header_construct(struct Http http, char* line)
{
	struct HttpHeader* Header = malloc(sizeof(struct HttpHeader));
	char* savePoint;
	Header->key = strtok_r(line, ":", &savePoint);
	Header->value = strtok_r(NULL, "\r\n", &savePoint);
	Header->next = http.headerHead;
	Header->prev = NULL;
	return Header;
}

struct Http http_construct(char* message)
{
	struct Http http;
	http.headerHead = NULL;
	http.msg = strdup(message);
	http.version = (char*)malloc(32);
	http.method = (char*)malloc(16);
	http.path = (char*)malloc(512);

	char* savePoint;
	char* requestLine = strdup(strtok_r(message, "\r\n", &savePoint));
	char* headerLine = NULL;
	sscanf(requestLine, "%s %s %s", http.method, http.path, http.version);
	while ((headerLine = strtok_r(NULL, "\r\n", &savePoint)) != NULL) {
		http.headerHead = http_header_construct(http, headerLine);
		if (http.headerHead->next != NULL)
		{
			http.headerHead->next->prev = http.headerHead;
		}
	}
	return http;
}

char* buildhttpString(struct Http http)
{
	/*
		 char* httpString = concat(concat(http.method, http.path), http.version);
		 struct HttpHeader* header = NULL;
		 for (header = http.headerHead; header != NULL; header = header->next)
		 {
		 char* headerText = concat(concat(header->key, header->value), "\r\n");
		 httpString = concat(httpString, headerText);
		 }
		 return httpString;
		 */
	return NULL;
}

struct HttpHeader* getHeader(struct Http http, char* key)
{
	struct HttpHeader* p;
	for (p = http.headerHead; p != NULL; p = p->next)
	{
		if (strcmp(key, p->key) == 0) {
			return p;
		}
	}
	return NULL;
}

bool removeHeader(struct Http* http, char* key)
{
	struct HttpHeader* headerToRemove = getHeader(*http, key);
	if (headerToRemove == NULL)
	{
		return NULL;
	}
	//Only header
	if (headerToRemove->next == NULL)
	{
		http->headerHead = NULL;
		free(headerToRemove);
		return true;
	}
	//First header
	if (headerToRemove->prev == NULL)
	{
		http->headerHead = headerToRemove->next;
		http->headerHead->prev = NULL;
		free(headerToRemove);
		return true;
	}
	//Middle header
	headerToRemove->next->prev = headerToRemove->prev;
	headerToRemove->prev->next = headerToRemove->next;
	free(headerToRemove);
	return true;
}

char* getHeaderValue(struct Http http, char* key)
{
	struct HttpHeader* Header = getHeader(http, key);
	return (Header != NULL) ? strdup(Header->value) : NULL;
}

struct HttpHeader* modifyHeaderValue(struct Http http, char* key, char* insert)
{
	struct HttpHeader* Header = getHeader(http, key);
	free(Header->value);
	Header->value = insert;
	return Header;
}
