#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <arpa/inet.h>

struct Host {
	int fd;
	struct sockaddr_in addr;
	socklen_t addr_len;
};

struct Host acceptClient(struct Host host);
struct Host initServer(short port);

#endif
