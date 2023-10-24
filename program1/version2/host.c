#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "server.h"

struct Host acceptClient(struct Host host) {
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	struct Host client;
	client.addr = clientAddr;
	client.addr_len = sizeof(client.addr);
	client.fd = (accept(host.fd, (struct sockaddr*)&client.addr, &client.addr_len));
	if (client.fd < 0) {
		perror("(accepting client)");
	};
	fprintf(stdout, "Received connection from %s\n", inet_ntoa(client.addr.sin_addr));
	return client;
}



struct Host initServer(short port) {

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	struct Host host;
	host.addr = serverAddr;
	host.addr_len = sizeof(host.addr);
	host.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (host.fd < 0) {
		perror("(socket create)");
	}
	if (bind(host.fd, (struct sockaddr*)&host.addr, host.addr_len) < 0) {
		perror("(server bind)");
		exit(EXIT_FAILURE);
	}
	if (listen(host.fd, SOMAXCONN) < 0) {
		perror("(server listen)");
		exit(EXIT_FAILURE);
	}
	return host;
}
