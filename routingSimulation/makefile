CC = gcc
CFLAGS = -Wall -g
LINKS = -lpthread
ROUTERS = router_1 router_2 router_3 router_4 router_5 router_6

all: $(ROUTERS)

router_1: router_1.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

router_2: router_2.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

router_3: router_3.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

router_4: router_4.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

router_5: router_5.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

router_6: router_6.c router.c router.h host.c host.h io.c io.h
	$(CC) $(CFLAGS) $< router.c host.c io.c -o $@ $(LINKS)

run:
	./router_1

clean:
	rm router_? *.txt
