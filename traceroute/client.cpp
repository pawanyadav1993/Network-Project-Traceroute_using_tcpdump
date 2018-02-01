#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include "struct.h"

int main(int argc, char *argv[]) {
	if(argc!= 4){
		printf("Usage : %s <SRC IP> <DST IP> <TCP port>.\n", argv[0]);
		exit(1);
	}

	int test = 1;

	int sock, sockfd;
	char reqbuffer[PCKT_LEN],recvbuffer[PCKT_LEN], *data, *datagram;
	int one = 1;
	const int *val = &one;
	memset(reqbuffer, 0, PCKT_LEN);
	unsigned short ip_len;


	sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(sock < 0) {
  	 	errorMessage("socket() error\n");
	} else{
		printf("socket()-SOCK_RAW and tcp protocol is OK.\n");
	}

	if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
		errorMessage("setsockopt() error\n");
	} else{
   		printf("setsockopt() is OK\n");
	}


	struct timeval tv, tv2, begin, begin2, current, current2, current3;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	tv2.tv_sec = 1;
	tv2.tv_usec = 0;

	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin)); 
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin)); 

	struct sockaddr_in sin, din, sin1;
	socklen_t sinlen, sinlen1;
	bzero(&sin, sizeof(sin));
	bzero(&din, sizeof(din));

	sin1.sin_family = AF_INET;
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	//sin.sin_port = htons(31500);
	din.sin_port = htons(atoi(argv[3]));
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin1.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[2]);

	// The size of the headers
	struct ip *iph = (struct ip *) reqbuffer;
	struct tcphdr *tcph = (struct tcphdr *) (reqbuffer + sizeof(struct ip));
	struct p_head pseudo;

	//data = (char *) (reqbuffer + sizeof(struct ip) + sizeof(struct tcphdr));
	//strcpy(data, "CSCI6760-S11");

	u_long sequence = 0, seq = 1;
	bool goticmp = false, stop = false;
	int ttl, repliedPacketsSent, i;
	double elapsedTime, timed[REQUESTS_PER_TTL];  // variable used to compute the average time of responses
	struct timeval sendTime[REQUESTS_PER_TTL], recvTime[REQUESTS_PER_TTL];;

	//IP Header
	iph->ip_hl = 5;
	iph->ip_v = 4;
	iph->ip_tos = 0;
	iph->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr));
	iph->ip_id = htons(54321);
	iph->ip_off = htons(IP_DF);
	iph->ip_p = IPPROTO_TCP; // TCP
	iph->ip_sum = 0; 
	iph->ip_src.s_addr = inet_addr(argv[1]);
	// Destination IP 
	iph->ip_dst.s_addr = inet_addr(argv[2]);
	// IP checksum calculation
	iph->ip_sum = htons(in_cksum((unsigned short *) reqbuffer, sizeof(struct ip)));
	ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
	
	//TCP Header	
	// The destination port
	tcph->dest = htons(atoi(argv[3]));
	// The Source port
	tcph->source = htons(1024+rand()%(65535-1024));
	tcph->ack_seq = htonl(8000);
	tcph->seq = htonl(1);
	tcph->th_off = 5;
	tcph->syn = 1;
	tcph->window = htons(1460);
	tcph->urg_ptr = 0;
	tcph->check = tcp_checksum((unsigned short)(ip_len - iph->ip_hl*4), (unsigned short*) &(iph->ip_src.s_addr), (unsigned short*)&(iph->ip_dst.s_addr), (char*)tcph);  
	
	//Senders ports
	sin.sin_port = tcph->source;
	sinlen = sizeof(sin);
	sin1.sin_port = htons(1022+rand()%(65535-1024));
	sinlen1 = sizeof(sin1);

	// TCP checksum calculation when using payload
	//pseudo.s_addr = inet_addr(argv[1]);
	//pseudo.d_addr = inet_addr(argv[2]);
	//pseudo.pholder = 0;
	//pseudo.protocol = 6;
	//pseudo.tcp_len = (sizeof(struct tcphdr) + strlen(data));
	//tp_size = sizeof(struct p_head) + sizeof(struct tcphdr) + strlen(data);
	//datagram = (char*) malloc(tp_size);

	//memset(datagram, 0, sizeof(struct p_head) + sizeof(struct tcphdr) + strlen(data));
	//memcpy(datagram , (char *) &pseudo , sizeof (struct p_head));
	//memcpy(datagram + sizeof(struct p_head) , tcph, sizeof(struct tcphdr) + strlen(data));


	// For binding sock
	if(bind(sock, (struct sockaddr *) &sin, sizeof sin) < 0){
		printf("Unable to bind\n");
	}


    //Sending and Receiving packets. 	
	for(ttl=1; ttl<=TTL_LIMIT; ttl++) {
		
		if (stop == true){
			printf("\n\n");
			exit(0);
		}

		if (test) printf("\nTTL = %d\n", ttl);

		// set a new socket listening to ICMP
		sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (sockfd < 0){
			errorMessage("sock:");
		}

		// set ICMP socket receive expiration time as 1 sec
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		// set TCP socket receive expiration time as 1 sec
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv2, sizeof(tv2));  

		sequence = 0;
		repliedPacketsSent = 0;
		elapsedTime = 0.0;
		goticmp = false;

		// Sending packets
		for(i=1; i<=REQUESTS_PER_TTL; i++) {

			iph->ip_ttl = (char)ttl;
			

			tcph->seq = htonl(seq);  // set sequence number, for later identification
			++sequence;
			gettimeofday(&sendTime[(sequence-1) % REQUESTS_PER_TTL], NULL);
			sendto(sock, reqbuffer, ip_len, 0,(struct sockaddr *) &din, sizeof(din));
		}


		gettimeofday(&begin, NULL);

		// listen on the raw tcp socket and break the loop accordingly
		// to be coded
		

		while(repliedPacketsSent < REQUESTS_PER_TTL) {

			gettimeofday(&current3, NULL);

			// wait 1 s for a packet (at most)
			int recv1 = recvfrom(sockfd, recvbuffer, PCKT_LEN, 0, (struct sockaddr *) &sin1, &sinlen1);
			
			// time current
            gettimeofday(&current, NULL);
			
			// for calculating time difference of tcp receive time correction
			{
            	timed[repliedPacketsSent] = (current.tv_sec - current3.tv_sec) * 1000;
            	timed[repliedPacketsSent] += (current.tv_usec - current3.tv_usec)/1000;
            }
			if(recv1 < 0) {


				//if (test) printf("Nothing is received\n");
				if(timeDifference(begin, current) >= TIMEOUT) {
					repliedPacketsSent += 1;
					printf("* ms ");
					continue;
				}
			
			} else {
				// shift bytes to get icmp packet
				struct ip *ip_reply = (struct ip *) recvbuffer;
				struct icmp *icmp_reply = (struct icmp *)(recvbuffer + ip_reply->ip_hl*4);
				struct ip* packet = (struct ip*)(recvbuffer + 28);
				struct tcphdr* tpacket = (struct tcphdr*)(recvbuffer + 48);

				if(sequence - ntohl(tpacket->th_seq) < REQUESTS_PER_TTL){
					// print out information
					if(icmp_reply->icmp_type == ICMP_TIME_EXCEEDED && 
						icmp_reply->icmp_code == ICMP_EXC_TTL) {
			
						if(ntohs(packet->ip_id) == 54321){
							// print ttl and ip address first
							if (repliedPacketsSent == 0) {
								printf("%d  ", ttl);

								printf("  %s  ", inet_ntoa(ip_reply->ip_src));
								
							}
							// print elapsed time on the same row
							
							elapsedTime = (current.tv_sec - sendTime[repliedPacketsSent].tv_sec) * 1000.0;
							elapsedTime += (current.tv_usec - sendTime[repliedPacketsSent].tv_usec) / 1000.0;
							printf("  %.2f ms  ", elapsedTime);

							goticmp = true;
						}
					}
				}  
			}
		
			repliedPacketsSent++;
		}


		gettimeofday(&begin2, NULL);

		if(goticmp == true){

			repliedPacketsSent = 2;

		}else {

			repliedPacketsSent = 0;
		}

		elapsedTime = 0.0;
		bool disp = false; // for printing the destination address with ttl once

		// For checking TCP Reply from Destination..

		while(repliedPacketsSent < REQUESTS_PER_TTL){

			int recvv = recvfrom(sock, recvbuffer, PCKT_LEN, 0, (struct sockaddr *) &sin, &sinlen);
			
			//Current time
			gettimeofday(&current2, NULL);

			struct ip *ipt_reply = (struct ip *) recvbuffer;
			struct tcphdr *tcp_reply = (struct tcphdr *)(recvbuffer + ipt_reply->ip_hl*4);

			if(recvv < 0 && goticmp == false){
				if(timeDifference(begin2, current2) >= TIMEOUT) {
					repliedPacketsSent += 1;
					continue;
				}		

			}else {

			//timed = timeDifference(begin, current);


			if(ipt_reply->ip_p == IPPROTO_TCP){

				if(inet_ntoa(iph->ip_src)==inet_ntoa(ipt_reply->ip_dst) && inet_ntoa(iph->ip_dst)==inet_ntoa(ipt_reply->ip_src)){

					if ((ntohs(tcp_reply->source) == htons(tcph->dest)) && (ntohs(tcp_reply->dest) == htons(tcph->source))){
						if (tcp_reply->th_flags & TH_RST) {

							// print ttl and ip address first
							if (disp == false) {
								printf("\n%d  ", ttl);
								printf("Got TCP RST from Destination  %s  ", inet_ntoa(ipt_reply->ip_src));
								//disp = true;
							}		
							//elapsedTime = ((current3.tv_sec - sendTime[repliedPacketsSent].tv_sec) * 1000.0 )+ ((current2.tv_sec - begin2.tv_sec) * 1000.0 );
							//elapsedTime += ((current3.tv_usec - sendTime[repliedPacketsSent].tv_usec) / 1000.0) + ((current2.tv_usec - begin2.tv_usec)/ 1000.0 );
							//printf("  %.2f ms  (Approx)", (elapsedTime/100));
							stop = true;
						}else if ((tcp_reply->th_flags & TH_SYN) && (tcp_reply->th_flags & TH_ACK)) {
							// print ttl and ip address first
							if (disp == false) {
								printf("\n%d  ", ttl);
								printf("Got TCP SYN/ACK from Destination  %s  ", inet_ntoa(ipt_reply->ip_src));
								//disp = true;
							}		
							//elapsedTime = ((current3.tv_sec - sendTime[repliedPacketsSent].tv_sec) * 1000.0 )+ ((current2.tv_sec - begin2.tv_sec) * 1000.0 );
							//elapsedTime += ((current3.tv_usec - sendTime[repliedPacketsSent].tv_usec) / 1000.0) + ((current2.tv_usec - begin2.tv_usec)/ 1000.0 );
							//printf("  %.2f ms  (Approx)", (elapsedTime/100));
							stop = true;
						}
					}
				}
		
			}
		}
			repliedPacketsSent++;
		}
		
		printf("\n");
		
	}
	
	return 0;
}
