#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	struct Host interface8006 = initServer(8006);
	struct Router router_6 = constructRouter(6, &interface8006, 3);
	while (true)
	{
		acceptPackets(router_6, interface8006);
	}

	return 0;
}
