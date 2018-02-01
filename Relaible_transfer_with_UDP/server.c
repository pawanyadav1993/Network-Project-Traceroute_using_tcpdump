#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include "udp.h"


int main(int argc, char *argv[]) {
	if(argc!= 2){
		printf("Usage : %s <proxy port>.\n", argv[0]);
		exit(1);
	}

	int test = 1;

	struct sockaddr_in servAddr, clientAddr;
	int sock, i, recvLen; 
	unsigned int clientAddrLen = sizeof(clientAddr);
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	//create a UDP socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	    errorMessage("Error with creating socket");
	}

	// server address structure
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htons(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	//bind socket to port
	if(bind(sock, (struct sockaddr*)&servAddr, sizeof(servAddr) ) == -1) {
	    errorMessage("Error with bind");
	}

	int chunk,j,k,l,m,n,o,q;
	q = 0;
	o = 0;
	j = 0;
	k = 0;
	m = 0;
	n = 0;
	l = 0;
	chunk = 5;
	int seq[80];
	for(int i = 0; i<80 ; i++)
	{
		seq[i] = 100;
	}
	Frame prevFrame;
	FILE* pFile;
	for (;;) {


		Frame rcvFrame;
		
		//try to receive some data, this is a blocking call
		recvLen = recvfrom(sock, &rcvFrame, sizeof(struct frame), 0, 
			(struct sockaddr *) &clientAddr, &clientAddrLen);

		// deal with input	
		if (recvLen <= 0) {
			printf("There was a packet loss.\n");
			printf("\n");
		} else if (rcvFrame.frameType == 0 && rcvFrame.sqNum == -1 && j == 0 ) {
			// start: only name
			
			printf("File name is %s.\n", rcvFrame.name);
			pFile = fopen(rcvFrame.name,"wb");
			chunk = rcvFrame.chunks;
			j = 1;

		} else if (rcvFrame.frameType == 1 && 
			rcvFrame.sqNum == 0 && o == 0) {
			// first packet but not the last
			o = 1;
			fwrite(rcvFrame.packet, rcvFrame.pLength, 1, pFile);
			prevFrame.sqNum = rcvFrame.sqNum;
			prevFrame.ack = rcvFrame.ack;

		} else if (rcvFrame.frameType == 2 && 
			rcvFrame.sqNum == 0 && m == 0) {
			// first and the last packet
			m = 1;
			q = 1;
			fwrite(rcvFrame.packet, rcvFrame.pLength, 1, pFile);
			fclose(pFile); 	
			if(q == 1)	{
				for (int t = 0; t < chunk; t++)
					{	
						seq[t] = 100;
					}
				j = 0;
				m = 0;
				o = 0;
				n = 0;
				k = 0;
				q = 0;	
				}	
		} else if (rcvFrame.frameType == 1 && 
			rcvFrame.ack != prevFrame.ack && 
			rcvFrame.sqNum == prevFrame.sqNum + 1) {
			// middle packets
			for (int p =0; p < chunk ; p++)
				{	
					if(rcvFrame.sqNum == seq[p])
						{
							l==1;
						}
				}
			if(l == 0)
			{
			fwrite(rcvFrame.packet, rcvFrame.pLength, 1, pFile);
			prevFrame.sqNum = rcvFrame.sqNum;
			prevFrame.ack = rcvFrame.ack;
			seq[k] = rcvFrame.sqNum;
			k++;
			}

		} else if (rcvFrame.frameType == 2 && 
			rcvFrame.ack != prevFrame.ack && 
			rcvFrame.sqNum == prevFrame.sqNum + 1 && n == 0) {
			// last but not the first packet
			n = 1;
			q = 1;
			fwrite(rcvFrame.packet, rcvFrame.pLength, 1, pFile);
			fclose(pFile);
			prevFrame.sqNum = rcvFrame.sqNum;
			prevFrame.ack = rcvFrame.ack;
			if(q == 1)	{
				for (int t = 0; t < chunk; t++)
					{	
						seq[t] = 100;
					}
				j = 0;
				m = 0;
				o = 0;
				n = 0;
				k = 0;
				q = 0;	
				}
		} else {
			printf("Unknown packet is delivered.\n");
			printf("\n");
		}
	

		if (test) {
			printf("Packet received.\n");
			printf("Server: Received seq number - %d.\n", rcvFrame.sqNum);
			printf("Server: Received type %d.\n", rcvFrame.frameType);
			printf("Server: Received pLength - %d.\n", rcvFrame.pLength);
			printf("Server: Received ack - %d.\n", rcvFrame.ack);
			printf("\n");
		}


		//now reply the client with the same data
		if (sendto(sock, &rcvFrame, sizeof(struct frame), 0, 
			(struct sockaddr*) &clientAddr, clientAddrLen) == -1) {
		    errorMessage("Error with sendto()");
		}


	}

    close(sock);
    return 0;
}
