#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define BUFLEN 512
#define NAMELEN 100

// all functions
void errorMessage(const char *msg);

// data structure
typedef struct frame {
    int frameType; //START:0, SEQ:1 END:2
    int sqNum;
    int chunks;
    int ack; // switch between 0 and 1
    int pLength; // packet pLength
    char name[NAMELEN];
    char packet[BUFLEN];
} Frame;
