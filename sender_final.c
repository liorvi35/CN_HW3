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

#define IP "127.0.0.1"
#define PORT 8395
#define BUFSIZE 8192
#define AUTH "0000000111111001"

int main()
{
    FILE* file = fopen("msg.txt" , "r");
    if(file == NULL)
    {
        perror("fopen()");
        exit(errno);
    }
    printf("the file is open\n");

    size_t size = 0, half = 0;
    fseek(file , SEEK_SET , SEEK_END);
    size = ftell(file);
    rewind(file);
    half = size/2;

    char *partA =NULL,*partB = NULL;
    partA = (char*)malloc((half+1) * sizeof(char));
    partB = (char*)malloc((size - half+1) * sizeof(char));
    memset(partA , '\0' ,(half+1)*sizeof(char) );
    memset(partB , '\0' ,(size - half+1)*sizeof(char) );

    fread(partA , 1 , half + 1 , file);
    fread(partB , 1 , size - half + 1 , file);
    fclose(file);

    printf("A: the length is: %ld\n" , strlen(partA) + 1);
    printf("B: the length is: %ld\n" , strlen(partB) + 1);


    int sock = 0;
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    char buffer[BUFSIZE] = {'\0'}, d = '\0', *CC = "\0";
    char check[BUFSIZ] = {'\0'};

    sock = socket(AF_INET, SOCK_STREAM, 0); // create the socket
    if(sock < 0)
    {
        perror("socket() failed");
        exit(errno);
    }
    printf("socket created\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) // connect to the receiver
    {
        perror("connect() failed");
        exit(errno);
    }
    printf("connected to server!\n");

    CC = "cubic";
    size_t bytesSent = 0;

    size_t size1 = strlen(partA) + 1; 
    size_t size2 = strlen(partB) + 1; 

    printf("send the first size\n");
    send(sock, &size1, sizeof(size1), 0); //send ok message to the receiver
    printf("sent...");
    printf("send the second size\n");
    send(sock, &size2 , sizeof(size2), 0); //send ok message to the receiver
    printf("sent...");
    
    REPEAT:

    printf("wait for ok message from the receiver...\n"); 
    send(sock, "ok", strlen("ok") + 1, 0); //send ok message to the receiver
    recv(sock, check, BUFSIZE, 0); // receive back ok nessage
    if(strcmp("ok", check) != 0) //check if i got ok
    {
        printf("didnt receiver ok...\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("ok message - continue\n");

    printf("send the first half...\n");
    bytesSent = send(sock, partA, strlen(partA) + 1, 0); //send the first part
    if (bytesSent == 0) {
        printf("peer has closed the TCP connection prior to send().\n");
    } else if (bytesSent < strlen(partA) + 1 ) {
        printf("sent only %ld bytes from the required %ld.\n", bytesSent , strlen(partA) + 1);
    } else {
        printf("message was successfully sent.\n");
    }
    printf("first half has been sent\n");

    printf("wait for authentication...\n");
    recv(sock, buffer, BUFSIZE, 0); //receive the authentication
    if(strcmp(AUTH, buffer) != 0)
    {
        printf("authentication doesn match\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("authentication match\n");

    printf("change the cc Algorithm...\n");
    if(strcmp(CC , "reno") == 0){ //check if change to "reno" or "cubic"
        CC = "cubic"; 
    }
    else{
        CC = "reno";
    }
    setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, &CC, sizeof(CC)); //change the cc Algorithm fron "reno" to "cubic" or from "cubic" to "reno"
    printf("change the cc Algorithm to %s\n" , CC);

    printf("send the second part...\n");
    bytesSent = 0;
    bytesSent = send(sock, partB , strlen(partB) + 1, 0); // send the second part
    if (bytesSent == 0) {
        printf("peer has closed the TCP connection prior to send().\n");
    } else if (bytesSent < strlen(partB) + 1 ) {
        printf("sent only %ld bytes from the required %ld.\n", bytesSent , strlen(partB) + 1);
    } else {
        printf("message was successfully sent.\n");
    }
    printf("second half has been sent\n");

    printf("send again? (y/n)\n"); // ask if send again
    scanf(" %c", &d);
    if(d == 'n')
    {
        send(sock, "exit", strlen("exit") + 1, 0); // if not - send an exit message to the receiver
        printf("exit message has been sent\n");
    }
    else
    {
        printf("change the cc Algorithm...\n");
        if(strcmp(CC , "reno") == 0){ // check if change to "reno" or "cubic"
            CC = "cubic"; 
        }
        else{
            CC = "reno";
        }
        setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, &CC, sizeof(CC)); //change the cc Algorithm
        printf("change the cc Algorithm to %s\n" , CC);
        goto REPEAT; // repeat on the process 
    }

    close(sock); // close the socket

    return 0;
}
