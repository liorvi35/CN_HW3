#include "myLib.h"

int main()
{
    /* (1) reading the file */
    long size = 0;
    FILE *f = fopen("msg.txt", "r");
    if(f == NULL) // checking if file opened
    {
        fclose(f);
        perror("fopen() failed");
        exit(errno);
    }
    printf("Success: file is opened!\n");
    // getting the size in bytes of the file:
    fseek(f, SEEK_SET, SEEK_END);
    size = ftell(f);
    rewind(f);


    /* (2) creating a TCP connection between the sender and receiver */
    // creating socket:
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == ERR) // checking if socket created
    {
        fclose(f);
        close(clientSocket);
        perror("socket() failed");
        exit(errno);
    }
    printf("Success: socket is created!\n");
    // checking if port is free to use:
    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret == ERR) 
    {
        perror("setsockopt() failed");
        exit(errno);
    }
    printf("Success: port is usable!\n");
    // ?
    struct sockaddr_in serverAddress = {0};
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_port = htons(CONNECTION_PORT);                                             
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP, &serverAddress.sin_addr);
    if (rval == ERR)
    {
        fclose(f);
        close(clientSocket);
        perror("inet_pton() failed");
        exit(ERR);
    }
    // making connection: 
    int connectResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == ERR)
    {
        fclose(f);
        close(clientSocket);
        perror("connect() failed");
        exit(errno);
    }
    printf("Success: connected to receiver!\n");

    /* (3) sending the first part of the file */
    REPEAT:
    size_t halfFIle = size / 2;
    int bytesSent = send(clientSocket, f, halfFIle, 0);
    if(bytesSent == -1 || bytesSent == 0 ) // checking if first part has been sent
    {
        fclose(f);
        close(clientSocket);
        perror("send() failed");
        exit(errno);
    }
    printf("Success: first half of file has been sent!\n");

    /* (4) checking for authintication */
    int auth = 0;
    recv(clientSocket, &auth, sizeof(int), 0);
    if(auth != XOR_ID) // checking if uthintication code match
    {
        fclose(f);
        close(clientSocket);
        perror("authintication code doesnt match");
        exit(ERR);
    }
    printf("Success: authintication code match!\n");

    /* (5) change the CC algorithm */
    char *CC = "reno";
    if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == ERR)
    {
        fclose(f);
        close(clientSocket);
        perror("setsockop() failed");
        exit(errno);
    }
    printf("Success: CC has been changed!\n");

    /* (6) sending the second part of the file */
    if(send(clientSocket, f + halfFIle, size - halfFIle, 0) == -1)
    {
        fclose(f);
        close(clientSocket);
        perror("send() failed");
        exit(errno);
    }
    printf("Success: second half of file has been sent!\n");

    /* (7) user decision */
    char d = '\0';
    printf("send the file again? y/n");
    d = getchar();
    if(d == 'y')
    {
        //ask tuvia
        if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == -1)
        {
            fclose(f);
            close(clientSocket);
            perror("setsockop() failed");
            exit(errno);
        }
        printf("Success: repeting process!\n");
        goto REPEAT;
    }
    else
    {
        char *msg = "exit";
        if(send(clientSocket, msg, strlen(msg), 0) == -1)
        {
            fclose(f);
            close(clientSocket);
            perror("send() failed");
            exit(errno);
        }
        fclose(f);
        close(clientSocket);
        printf("Success: closing connection!\n");
    }

    return 0;
}
