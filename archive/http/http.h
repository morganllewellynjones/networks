#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>

enum httpType {
	REQUEST,
	RESPONSE
};

enum statusCode {
	OK = 200,
	RELOCATE = 301,
	BADREQUEST = 400,
	NOTFOUND = 404,
	SERVERERROR = 500,
};

struct HttpHeader {
	char* key;
	char* value;
	struct HttpHeader* next;
	struct HttpHeader* prev;
};

struct Http {
	char* msg;
	char* method;
	char* version;
	char* path;
	enum httpType type;
	enum statusCode status;
	struct HttpHeader* headerHead;
};

struct HttpHeader* http_header_construct(struct Http http, char* line);
struct Http http_construct(char* message);
char* getHeaderValue(struct Http http, char* key);
char* buildhttpString(struct Http http);
struct HttpHeader* getHeader(struct Http http, char* key);
bool removeHeader(struct Http* http, char* key);
struct HttpHeader* modifyHeaderValue(struct Http http, char* key, char* insert);
#endif
