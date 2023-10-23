all: proxy

proxy: proxy.c server.c
	gcc -Wall proxy.c server.c -o proxy -lpthread

clean:
	rm proxy
