#include <stdio.h>
#include "router.h"
#include "host.h"
#include "io.h"


int main()
{
	/*struct Host interfaceC, interfaceD;*/
	struct Host interface8002 = initServer(8002, SOCK_DGRAM);
	struct Router router_2 = constructRouter(2, &interface8002, 3);
	acceptPackets(router_2, interface8002);

	return 0;
}
