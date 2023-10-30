all: proxy

proxy: proxy.c host.c io.c host.h io.h
	gcc -Wall -g proxy.c host.c io.c -o proxy

clean:
	rm proxy
	rm -rf cache
