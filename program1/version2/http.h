#ifndef HTTP_H
#define HTTP_H


struct http {
	char* text;
	char* method;
	char* url;
	char** requestLines;
}

#endif
