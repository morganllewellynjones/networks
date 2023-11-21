#include "router.h"

bool isIPString(char* ipstring)
{
	if ((strlen(ipstring)) > 24)
	{
		fprintf(stderr, "Error: string too large to correspond to IPV4 address or netmask\n");
		return false;
	}
	else if ((strchr(ipstring, '.')) == NULL)
	{
		return false;
	}
	return true;
}

unsigned int IPtoInt(char* ipstring)
{
	unsigned int subnet1, subnet2, subnet3, subnet4;
	sscanf(ipstring, "%d.%d.%d.%d", &subnet1, &subnet2, &subnet3, &subnet4);
	unsigned int netmask = (subnet1 << 24) + (subnet2 << 16) + (subnet3 << 8) + subnet4;
	return netmask;
}

struct Packet strToPacket(char* data)
{
	struct Packet packet;
	packet.src = IPtoInt(strsep(&data, ","));
	packet.dest = IPtoInt(strsep(&data, ","));
	packet.payload = strdup(strsep(&data, ","));
	packet.ttl = (unsigned char)(*(strsep(&data, "\r")) - '0');
	return packet;
}

void acceptPackets(struct Router router, struct Host interface)
{
	struct Host client = acceptClient(interface);

	struct ThreadData threadData;
	threadData.router = router;
	threadData.client = client;

	pthread_t clientThread;
	pthread_create((void*)&threadData, NULL, receiveRequest, (void*)&client);
}

void* receiveRequest(void* args)
{
	/*A function to be processed within a pthread
	 * Accepted client connection is passed to the thread
	 * Thread then extracts packet data from the message and processes it */
	const int buffsize = 1024;

	struct ThreadData* data = (struct ThreadData*)args;
	struct Host client = data->client;
	struct Router router = data->router;
	char buffer[buffsize];
	memset(buffer, 0, buffsize);
	Read(client.fd, buffer, buffsize-1);

	struct Packet packet = strToPacket(buffer);
	processRequest(router, &packet, buffer);

	return (void*)0;
}

void processRequest(struct Router router, struct Packet* packet, char* rawPacketString)
{
	//write {payload received by router#} to received_by_router_#.txt
	char outfile[] = "received_by_router_#.txt";
	*strchr(outfile, '#') = router.id + '0';

	int fd = Open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
	Write(fd, rawPacketString, strlen(rawPacketString));

	/* Decrement and check for discard */
	if (packet->ttl-- <= 0) {
		char discardfile[] = "discarded_by_router_#.txt";
		*strchr(discardfile, '#') = router.id + '0';

		int discardfd = Open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
		Write(discardfd, rawPacketString, strlen(rawPacketString));
		//write {payload discarded by router #} to discarded_by_router_#.txt
		return;
	}
	char* interface = match(router.forwardTable, packet->dest);
	//if interface == 127.0.0.1 then write payload only to out_router_#.txt
	//return
	if (strcmp(interface, "127.0.0.1") == 0) {
		char outfile[] = "out_router_#.txt";
		*strchr(outfile, '#') = router.id + '0';

		int outfd = Open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
		Write(outfd, packet->payload, strlen(packet->payload));

		close(outfd);
		return;
	}

	//send packet
	//write {payload sent by router# to dest#} to sent_by_router_#.txt
	/*
		 int serverfd = connectToServer("127.0.0.1", interface);
		 Write(serverfd, rawPacketString, strlen(rawPacketString));
		 char sentFile[] = "sent_by_router_#.txt";
	 *strchr(sentFile, '#') = router.id + '0';
	 int sentfd = Open(sentFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
	 Write(sentfd, "sent by router\n", strlen("sent by router"));
	 */
	return;
}

char* match(struct ForwardTable table, unsigned int packetDest)
{
	unsigned int longestPrefix = 0;
	char* interface = NULL;

	for (int entry = 0; entry < table.entries; ++entry)
	{
		unsigned int networkid = table.dest[entry] & table.netmask[entry];
		bool isMatch = (networkid == (packetDest & table.netmask[entry]));
		bool isLonger = (networkid >= longestPrefix);

		interface = (isMatch && isLonger) ? table.interface[entry] : interface;
		longestPrefix = (isMatch && isLonger) ? networkid : longestPrefix;
	}

	return strdup(interface);
}

struct ForwardTable constructForwardTable()
{
	struct ForwardTable table;
	table.entries = 0;
	memset(table.dest, 0, sizeof(unsigned int) * TABLE_SIZE);
	memset(table.netmask, 0, sizeof(unsigned int) * TABLE_SIZE);
	memset(table.gateway, 0, sizeof(unsigned int) * TABLE_SIZE);

	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		table.interface[i] = NULL;
	}

	return table;
}

void getForwardTable(struct ForwardTable* table, char* filename)
{
	const size_t buffsize = 8192;
	int fd = Open(filename, O_RDONLY, 0666);
	char* buffer = alloca(buffsize);
	memset(buffer, 0, buffsize);
	while ((Read(fd, buffer, buffsize-1) > 0))
		;

	unsigned int entry = 0;
	char* line = NULL;
	while ((line = strsep(&buffer, "\n")) != NULL)
	{
		table->dest[entry] = IPtoInt(strsep(&line, ","));
		table->netmask[entry] = IPtoInt(strsep(&line, ","));
		table->gateway[entry] = IPtoInt(strsep(&line, ","));
		table->interface[entry] = strdup(strsep(&line, "\r"));

		++entry;
	}
	Close(fd);
	table->entries = entry;
}

struct Router constructRouter(char id, struct Host* interfaces, short interfaceCount)
{
	struct Router router;
	router.id = id;
	router.interfaces = Malloc(sizeof(struct Host) * interfaceCount);
	for (short i = 0; i < interfaceCount; ++i)
	{
		router.interfaces[i] = interfaces[i];
	}
	char forwardTableFile[] = "input/router_#_table.csv";
	*strchr(forwardTableFile, '#') = id + '0';
	(void)getForwardTable(&router.forwardTable, forwardTableFile);

	return router;
}

void printForwardTable(struct ForwardTable table)
{
	const int entries = table.entries;
	for (int entry = 0; entry < entries; ++entry)
	{
		printf("%x, ", table.dest[entry]);
		printf("%x, ", table.netmask[entry]);
		printf("%x, ", table.gateway[entry]);
		printf("%s\n", table.interface[entry]);
	}
}
