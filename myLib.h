/* including libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* constants */
#define CONNECTION_PORT 5060 
#define SERVER_IP "127.0.0.1"
#define ID_Lior 1763 
#define ID_Yoad 1818
#define XOR_ID 505
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 3
#define ERR