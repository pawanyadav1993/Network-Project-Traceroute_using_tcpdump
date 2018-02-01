all: urft-client urft-server

client.o: client.c udp.h
	g++ -c client.c

server.o: server.c udp.h
	g++ -c server.c

helpers.o: helpers.c udp.h
	g++ -c helpers.c

urft-client: client.o helpers.o
	g++ client.o helpers.o -o urft-client

urft-server: server.o helpers.o
	g++ server.o helpers.o -o urft-server

	rm -rf *.o
