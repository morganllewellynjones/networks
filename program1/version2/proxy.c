#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#include "host.h"
#include "http.h"
#include "io.h"

#define BUFFSIZE 1048576

static volatile bool sigint = false;

void sigint_handle(int) {
	fprintf(stdout, "\nsigint received!\n");
	sigint = true;
}

void register_sigint() {
	struct sigaction sigintaction;
	memset(&sigintaction, 0, sizeof(struct sigaction));
	sigintaction.sa_handler = sigint_handle;
	if (sigaction(SIGINT, &sigintaction, NULL) < 0) {
		perror("(sigint register)");
		exit(EXIT_FAILURE);
	}
}

short parseArgs(int argc, char** argv) {
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
	return port;
}
/*
	 int pthread_create(pthread_t *restrict thread,
	 const pthread_attr_t *restrict attr,
	 void *(*start_routine)(void *),
	 void *restrict arg);
	 */
/*
	 void* acceptConnections(void* args)
	 {
	 struct server Host = *(struct server*)args;
	 struct client Client = acceptClient(Host);
	 char buffer[8192];
	 memset(buffer, 0, sizeof(buffer));
	 size_t msgLength = read(Client.fd, buffer, sizeof(buffer));
	 fprintf(stdout, "%s\n", buffer);
	 return (void*)0;
	 }
	 */

/*
	 void* fetchFile(void* args)
	 {
	 struct server Host = *(struct server*)args;
	 }
	 */

struct Url
{
	char* full;
	char* hostname;
	char* path;
};

struct Url extractRequestUrl(char* request) 
{
	struct Url url;
	size_t dist = 0;
	url.full = strextract(request, "/", " ", CROPBOTH);

	/*Hostname is the section of the url up to a delimiting '/' */
	url.hostname = strsplit(url.full, "", &dist);

	/*If dist == 0 then strsplit found no '/' and therefore there is no path, only a hostname */
	url.path = (dist == 0) ? strdup("") : strdup(url.full + dist + 1);

	return url;
}

void purgeUrl(struct Url url)
{
	free(url.full);
	free(url.hostname);
	free(url.path);
}

char* prepareForwardRequest(char* request)
{
	/*The request made to the proxy needs to be modified for forwarding to the end server
	 * We use a number of string functions here, some custom built in io.c, to modify the original
	 * request to be suitable for forwarding */

	/*Change connection to closed or tcp will hang*/
	char* stage1 = strreplace(request, "Connection: keep-alive", "Connection: close");

	/*Remove hostname from request line and move it to host header*/
	struct Url url = extractRequestUrl(request);
	char* hostHeader = strextract(stage1, "Host: ", "\r\n", CROPBOTH);
	char* stage2 = strreplace(stage1, hostHeader, url.hostname);
	char* newRequest = strreplace(stage2, url.full, url.path);

	free(stage1);
	free(stage2);
	free(hostHeader);
	purgeUrl(url);

	return newRequest;
}

int main(int argc, char** argv)
{
	register_sigint();
	short port = parseArgs(argc, argv);

	/*creates socket, binds and listens*/
	struct Host proxy = initServer(port);

	struct Host client;
	while (!sigint)
	{
		client = acceptClient(proxy);

		/*read request from client*/
		char buffer[8192];
		memset(buffer, 0, sizeof(buffer));
		size_t bytes = read(client.fd, buffer, sizeof(buffer)-1);

		/*client request is made to proxy, prepare request to be forwarded to end server*/
		char* request = prepareForwardRequest(buffer);
		char* serverName = strextract(request, "Host: ", "\r\n", CROPBOTH);
		char* serverPort = "80";

		/*log request being forwarded for debugging*/
		int requestfd = open("request.http", O_CREAT | O_RDWR, 0666);
		write(requestfd, request, strlen(request));
		close(requestfd);

		/*connect socket to end server and write formatted request*/
		int serverfd = connectToServer(serverName, serverPort);
		bytes = write(serverfd, request, strlen(request));

		/*open cache file, set buffer*/
		int cachefd = open("response.http", O_CREAT | O_RDWR, 0666);
		memset(buffer, 0, sizeof(buffer));

		/*write server response to cache and to client simultaneously*/
		while ((bytes = read(serverfd, buffer, sizeof(buffer) -1)) > 0)
		{
			write(cachefd, buffer, bytes);
			write(client.fd, buffer, bytes);
			fprintf(stdout, "%s", buffer);
			memset(buffer, 0, bytes);
		}
		close(cachefd);
		close(serverfd);
	}

	return 0;
}
