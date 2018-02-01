#include <stdio.h>
#include <stdlib.h>

// print out error msg and exit
void errorMessage(const char *msg) {
	perror(msg);
	exit(1);
}
