#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	/*struct Host interfaceE, interfaceF;*/
	struct Host interface8004 = initServer(8004, SOCK_DGRAM);
	struct Router router_4 = constructRouter(4, &interface8004, 3);
	acceptPackets(router_4, interface8004);

	return 0;
}
