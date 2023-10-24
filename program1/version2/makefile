all: proxy

proxy: proxy.c host.c http.c io.c host.h http.h io.h
	gcc -Wall -g proxy.c host.c http.c io.c -o proxy

clean:
	rm proxy
