#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	struct Host interfaceC, interfaceD;
	struct Host interface8002 = initServer(8002);
	struct Host interfaces[3] = {interfaceC, interfaceD, interface8002};
	struct Router router_2 = constructRouter(2, interfaces, 3);
	while (true)
	{
		struct Host client = acceptClient(interface8002);

		struct ThreadData threadData;
		threadData.router = router_2;
		threadData.client = client;

		pthread_t clientThread;
		pthread_create(&threadData, NULL, receiveRequest, (void*)&client);
	}

	return 0;
}
