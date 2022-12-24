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

/* defining constants */
#define IP "127.0.0.1" // server's ip address
#define PORT 8395 // connection port
#define BUFSIZE 8192 // size of the buffer
#define AUTH "0000000111111001" // authentication code
#define OK "ok" // ready message
#define CONNECTIONS 50 // number of clients that server can listen simultaneously
