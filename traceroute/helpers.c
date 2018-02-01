#include <stdio.h>
#include <stdlib.h>
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

// print out error msg and exit
void errorMessage(const char *msg) {
	perror(msg);
	exit(1);
}

unsigned short in_cksum(unsigned short *addr, int len)
{
	int	nleft = len;
	int	sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

		/* 4mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w ;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;				
	return(answer);
}

unsigned short tcp_checksum(unsigned short len_tcp,
                            unsigned short *src_addr, unsigned short *dest_addr,
                            char *tcp_pkt) {

  unsigned short prot_tcp = 6;
  unsigned short pkt_len = len_tcp;
  long sum;
  int i;
  sum = 0;


   // Check if the tcp length is even or odd.  Add padding if odd.
   if((pkt_len % 2) == 1){
    pkt_len += 1; // increase length to make even.
   }

   char pkt[pkt_len];
   memset(pkt,0,pkt_len);
   memcpy(pkt,tcp_pkt,len_tcp);
   unsigned short *buff = (unsigned short*)pkt;

   // add the pseudo header 
   sum += ntohs(src_addr[0]);
   sum += ntohs(src_addr[1]);
   sum += ntohs(dest_addr[0]);
   sum += ntohs(dest_addr[1]);
   sum += pkt_len; // already in host format.
   sum += prot_tcp; // already in host format.

   // calculate the checksum for the tcp header and payload
   // len_tcp represents number of 8-bit bytes, 
   //  we are working with 16-bit words so divide pkt_len by 2. 
   for(i=0;i<(pkt_len/2);i++){
      sum += ntohs(buff[i]);
   }

   // keep only the last 16 bits of the 32 bit calculated sum and add the carries
   sum = (sum & 0xFFFF) + (sum >> 16);
   sum += (sum >> 16);

   // Take the bitwise complement of sum
   sum = ~sum;

  return htons(((unsigned short) sum));
}

double timeDifference(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_usec - start.tv_usec)/1000.0;
}
