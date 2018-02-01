# Relable Transfer over UDPtype.

Client-Server communication protocol

Communication Protocol:
	Format of packet header:
		- int frameType: 
			Type 0 only carries the name of the file
			Type 1 carries data
			Type 2 carries data and also flags that this is the end of the data
		- int sqNum:
			Sequence number check that stops from writing redundant data to the file
		- int chunk:
			Number of segmented packets
		- int pLength:
			Real packet length that needs to be written to the file
			This number could possible be smaller than 512
		- char name[NAMELEN]:
			The name of the file
			Only initialized when frameType == 0

	ACK or NAK check:
		Send the header of the received packet back to the client
		if (Received sqNum == Sent sqNum && 
			Received ACK == Sent ACK &&
			Received checkSum == Sent checkSum) ACK
		else NAK

	Length of timeout: 1 second


How it works??
	1) run make command on terminal to buid executable files urft-client, urft-server.
	2) put urft-client and urft-server in seprate folders.
	3) give executable permission to urft-proxy.
	4) run urft-server first, run it like 
		### urft-server <server port>
		$ ./urft-server 20000
	5) run urft- proxy second, run it like 
		### urft-proxy <loss probability> <proxy port> <server ip> <server port>
		$ ./urft-proxy 0.10 10000 127.0.0.1 20000
	6) run urft-client at last, run it like
		### urft-client <proxy ip> <proxy port> <file name>
		$ ./urft-client 10.0.0.1 10000 file1.txt
	7) in order ot send next file kill both server and proxy.
	8) also free the server and proxy port, using $ fuser -k 10000/udp.

Working
	1) Normal Case - without packet loss.
	Client sends the datagram using UDP, and waits for 1s to receive acknowledgement. Once received it sends another datagram and so on.
	Server receives datagram and writes it onto a file and replies the client with plength, sqNum, ack of the datagram.

	2) Withstand Packet Loss.
	Client sends the datagram using UDP, and waits for 1s to receive acknowledgement. Once received it sends another datagram and so on.
	Server receives datagram and writes it onto a file and replies the client with plength, sqNum, ack of the datagram.
	Since there is packet loss from both the sides (client and server), so multiple packets are send one after the other, which introduces the new problem of duplication. This problem is handled by using Timeout function as well as proper identification of packets on server side and acknowledging packets to client side.
