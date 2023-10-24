#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char msg[] = "GET / HTTP/1.1\r\nHost: google.com\r\nConnection: keep-alive\r\n\r\n";
	char* savePointer;
	printf("%s\n", strtok_r(msg, "\r\n", &savePointer));
	printf("%s\n", strtok_r(NULL, "\r\n", &savePointer));
	printf("%s\n", strtok_r(NULL, "\r\n", &savePointer));
	return 0;
}
