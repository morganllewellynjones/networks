#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "server.h"

static volatile bool interrupted = false;

void sigint_handle(int) {
	interrupted = true;
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

void* fetchFile(void* args)
{
	struct server Host = *(struct server*)args;
}

int main()
{
	register_sigint();

	//creates socket, binds and listens
	struct server Host = initServer(8080);
	//struct client Client = acceptClient(Host);

	pthread_t clientThread;
	pthread_create(&clientThread, NULL, acceptConnections, &Host);
	pthread_join(clientThread, NULL);
	return 0;
}
