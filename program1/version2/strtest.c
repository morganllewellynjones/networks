#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char msg[] = "GET / HTTP/1.1\r\nHost: google.com\r\nConnection: keep-alive\r\n\r\n";
	char* savePointer = NULL;
	char* token = strtok_r(msg, "\r\n", &savePointer);
	if (token != NULL) {
		printf(token);
	}
	while ((token = strtok_r(NULL, "\r\n", &savePointer)) != NULL)
	{
		printf(token);
	}
	return 0;
}
