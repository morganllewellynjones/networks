#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	struct Host interface8003 = initServer(8003, SOCK_DGRAM);
	struct Router router_3 = constructRouter(3, &interface8003, 1);

	acceptPackets(router_3, interface8003);

	return 0;
}
