#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "host.h"

int main()
{
	struct Host client;
	client.fd = connectToLocal(8080, SOCK_DGRAM);
	printf("Connected to server\n");
	Write(client.fd, "Hello world\n", strlen("Hello world\n"));

	return 0;
}
