#include "myLib.h"

int main()
{
    // for saving the time 
    int* timesave = malloc(2*sizeof(int));
    int size = 2;

    /* (1) creating a TCP connection between the sender and receiver */
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("failed to open socket");
        exit(ERR);
    }

    // checking if port is free to use:
    int enableReuse = 1;
    int ret = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret == ERR) 
    {
        perror("setsockopt() failed");
        exit(errno);
    }
    printf("Success: port is usable!\n");

    /* (2) getting connection from the sender */
    // create a struct
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // any IP at this port (Address to accept any incoming messages)
    serverAddress.sin_port = htons(CONNECTION_PORT);  // network order (makes byte order consistent)
    
    // Bind the socket to the port with any IP at this port
    int bindResult = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1) {
        // close the socket
        close(serverSocket);
        perror("Bind failed");
        exit(errno);
    }
    printf("Bind() success\n");

    // Make the socket listening
    // number of concurrent connections
    int listenResult = listen(serverSocket, 3);
    if (listenResult == -1) {
        perror("listen() failed");
        // close the socket
        close(serverSocket);
        exit(errno);
    }
        printf("listen() success\n");

    // Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    int rec = 0; // how much part of the file im already received
    int files = 0; //how much files i receive

    while (1) {
        printf("Start loop\n");
        struct timeval current_time; // save the current time
        int curtime = gettimeofday(&current_time, NULL);

        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
              perror("listen failed");
                // close the sockets
                close(serverSocket);
                exit(errno);
        }

        printf("A new client connection accepted\n");

        /* (10) if getting exit message */ 
        if(rec >= 2){ // i receive all the file
            printf("%d\n" , rec);
            char buffer[BUFFER_SIZE] = {'\0'};
            if(recv(serverSocket, buffer, sizeof(buffer), 0) == -1)
            {
                perror("error occurred");
                close(serverSocket);
                exit(ERR);
            }

            if(strcmp(buffer, "exit")) // check if we need to exit 
            {
                break;   
            }
            else{ // need to get a new file
                rec = 0; 
                files++;
                continue;
            }
        }

        /* (3) reveiving the first part 
        or
       (7) receiving the second part */
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == -1) {
            perror("error occurred2");
            // close the sockets
            close(serverSocket);
            close(clientSocket);
            exit(ERR);
        }

        /* (4) measuring the time it took to receive the first part + (5) saving the time 
         or   
         (8) measuring the time it took to receive the second part + (9) saving times */
        int aftertime = gettimeofday(&current_time, NULL);
        int time = aftertime - curtime;
        if(rec==0 && files>1){
            timesave = realloc(timesave , size*sizeof(int) + 2 * sizeof(int));
            size++;
        }
        timesave[files + rec] = time;

        if(rec == 0){
            printf("Success: first half of file has been receive!\n");
        }
        else{
            printf("Success: second half of file has been receive!\n");
        }
        rec++; 

        if(rec == 1){
            /* (6) sending back the authentication */ 
            char *message = XOR_ID;
            int messageLen = strlen(message) + 1;

            int bytesSent = send(clientSocket, message, messageLen, 0);
            if (bytesSent == -1) {
                perror("send() failed");
                close(serverSocket);
                close(clientSocket);
                exit(ERR);
            } 
            else if (bytesSent == 0) {
                printf("peer has closed the TCP connection prior to send().\n");
            } 
            else {
                printf("authentication was successfully sent.\n");
            }
        }
        printf("End loop\n");

    }


    /* (10) if getting exit message */ 
    int avg = 0;
    for(int i=0; i<size*2; i++ ){
        printf("the time of the %d part is%d\n" , i , timesave[i]);
        avg+= timesave[i];
    }
    avg /= (size*2);
   
    return 0;
}

