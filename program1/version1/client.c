#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <unistd.h>

void handle_error(const char* msg) {
	printf("%s\n", msg);
	exit(EXIT_FAILURE);;
}
/*
	 struct addrinfo {
	 int              ai_flags;
	 int              ai_family;
	 int              ai_socktype;
	 int              ai_protocol;
	 socklen_t        ai_addrlen;
	 struct sockaddr *ai_addr;
	 char            *ai_canonname;
	 struct addrinfo *ai_next;
	 };*/
int main(int argc, char** argv) {

	short port = 8080;
	int opt = -1;
	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}				

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	client_addr.sin_addr.s_addr = INADDR_ANY;

	const int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		handle_error("Failed to open socket\n");
	}

	if (connect(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
		handle_error("Failed to connect\n");
	};

	//struct addrinfo serveraddr;
	//struct addrinfo* addrlist;
	//getaddrinfo("localhost", "8080", &serveraddr, &addrlist);
	printf("Connected to server\n");
	write(client_fd, "Hello world\n", strlen("Hello world\n"));
	close(client_fd);
}
