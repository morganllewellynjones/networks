#include "router.h"

unsigned int IPtoInt(char* ipstring)
{
	unsigned int subnet1, subnet2, subnet3, subnet4;
	sscanf(ipstring, "%d.%d.%d.%d", &subnet1, &subnet2, &subnet3, &subnet4);
	unsigned int ipaddress = (subnet1 << 24) + (subnet2 << 16) + (subnet3 << 8) + subnet4;
	return ipaddress;
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
	/* Reads from the routers 'server' interface and forwards data to a pthread function for processing
	 * We do not store information about the client interface since we are not responding to connections */
	while (true)
	{
		const size_t buffsize = 4096;
		char buffer[buffsize];
		memset(buffer, 0, buffsize);

		/*Read packet data from interface*/
		Read(interface.fd, buffer, buffsize-1);

		/*Construct args for forwarding to pthread*/
		struct ThreadData* threadData = (struct ThreadData*)Malloc(sizeof(struct ThreadData));
		threadData->router = router;
		threadData->packetString = strdup(buffer);

		printf("Instantializing pthread\n");
		pthread_t packetThread;
		pthread_create((void*)&packetThread, NULL, receiveRequest, (void*)threadData);
	}
}

void* receiveRequest(void* args)
{
	/*A function to be processed within a pthread
	 * Accepted client connection is passed to the thread
	 * Thread then extracts packet data from the message and processes it */

	struct ThreadData* data = (struct ThreadData*)args;
	struct Router router = data->router;
	char* packetString = data->packetString;

	struct Packet packet = strToPacket(strdup(packetString));
	processRequest(router, &packet, packetString);

	return (void*)0;
}

void processRequest(struct Router router, struct Packet* packet, char* packetString)
{
	/*Written as a separate frunction from 'receiveRequest' 
	 * because router_1 processes requests but does not receive them */

	printf("Processing request\n");
	printf("String to process: ");
	printf(packetString);
	char receivedFile[] = "received_by_router_#.txt";
	*strchr(receivedFile, '#') = router.id + '0';

	int fd = Open(receivedFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
	Write(fd, packetString, strlen(packetString));

	/* Decrement string value and packet struct information
	 * String decrement is hacky but last 2 characters should always be \r\n 
	 * TTL right before */

	packetString[strlen(packetString) - 3] = --packet->ttl + '0';

	if (packet->ttl <= 0) {
		/*Write packet to discard file*/
		char discardfile[] = "discarded_by_router_#.txt";
		*strchr(discardfile, '#') = router.id + '0';

		int discardfd = Open(discardfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
		Write(discardfd, packetString, strlen(packetString));
		return;
	}
	char* interface = match(router.forwardTable, packet->dest);

	/* if interface == 127.0.0.1 then write payload only to out_router_#.txt */
	if (strcmp(interface, "127.0.0.1") == 0) {
		char outfile[] = "out_router_#.txt";
		*strchr(outfile, '#') = router.id + '0';

		int outfd = Open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
		Write(outfd, packet->payload, strlen(packet->payload));
		Write(outfd, "\n", 1);

		close(outfd);
		return;
	}

	/*Send packet and write to sent_by_router file */
	int serverfd = connectToLocal(atoi(interface), SOCK_DGRAM);
	Write(serverfd, packetString, strlen(packetString));

	char sentFile[] = "sent_by_router_#.txt";
	*strchr(sentFile, '#') = router.id + '0';

	int sentfd = Open(sentFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
	Write(sentfd, packetString, strlen(packetString));
	return;
}

char* match(struct ForwardTable table, unsigned int packetDest)
{
	/* Match an entry in the forwarding table to a packet destination.
	 * Since the router corresponds to a specific router on a subnet,
	 * and these packets are in theory destined for hosts, the addresses 
	 * form a disjoint set. We netmask both and compare to match packets 
	 * to the router that handles the appropriate subnet */

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
	/*Reads data from the csv filename into the forward table struct*/

	const size_t buffsize = 8192;
	int fd = Open(filename, O_RDONLY, 0666);
	char* buffer = alloca(buffsize); /*Allocates on the stack but stored as a pointer*/
	memset(buffer, 0, buffsize);
	while ((Read(fd, buffer, buffsize-1) > 0))
		;

	unsigned int index = 0;
	char* line = NULL;

	while ((line = strsep(&buffer, "\n")) != NULL)
	{
		table->dest[index] = IPtoInt(strsep(&line, ","));
		table->netmask[index] = IPtoInt(strsep(&line, ","));
		table->gateway[index] = IPtoInt(strsep(&line, ","));
		table->interface[index] = strdup(strsep(&line, "\r"));

		++index;
	}

	Close(fd);
	table->entries = index;
}

struct Router constructRouter(char id, struct Host* interfaces, short interfaceCount)
{
	/*Interfaces are passed in to the router since "in theory" we don't know
	 * how many there will be. In reality, every router only needs one interface 
	 * in order to be fully operational for the purposes of this assignments,
	 * since interfaces a, b, c, d..etc are abstract representations of client connections only.
	 * so we pass in only one interface to avoid unused variable warnings */

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
