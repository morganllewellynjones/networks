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

#include "server.h"
#include "http.h"
#include "io.h"

#define BUFFSIZE 8192

static volatile bool sigint = false;

void sigint_handle(int) {
	fprintf(stdout, "\nsigint received!\n");
	sigint = true;
}

void register_sigint() {
	struct sigaction sigintaction;
	sigintaction.sa_handler = sigint_handle;
	if (sigaction(SIGINT, &sigintaction, NULL) < 0) {
		perror("(sigint register)");
		exit(EXIT_FAILURE);
	}
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

int main()
{
	register_sigint();

	//creates socket, binds and listens
	struct Host proxy = initServer(8080);
	struct Host client = acceptClient(proxy);
	//char buffer[BUFFSIZE];
	//memset(buffer, 0, sizeof(buffer));
	//read(Client.fd, buffer, sizeof(buffer));
	if (!cacheFile("request.http", client.fd)) {
		return EXIT_FAILURE;
	}
	//struct Http httpRequest = http_construct(buffer);
	//fprintf(stdout, httpRequest.msg);

	return EXIT_SUCCESS;
}
