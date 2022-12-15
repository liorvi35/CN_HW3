#include "myLib.h"

int main()
{
    /* (1) creating a TCP connection between the sender and receiver */
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("failed to open socket");
        exit(ERR);
    }

    /* (2) getting connection from the sender */
    struct sockaddr_in addr = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);
    if(bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("cannot connect to sender");
        close(serverSocket);
        exit(ERR);
    }
    if (listen(serverSocket, MAX_CONNECTIONS) == -1)
    {
        perror("listening failed");
        close(serverSocket);
        exit(ERR);
    }
    int conn = accept(serverSocket, NULL, NULL);


    /* (3) reveiving the first part */
    char buffer[BUFFER_SIZE] = {'\0'};
    if(recv(serverSocket, buffer, sizeof(buffer), 0) == -1)
    {
        perror("error occurred");
        close(serverSocket);
        exit(ERR);
    }

    /* (4) measuring the time it took to receive the first part */

    /* (5) saving the time */

    /* (6) sending back the authentication */
    int auth = XOR_ID;
    send(serverSocket, &auth, sizeof(auth));

    /* (7) receiving the second part */

    /* (8) measuring the time it took to receive the second part */

    /* (9) saving times */

    /* (10) if getting exit message */
    char buffet[BUFFER_SIZE] = {'\0'};
    if(recv(serverSocket, buffet, sizeof(buffer), 0) == -1)
    {
        perror("error occurred");
        close(serverSocket);
        exit(ERR);
    }
    if(strcmp(buffer, "exit"))
    {

    }
    
    return 0;
}

