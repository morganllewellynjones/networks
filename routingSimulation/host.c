#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "host.h"

/*
 * Author: Morgan Jones
 * 1029/2023
 *
 * This work is my own.
 * It includes a struct and some helper functions to remove
 * clutter from server calls in the main event loop for proxy.
 * */

struct Host acceptClient(struct Host host) {
	/*
	 * Pass in a host which is bound and listening on a socket
	 * Accept a client connection and return a struct with its
	 * socket file descriptor.
	 * */

	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));

	struct Host client;
	client.addr = clientAddr;
	client.addr_len = sizeof(client.addr);
	client.fd = (accept(host.fd, (struct sockaddr*)&client.addr, &client.addr_len));

	if (client.fd < 0) {
		perror("(accepting client)");
		exit(EXIT_FAILURE);
	};
	fprintf(stdout, "Received connection from %s\n", inet_ntoa(client.addr.sin_addr));

	return client;
}

int connectToServer(char* hostname, char* service)
{
	/*
	 * Uses getAddrInfo to fetch a linked list of possible endpoints
	 * Checks each node in the list for a connection
	 * If a socket connects return its file descriptor
	 * */

	struct addrinfo hints;
	struct addrinfo* serverAddresses;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */
	//const short httpport = htons(30);
	int error = getaddrinfo(hostname, service, &hints, &serverAddresses);
	if (error != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		exit(EXIT_FAILURE);
	}
	struct addrinfo* currAddr;
	int socketfd = -1;
	for (currAddr = serverAddresses; currAddr != NULL; currAddr = currAddr->ai_next)
	{
		socketfd = socket(currAddr->ai_family, currAddr->ai_socktype,
				currAddr->ai_protocol);
		if (socketfd == -1) {continue;}
		if (connect(socketfd, currAddr->ai_addr, currAddr->ai_addrlen) != -1) {
			break;                  /* Success */
		}

		close(socketfd);
	}

	freeaddrinfo(serverAddresses);           /* No longer needed */

	if (currAddr == NULL) {               /* No address succeeded */
		perror("(could not connect to server\n)");
		exit(EXIT_FAILURE);
	}
	return socketfd;
}

int connectToLocal(int port, int socktype)
{  
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	client_addr.sin_addr.s_addr = INADDR_ANY;

	const int client_fd = Socket(AF_INET, socktype, 0);
	Connect(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
	return client_fd;
}

struct Host initServer(short port, int socktype) {

	/* Open a socket, bind it and set it to listen on port
	 * Return all details in the form of a host struct
	 * */

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	struct Host host;
	host.addr = serverAddr;
	host.addr_len = sizeof(host.addr);

	host.fd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(host.fd, (struct sockaddr*)&host.addr, host.addr_len);
	if (socktype == SOCK_STREAM) {
		Listen(host.fd, SOMAXCONN);
	}

	return host;
}

int Socket(int domain, int type, int protocol)
{
	int fd = socket(domain, type, protocol);
	if (fd < 0) {
		perror("(socket create)");
		exit(EXIT_FAILURE);
	}
	return fd;
}

int Bind(int fd, struct sockaddr* hostaddr, size_t addrlen)
{
	int status = 0;
	if ((status = bind(fd, hostaddr, addrlen)) < 0) {
		perror("(server bind)");
		exit(EXIT_FAILURE);
	}
	return status;
}

int Listen(int fd, int backlog)
{
	int status = 0;
	if ((status = listen(fd, backlog)) < 0) {
		perror("(server listen)");
		exit(EXIT_FAILURE);
	}
	return status;
}

int Connect(int fd, struct sockaddr* addr, size_t addrlen)
{
	int status = 0;
	if ((status = connect(fd, addr, addrlen)) < 0) {
		perror("(connect failed)");
		exit(EXIT_FAILURE);
	}
	return status;
}
