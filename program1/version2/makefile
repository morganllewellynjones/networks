all: proxy

proxy: proxy.c server.c http.c io.c
	gcc -Wall -g proxy.c server.c http.c io.c -o proxy

clean:
	rm proxy
