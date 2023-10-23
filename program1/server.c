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
#include <fcntl.h>
#include <pthread.h>

#define BUFF_SIZE 128000

static volatile sig_atomic_t keepRunning = 1;

void handle_error(const char* msg) {
	printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void interruptHandler(int blank) {
	fprintf(stdout, "Shutting down!\n");
	keepRunning = 0;
}

void* fetchRequest(void* args) {

	char buffer[BUFF_SIZE];
	int tempfd = open("serverPage", O_RDWR | O_CREAT, 0666);
	size_t responseLen = 0;
	while (responseLen = read(socketfd, buffer, sizeof(buffer));
			size_t bytesRead = read(socketfd, buffer, sizeof(buffer));
			write(tempfd, buffer, bytesRead);

			}

			char* readFile(int fd) {
			const off_t fileSize = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);
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
	struct sigaction interruptHandlerAction;
	interruptHandlerAction.sa_handler = interruptHandler;
	sigaction(SIGINT, &interruptHandlerAction, NULL);

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

	const int hostfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in hostAddr;

	memset(&hostAddr, 0, sizeof(hostAddr));
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(port);
	hostAddr.sin_addr.s_addr = INADDR_ANY;

	if (hostfd < 0) {
		handle_error("Failed to create socket\n");
	}

	printf("Initializing server\n");

	if (bind(hostfd, (struct sockaddr*)&hostAddr, sizeof(hostAddr)) < 0) {
		handle_error("Failed to bind socket\n");
	}

	printf("Bound socket\n");

	if (listen(hostfd, 1024) < 0) {
		handle_error("Socket failed to listen\n");
	}

	printf("Server listening for incoming requests\n");

	while (keepRunning == 1) {
		printf("Ready to serve\n");
		struct sockaddr_in clientAddr;
		socklen_t clientLength = sizeof(struct sockaddr_in);
		const int clientfd = accept(hostfd, (struct sockaddr*)&clientAddr, &clientLength);
		if (clientfd < 0) {
			printf("Error connecting to client\n");
			continue;
		}
		printf("Received connection from %s\n", inet_ntoa(clientAddr.sin_addr));
		char clientMessage[8096];
		read(clientfd, clientMessage, sizeof(clientMessage));
		printf("%s\n", clientMessage);
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
			 };
			 */

		struct addrinfo hints;
		struct addrinfo* serverAddresses;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;    /* Allow IPv4 */
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = 0;
		hints.ai_protocol = 0;          /* Any protocol */
		//const short httpport = htons(30);
		int error = getaddrinfo("google.com", "80", &hints, &serverAddresses);
		if (error != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		}
		struct addrinfo* currAddr;
		int socketfd = -1;
		for (currAddr = serverAddresses; currAddr != NULL; currAddr = currAddr->ai_next)
		{
			socketfd = socket(currAddr->ai_family, currAddr->ai_socktype,
					currAddr->ai_protocol);
			if (socketfd == -1) {
				continue;
			}

			if (connect(socketfd, currAddr->ai_addr, currAddr->ai_addrlen) != -1) {
				break;                  /* Success */
			}

			close(socketfd);
		}

		freeaddrinfo(serverAddresses);           /* No longer needed */

		if (currAddr == NULL) {               /* No address succeeded */
			fprintf(stderr, "Could not connect\n");
			exit(EXIT_FAILURE);
		}

		char serverRequest[] = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
		size_t bytesToServer = write(socketfd, serverRequest, strlen(serverRequest));
		fprintf(stdout, "Wrote %ld bytes to server\n", bytesToServer);
		char buffer[BUFF_SIZE];
		pthread_t fetchThread;
		pthread_create(&fetchThread, NULL, fetchRequest, socketfd);
		/*
			 while (((bytesRead = read(socketfd, buffer, sizeof(buffer))) > 0)) {
			 if (keepRunning == 0) break;
			 if (buffer[bytesRead] == '0') break;
			 fprintf(stdout, "STILL RUNNING!\n");
			 fprintf(stdout, "Reading %ld bytes to buffer\n", bytesRead);
			 fprintf(stdout, "Writing %ld bytes to file\n", write(tempfd, buffer, bytesRead));
			 fprintf(stdout, "%s", buffer);
			 };
			 */
		char* serverFile = readFile(tempfd);
		fprintf(stdout, serverFile);	
		fprintf(stdout, "Writing %ld bytes to client\n", write(clientfd, serverFile, strlen(serverFile)));

		//Send generic response to client
		/*
			 char resp[] = "HTTP/1.0 200 OK\r\n"
			 "Server: webserver-c\r\n"
			 "Content-type: text/html\r\n\r\n"
			 "<html>hello, world</html>\r\n";
			 ssize_t bytesWritten = write(clientfd, resp, strlen(resp));
			 printf("Wrote %ld bytes to the client\n", bytesWritten);
			 */
		close(tempfd);
		close(clientfd);
		close(socketfd);
	}
	close(hostfd);
	//pthread_t tid;
	////pthread_create(&tid, NULL, runSocket, (void *)clientfd);
}
