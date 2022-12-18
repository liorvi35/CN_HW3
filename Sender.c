#include "myLib.h"

int main()
{

    /* (1) reading the file */
    FILE *f = fopen("msg.txt", "r");
    if(f == NULL)
    {
        perror("failed to open the file");
        exit(ERR);
    }
    fseek(f, SEEK_SET, SEEK_END);
    long size = ftell(f);
    rewind(f);


    /* (2) creating a TCP connection between the sender and receiver */
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("failed to open socket");
        exit(ERR);
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_port = htons(CONNECTION_PORT);                                              // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP, &serverAddress.sin_addr);  // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0) {
        perror("inet_pton() failed");
        exit(ERR);
    }

// Make a connection to the server with socket SendingSocket.
    int connectResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1) {
        perror("connect() failed");
        // cleanup the socket;
        close(clientSocket);
        exit(ERR);
    }

    printf("connected to server\n");

    /* (3) sending the first part of the file */
    REPEAT:
    size_t halfFIle = size / 2;
    int bytesSent = send(clientSocket, f, halfFIle, 0);
    if(bytesSent == -1 || bytesSent == 0 )
    {
        perror("failed to send the firtst part");
        exit(ERR);
    }

    /* (4) checking for authintication */
    int auth = 0;
    recv(clientSocket, &auth, sizeof(int), 0);
    if(auth != XOR_ID)
    {
        perror("authintication code doesnt match");
        exit(ERR);
    }

    /* (5) change the CC algorithm */
    char *CC = "reno";
    if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == -1)
    {
        perror("cannot change the congestion control");
        exit(ERR);
    }

    /* (6) sending the second part of the file */
    if(send(clientSocket, f + halfFIle, size - halfFIle, 0) == -1)
    {
        perror("failed to send the second part");
        exit(ERR);
    }

    /* (7) user decision */
    char d = '\0';
    printf("send the file again? y/n");
    d = getchar();
    if(d == 'y')
    {
        //ask tuvia
        if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == -1)
        {
            perror("cannot change the congestion control");
            exit(ERR);
        }
        goto REPEAT;
    }
    else
    {
        char *msg = "exit";
        if(send(clientSocket, msg, strlen(msg), 0) == -1)
        {
            perror("error occurred");
            exit(ERR);
        }
        close(clientSocket);
        fclose(f);
    }

    return 0;
}
