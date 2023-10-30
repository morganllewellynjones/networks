#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include "host.h"
#include "io.h"

/*Author: Morgan Jones
 * 10/29/2023
 * This work is my own, see README for details*/


static volatile bool sigint = false;
static const size_t buffsize = 8192;

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

struct Url extractRequestUrl(char* request) 
{
	struct Url url;
	size_t savePos = 0;
	url.full = strextract(request, "/", " ", CROPBOTH);

	char* https = strextract(request, "https:/", "/", NOCROP);
	char* http = strextract(request, "http:/", "/", NOCROP);
	char* hostname = url.full;

	if (https != NULL) {
		hostname = strextract(hostname, https, "/", NOCROP);
	}
	else if (http != NULL) {
		hostname = strextract(hostname, http, "/", NOCROP);
	}
	else {
		hostname = strsplit(url.full, "/", &savePos);
	}

	url.hostname = hostname;

	/*If dist == 0 then strsplit found no '/' and therefore there is no path, only a hostname */
	url.path = (savePos == 0) ? strdup("") : strdup(url.full + savePos + 1);

	return url;
}

void purgeUrl(struct Url url)
{
	free(url.full);
	free(url.hostname);
	free(url.path);
}

int getHttpStatus(char* response)
{
	/*Parses an Http string for the status number in the request line
	 * */

	char* statusCode = strextract(response, " ", " ", CROPBOTH);
	int status = (statusCode == NULL) ? 0 : atoi(statusCode);
	free(statusCode);
	return status;
}

char* urlStoreName(char* url)
{
	/* Maps a url to a name used by the caches filesystem
	 * All cache files are stored in the cache folder
	 * No subdirectories are made, so instead we change the '/'
	 * to a '|' in the pathname when storing and retrieving
	 * */

	int elementCount = wordCount(url, "/") + 1;
	char** elements = stringToArray(url, "/");
	char* fileUrl = arrayToString(elements, "|", elementCount);
	char path[] = "./cache/";
	char* fullUrl[2] = {path, fileUrl};
	char* storeName = arrayToString(fullUrl, "", 2);

	free(fileUrl);
	for (int i = 0; i < elementCount; ++i)
	{
		free(elements[i]);
	}
	free(elements);
	return storeName;

}

char* prepareForwardRequest(char* request)
{
	/*The request made to the proxy needs to be modified for forwarding to the end server
	 * We use a number of string functions here, some custom built in io.c, to modify the original
	 * request to be suitable for forwarding */

	/*Change connection to Closed or tcp will hang*/
	char* stage1 = strreplace(request, "Connection: keep-alive", "Connection: Close");

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

		/*Read request from client*/
		char buffer[buffsize];
		memset(buffer, 0, sizeof(buffer));
		size_t bytes = Read(client.fd, buffer, sizeof(buffer)-1);

		/*client request is made to proxy, prepare request to be forwarded to end server*/
		struct Url url = extractRequestUrl(buffer);
		char* request = prepareForwardRequest(buffer);

		/*create cache directory for future writes*/
		int cachedir = mkdir("cache", 00777);

		/*log request being forwarded for debugging*/
		int requestfd = Open("./cache/request.http", O_CREAT | O_RDWR, 0666);
		Write(requestfd, request, strlen(request));
		Close(requestfd);

		/*check if requested file exists in our cache*/
		char* fileUrl = urlStoreName(url.full);
		int sourcefd = open(fileUrl, O_RDWR, 0666);
		if (sourcefd != -1)
		{
			/*Read to client*/
			writeToSocket(sourcefd, client.fd, buffer, sizeof(buffer)-1);
			purgeUrl(url);
			Close(sourcefd);
			Close(client.fd);
			Close(cachedir);
			free(fileUrl);
			free(request);
			continue;
		}

		/*connect socket to end server and Write formatted request*/
		int serverfd = connectToServer(url.hostname, "80");
		bytes = Write(serverfd, request, strlen(request));

		/*open cache file, set buffer*/
		int cachefd = Open(fileUrl, O_CREAT | O_WRONLY, 0666);
		memset(buffer, 0, sizeof(buffer));

		/*Write server response to cache and to client simultaneously*/
		while ((bytes = Read(serverfd, buffer, sizeof(buffer) -1)) > 0)
		{
			Write(cachefd, buffer, bytes);
			Write(client.fd, buffer, bytes);
			fprintf(stdout, "%s", buffer);
			memset(buffer, 0, bytes);
		}

		/*Awkwardly close the fd and then reopen it
		 * Otherwise we can only write to it and not read
		 * */

		Close(cachefd);
		cachefd = Open(fileUrl, O_RDONLY, 0666);

		bytes = Read(cachefd, buffer, sizeof(buffer) -1);

		/*check status code, do not cache bad requests or file not found*/
		int httpStatus = getHttpStatus(buffer);
		if (httpStatus != 200) {
			Remove(fileUrl);
		}

		Close(sourcefd);
		Close(cachefd);
		Close(serverfd);
		Close(cachedir);
		purgeUrl(url);
		free(request);
		free(fileUrl);
	}

	return 0;
}
