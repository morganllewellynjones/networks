#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "io.h"

struct Url
{
	char* full;
	char* hostname;
	char* path;
};

struct Host {
	int fd;
	struct sockaddr_in addr;
	socklen_t addr_len;
};

struct Host acceptClient(struct Host host);
struct Host initServer(short port, int socktype);
int connectToLocal(int port, int socktype);
int connectToServer(char* hostName, char* service);
int Socket(int domain, int type, int protocol);
int Bind(int fd, struct sockaddr* hostaddr, size_t addrlen);
int Listen(int fd, int backlog);
int Connect(int fd, struct sockaddr* addr, size_t addrlen);

#endif
