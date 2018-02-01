#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define PCKT_LEN 512
#define buffer 128
#define TIMEOUT 1000 
#define TTL_LIMIT 30
#define REQUESTS_PER_TTL 3
#define ICMP_HEADER_LEN 8

// all functions
void errorMessage(const char *msg);
unsigned short in_cksum(unsigned short *addr, int len);
double timeDifference(struct timeval start, struct timeval end);
unsigned short tcp_checksum(unsigned short len_tcp,
                            unsigned short *src_addr, unsigned short *dest_addr,
                            char *tcp_pkt);

struct p_head
{
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t pholder;
    u_int8_t protocol;
    u_int16_t tcp_len;
};
