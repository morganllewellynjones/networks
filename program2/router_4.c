#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	struct Host interfaceE, interfaceF;
	struct Host interface8004 = initServer(8004);
	struct Host interfaces[3] = {interfaceE, interfaceF, interface8004};
	struct Router router_4 = constructRouter(4, interfaces, 3);
	while (true)
	{
		acceptPackets(router_4, interface8004);
	}

	return 0;
}
