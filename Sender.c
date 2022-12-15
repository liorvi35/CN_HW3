#include "myLib.h"

int main()
{

    /* (1) rading the file */
    FILE *f = fopen("msg.txt", "r");
    if(f == NULL)
    {
        perror("failed to open the file");
        exit(ERR);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *b = malloc(size);
    if(b == NULL)
    {
        perror("failed to allocate memory");
        exit(ERR);
    }
    fclose(f);

    /* (2) creating a TCP connection between the sender and receiver */
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("failed to open socket");
        exit(ERR);
    }

    /* (3) sending the first part of the file */
    REPEAT:
    size_t halfFIle = size / 2;
    if(send(clientSocket, b, halfFIle, 0) == -1)
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
    char *CC = "reno"
    if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == -1)
    {
        perror("cannot change the congestion control");
        exit(ERR);
    }

    /* (6) sending the second part of the file */
    if(send(clientSocket, b + halfFIle, size - halfFIle, 0) == -1)
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
        if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC)) == -1)
        {
            perror("cannot change the congestion control");
            exit(ERR);
        }
        goto REPEAT;
    }
    else
    {
        free(b);
        char *msg = "exit";
        if(send(clientSocket, msg, strlen(msg), 0) == -1)
        {
            perror("error occurred");
            exit(ERR);
        }
        close(clientSocket);
    }

    return 0;
}