all: client server

client: client.c
	gcc -Wall client.c -o client

proxy: proxy.c
	gcc -Wall proxy.c -o proxy

server: server.c
	gcc -Wall server.c -o server

clean:
	rm client server proxy
