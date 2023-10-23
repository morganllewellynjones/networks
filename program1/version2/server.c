#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "server.h"

struct client acceptClient(struct server Server) {
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	struct client Client;
	Client.addr = clientAddr;
	Client.addr_len = sizeof(Client.addr);
	Client.fd = (accept(Server.fd, (struct sockaddr*)&Client.addr, &Client.addr_len));
	if (Client.fd < 0) {
		perror("(accepting client)");
	};
	fprintf(stdout, "Received connection from %s\n", inet_ntoa(Client.addr.sin_addr));
	return Client;
}

struct server initServer(short port) {

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	struct server Server;
	Server.addr = serverAddr;
	Server.addr_len = sizeof(Server.addr);
	Server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (Server.fd < 0) {
		perror("(socket create)");
	}
	if (bind(Server.fd, (struct sockaddr*)&Server.addr, Server.addr_len) < 0) {
		perror("(server bind)");
		exit(EXIT_FAILURE);
	}
	if (listen(Server.fd, SOMAXCONN) < 0) {
		perror("(server listen)");
		exit(EXIT_FAILURE);
	}
	return Server;
}
