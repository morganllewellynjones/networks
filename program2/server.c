#include <stdio.h>
#include <stdlib.h>
#include "host.h"
#include "io.h"

int main()
{
	struct Host server = initServer(8080, SOCK_DGRAM);
	printf("Instantialized server at port 8080\n");

	char buffer[1024];
	memset(buffer, 0, 1024);
	Read(server.fd, buffer, 1023);
	printf("%s\n", buffer);
}
