#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netdb.h>

static volatile sig_atomic_t keepRunning = 1;

void handle_error(const char* msg) {
	printf("%s\n", msg);
	exit(EXIT_FAILURE);;
}

void interruptHandler(int) {
	fprintf(stdout, "Shutting down!\n");
	keepRunning = 0;
}

char* readFile(int fd) {
	const off_t fileSize = lseek(fd, 0, SEEK_END);
	char* buffer = malloc(fileSize + 1);
	read(fd, buffer, fileSize);
	buffer[fileSize] = 0;
	return buffer;
}

size_t getLineSize(char* string) {
	char* p = NULL;
	size_t lineSize = 0;
	for (p = string; *p != 0; ++p)
	{
		++lineSize;
		if (*p == '\n') {
			break;
		}
	}
	return lineSize;
}

char* readLine(char* string) {
	size_t index;
	size_t lineSize = getLineSize(string);
	char* lineBuffer = malloc(lineSize);
	for (index = 0; index < lineSize; ++index)
	{
		lineBuffer[index] = *(string + index);
	}
	lineBuffer[index] = 0;
	return lineBuffer;
}

int main(int argc, char** argv) 
{
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

	const int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	struct sigaction interruptHandlerAction;
	interruptHandlerAction.sa_handler = interruptHandler;
	sigaction(SIGINT, &interruptHandlerAction, NULL);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (serverfd < 0) {
		handle_error("Failed to create socket\n");
	}

	printf("Initializing server\n");

	if (bind(serverfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		handle_error("Failed to bind socket\n");
	}

	printf("Bound socket\n");

	if (listen(serverfd, 1024) < 0) {
		handle_error("Socket failed to listen\n");
	}

	printf("Server listening for incoming requests\n");

	while (keepRunning == 1) {
		printf("Ready to serve\n");
		struct sockaddr_in client_addr;
		socklen_t client_length = sizeof(struct sockaddr_in);
		const int client_fd = accept(serverfd, (struct sockaddr*)&client_addr, &client_length);
		if (client_fd < 0) {
			printf("Error connecting to client\n");
			continue;
		}
		printf("Received connection from %s\n", inet_ntoa(client_addr.sin_addr));
		struct sockaddr_in requestAddr;
		requestAddr.sin_family = AF_INET;
		requestAddr.sin_port = htons(port);
		requestAddr.sin_addr.s_addr = INADDR_ANY;

		const int requestFd = socket(AF_INET, SOCK_STREAM, 0);
		if (requestFd < 0) {
			handle_error("Failed to open new socket\n");
		}
		printf("Opened a new socket!\n");

		if (connect(requestFd, (struct sockaddr*)&requestAddr, sizeof(requestAddr)) < 0) {
			handle_error("Failed to connect to end server\n");
		}
		printf("Connected to the end server!\n");
		char msg[8096];
		socklen_t msgLen;
		/*ssize_t recvfrom(int sockfd, void buf[restrict .len], size_t len,
			int flags,
			struct sockaddr *_Nullable restrict src_addr,
			socklen_t *_Nullable restrict addrlen);*/
		recvfrom(client_fd, msg, sizeof(msg), 0, (struct sockaddr*)&client_addr, &msgLen);
		/*ssize_t sendto(int sockfd, const void buf[.len], size_t len, int flags,
			const struct sockaddr *dest_addr, socklen_t addrlen);*/

		printf("%s\n", msg);
		sendto(requestFd, msg, sizeof(msg), 0, (struct sockaddr*)&requestAddr, sizeof(requestAddr));


		//getaddrinfo("www.google.com", 80, &requestAddr, sizeof(struct sockaddr_in));
		//char* msg = readFile(client_fd);
		//char* requestLine = readLine(msg);
		//fprintf(stdout, "Request line: %s", requestLine);
		//fprintf(stdout, "Header and Body line: %s", msg);
		//
		//
		/*	char* msg_line = NULL;
				size_t line_size = 0;
				FILE* client_fp = fdopen(client_fd, "rb+");
				ssize_t bytes_read = getline(&msg_line, &line_size, client_fp);
				char msg[8096];
				char method[16];
				char path[128];
				char version[16];
				sscanf(msg_line, "%s /%s %s", method, path, version);
				fprintf(stdout, "Method is %s\n", method);
				fprintf(stdout, "Path is %s\n", path);
				fprintf(stdout, "Version is %s\n", version);
				while (getline(&msg_line, &line_size, client_fp) > 0) {
				fprintf(stdout, "%s", msg_line);
				}
				*/
		/*
			 while ((bytes_read = getline(&msg_line, &line_size, client_fp) > 0)) {
			 if (is_request_line) {
			 write(client_fd, "Hello world\n", strlen("Hello world\n"));
			 is_request_line = false;
			 }
		//printf("%s", msg_line);
		//write(client_fd, msg_line, bytes_read);
		free(msg_line);
		msg_line = NULL;
		fseek(client_fp, bytes_read, SEEK_CUR);
		}
		write(client_fd, "Hello world\n", strlen("Hello world\n"));
		*/
		close(client_fd);
		sleep(10);
	}
	close(serverfd);
	//pthread_t tid;
	//pthread_create(&tid, NULL, runSocket, (void *)client_fd);
}
