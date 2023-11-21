#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"

char* getPacketData()
{
	/*Inelegantly I am storing all of the packet data in one massive buffer and then processing it in the main loop
	 * A more robust solution might include realloc or a loop that processes data as it reads,
	 * but 64000 bytes should be more than enough to read in our packets.csv */
	const size_t buffsize = 64000;
	int fd = Open("input/packets.csv", O_RDONLY, 0666);
	char* buffer = (char*)Malloc(buffsize);
	memset(buffer, 0, buffsize);
	while ((Read(fd, buffer, buffsize-1) > 0))
		;
	return buffer;
}

struct Packet* constructPacketsFromData(char* data, int packetCount)
{
	struct Packet* packets = (struct Packet*)Malloc(sizeof(struct Packet) * packetCount);

	for (int entry = 0; entry < packetCount; ++entry) 
	{
		packets[entry] = strToPacket(strsep(&data, "\n"));
	}

	return packets;
}

int main()
{

	struct Host interfaceA, interfaceB;
	struct Host interfaces[2] = {interfaceA, interfaceB};
	struct Router router_1 = constructRouter(1, interfaces, 2);

	char* packetData = getPacketData();
	int packetCount = countLines(packetData);

	struct Packet packets[packetCount];
	memset(packets, 0, sizeof(packets));

	for (int entry = 0; entry < packetCount; ++entry)
	{
		char* packetString = strsep(&packetData, "\n");
		packets[entry] = strToPacket(packetString);
		processRequest(router_1, packets + entry, packetString);
	}

	free(packetData);
	//int connectToServer(char* hostName, char* service);

	return 0;
}
