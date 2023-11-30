#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	struct Host interface8005 = initServer(8005, SOCK_DGRAM);
	struct Router router_5 = constructRouter(5, &interface8005, 3);
	acceptPackets(router_5, interface8005);

	return 0;
}
