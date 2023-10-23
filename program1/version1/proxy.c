#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 500

	int
main(int argc, char *argv[])
{
	int                      serverfd, successCheck;
	char                     buffer[BUF_SIZE];
	ssize_t                  bytesRead;
	socklen_t                clientAddrlen;
	struct addrinfo          hints;
	struct addrinfo          *result, *rp;
	struct sockaddr_storage  clientAddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	successCheck = getaddrinfo(NULL, argv[1], &hints, &result);
	if (successCheck < 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(successCheck));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		 Try each address until we successfully bind(2).
		 If socket(2) (or bind(2)) fails, we (close the socket
		 and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		serverfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (serverfd == -1)
			continue;

		if (bind(serverfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  /* Success */

		close(serverfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	/* Read datagrams and echo them back to sender. */

	for (;;) {
		char host[NI_MAXHOST], service[NI_MAXSERV];

		clientAddrlen = sizeof(clientAddr);
		bytesRead = recvfrom(serverfd, buffer, BUF_SIZE, 0,
				(struct sockaddr *) &clientAddr, &clientAddrlen);
		if (bytesRead == -1)
			continue;               /* Ignore failed request */

		successCheck = getnameinfo((struct sockaddr *) &clientAddr, clientAddrlen, host, 
				NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		if (successCheck == 0)
		{
			printf("Received %zd bytes from %s:%s\n",
					bytesRead, host, service);
		}
		else {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(successCheck));
		}
		if (sendto(serverfd, buffer, bytesRead, 0, (struct sockaddr *) &clientAddr,
					clientAddrlen) != bytesRead)
		{
			fprintf(stderr, "Error sending response\n");
		}
	}
}
