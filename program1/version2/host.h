#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

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
struct Host initServer(short port);
int connectToServer(char* hostName, char* service);

#endif
