#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <arpa/inet.h>

struct server {
	int fd;
	struct sockaddr_in addr;
	socklen_t addr_len;
};

struct client {
	int fd;
	struct sockaddr_in addr;
	socklen_t addr_len;
};

struct client acceptClient(struct server Server);
struct server initServer(short port);

#endif
