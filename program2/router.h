#ifndef ROUTER_H
#define ROUTER_H

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "io.h"
#include "host.h"

#define TABLE_SIZE 64

struct ForwardTable
{
	unsigned int entries;
	unsigned int dest[TABLE_SIZE];
	unsigned int netmask[TABLE_SIZE];
	unsigned int gateway[TABLE_SIZE];
	char* interface[TABLE_SIZE];
};

struct Router
{
	char id;
	struct ForwardTable forwardTable;
	struct Host* interfaces;
};

struct Packet
{
	unsigned int src;
	unsigned int dest;
	char* payload;
	short ttl;
};

struct ThreadData
{
	struct Router router;
	struct Host client;
};

bool isIPString(char* ipstring);
unsigned int IPtoInt(char* ipstring);
struct Packet strToPacket(char* data);
void acceptPackets(struct Router router, struct Host interface);
void* receiveRequest(void* args);
void processRequest(struct Router router, struct Packet* packet, char* rawPacketString);
char* match(struct ForwardTable table, unsigned int packetDest);
struct ForwardTable constructForwardTable();
void getForwardTable(struct ForwardTable* table, char* filename);
struct Router constructRouter(char id, struct Host* interfaces, short interfaceCount);
void printForwardTable(struct ForwardTable table);

#endif
