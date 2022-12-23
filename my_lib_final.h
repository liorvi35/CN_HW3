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

#define IP "127.0.0.1" // the server's ip address
#define PORT 8395 // the connection port
#define BUFSIZE 8192 // the size of the buffer
#define AUTH "0000000111111001" // the authentication code
#define OK "ok" // ready message
