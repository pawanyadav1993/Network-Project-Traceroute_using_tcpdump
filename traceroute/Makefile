all: tcp_traceroute

client.o: client.c struct.h
	g++ -c client.c

helpers.o: helpers.c struct.h
	g++ -c helpers.c

tcp_traceroute: client.o helpers.o
	g++ client.o helpers.o -o tcp_traceroute

	rm -rf *.o
