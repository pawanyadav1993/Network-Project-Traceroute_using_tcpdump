#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include "udp.h"

extern int errno;

int main(int argc, char *argv[]) {
	if(argc!= 4){
		printf("Usage : %s <proxy ip><proxy port><file name>.\n", argv[0]);
		exit(1);
	}

	int test = 1;

	struct sockaddr_in servAddr;
	int sock, i;
	unsigned int servAddrLen = sizeof(servAddr);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	// create socket
	if ( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		errorMessage("Error with creating socket");
	}

	// server address structure
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	// read file
	FILE *file = fopen(argv[3], "rb");  
	fseek(file, 0, SEEK_END);
	long fileLen = ftell(file);  
	char *buffer = (char*) malloc(fileLen);  
	fseek(file, 0, SEEK_SET);
	fread(buffer, 1, fileLen, file);  
	fclose(file);

	// calculate chunks
	int cap = fileLen / BUFLEN + 1;

	// loop
	int recvLen = -1;
	for (int n = -1; n < cap; n++) {
		// put together a packet
		Frame sendFrame;
		sendFrame.sqNum = n;
		sendFrame.ack = n%2;
		sendFrame.chunks = cap;

		if (n == -1) {
			// start --- only feed the file name
			memset(sendFrame.name, '\0', NAMELEN);
			strcpy(sendFrame.name, argv[3]);
			sendFrame.frameType = 0;
			sendFrame.pLength = 10;
		} else if (n == cap-1) {
			// end
			sendFrame.frameType = 2;
			sendFrame.pLength = fileLen - BUFLEN*n;
			memset(sendFrame.packet, '\0', BUFLEN);
			memcpy(sendFrame.packet, buffer + BUFLEN*n, fileLen - BUFLEN*n);
		} else {
			// middle packets
			sendFrame.frameType = 1;
			sendFrame.pLength = BUFLEN;
			memset(sendFrame.packet, '\0', BUFLEN);
			memcpy(sendFrame.packet, buffer + BUFLEN*n, BUFLEN);
		}

		if (test) {
			printf("Client: Send packet.\n");
			printf("Client: packet seq number %d.\n", sendFrame.sqNum);
			printf("Client: packet ack %d.\n", sendFrame.ack);
			printf("Client: packet type %d.\n", sendFrame.frameType);
			printf("Client: packet pLength %d.\n", sendFrame.pLength);
			printf("\n");
		}

		// send a packet
		while(1) {
			// set timeout
			if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, 
				(const char*)&tv,sizeof(struct timeval)) < 0) {
				errorMessage("Can not set time out");
			}
			
			if (sendto(sock, &sendFrame, sizeof(struct frame), 0, 
				(struct sockaddr *) &servAddr, servAddrLen)==-1) {
				errorMessage("Error with sendto().");
			}

			Frame recvFrame;
			recvLen = recvfrom(sock, &recvFrame, sizeof(struct frame), 0, 
				(struct sockaddr *)&servAddr, &servAddrLen);

			if (recvLen <= 0) {
				printf("Did not receive anything from the server.\n");
				printf("\n");
			} else if (recvFrame.sqNum == sendFrame.sqNum && 
				recvFrame.ack == sendFrame.ack) {
				printf("Packet received.\n");
				printf("Client: Packet seq number %d.\n", recvFrame.sqNum);
				printf("Client: Packet ACK %d.\n", recvFrame.ack);
				printf("Client: Packet type %d.\n", recvFrame.frameType);
				printf("\n");
				break;
			} else{
				printf("Wrong packet received.\n");
				printf("Client: Packet seq number %d.\n", recvFrame.sqNum);
				printf("Client: Packet ACK %d.\n", recvFrame.ack);
				printf("Client: Packet type %d.\n", recvFrame.frameType);
				printf("\n");
			}
		}
	}

	free(buffer);
	close(sock);
	exit(0);
	return 0;
}
