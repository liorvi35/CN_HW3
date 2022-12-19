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
#define CONNECTION_PORT 5894
#define SERVER_IP "127.0.0.1"
#define ID_Lior 0000011011100011 
#define ID_Yoad 0000011100011010
#define XOR_ID 0000000111111001
#define BUFFER_SIZE 10
#define MAX_CONNECTIONS 3
#define ERR -1

