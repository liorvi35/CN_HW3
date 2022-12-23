#include "myLib.h" // including our library

int main()
{

    /*
        Variables Dictionary:
        (01) int client_socket: new socket after accepting the sender
        (02) int server_socket: listener socket for connections 
        (04) int last: the last position of array
        (05) int file: the amount of file received
        (06) int pf: the current file, use just for printing 
        (07) int i: loop counter 
        (00) int er: for enable reusing ip and port
        (00) int sizeTime: dynamic size of timeArray 
        (14) int *timeArray: array for saving times
        (08) struct sockaddr_in server_addr: listener socket's settings for storing ip and port in IPV4 connection
        (09) struct sockaddr_in client_addr: accept's socket's settings
        (10) socklen_t addr_size: the size of the client's address
        (11) char buffer[]: use to recevie the files
        (12) char *CC: the current congestion control algorithm
        (15) double avg1: the average of receivng first part - cubic
        (16) double avg2: the average of receivng second part - reno
        (17) struct timeval start: the current time before receving the file
        (18) struct timeval end: the current time after receiving the file
        (19) struct timeval calc: for calculation the diffrence in times 
        (20) size_t byteRecv: the amount of byte we received in each part
        (21) size_t sizePart1: the size of part 1
        (22) size_t sizePart2:  the size of part 2
        (23) size_t sum: sum of bytes in each receive
    */

    /* declaring and reseting variables */
    int client_sock = 0, server_sock = 0, last = 0, file = 1, pf = 0, i = 0, er = 1, sizeTime = 2 * file;
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    memset(&client_addr, '\0', sizeof(client_addr));
    socklen_t addr_size = 0;
    char buffer[BUFSIZE] = {'\0'}, *CC = "\0";
    double avg1 = 0, avg2 = 0;
    struct timeval start, end, calc;
    memset(&start, 0, sizeof(start));
    memset(&start, 0, sizeof(end));
    memset(&start, 0, sizeof(calc));
    size_t byteRecv = 0, sizepart1 = 0, sizepart2 = 0, sum = 0;
    int *timeArray = malloc(sizeTime * sizeof(int));
    if(timeArray == NULL) // checkig if memmory allocated
    {
        perror("malloc() failed");
        free(timeArray);
        exit(errno);
    }
    memset(timeArray, 0, sizeTime * sizeof(int));

    /* (1) creating TCP connection*/
    server_sock = socket(AF_INET, SOCK_STREAM, 0); // creating the listener socket
    if(server_sock <= 0) // checking if socket created
    {
        perror("socket() failed");
        close(server_sock);
        free(timeArray)
        exit(errno);
    }
    printf("socket created!\n");

    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &er, sizeof(er)) < 0) // // checking if ip and port are reusable
    {
        perror("setsockopt() failed");
        close(server_sock);
        free(timeArray)
        exit(errno);
    }

    server_addr.sin_family = AF_INET; //setting up socket's used protocol, port and ip
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // binding socket with settings
    {
        perror("bind() failed");
        close(server_sock);
        free(timeArray)
        exit(errno);
    }
    printf("socket bound!\n");

    if(listen(server_sock, CONNECTIONS) < 0) //listen to incoming connections
    {
        perror("listen() failed");
        close(server_sock);
        free(timeArray)
        exit(errno);
    } 
    printf("waiting for connection...\n");

    /* (2) getting connection from the sender */
    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size); //accept a connection
    if(client_sock <= 0) // checking if accepted
    {
        perror("bind() failed");
        close(client_sock);
        close(server_sock);
        free(timeArray)
        exit(errno);
    }
    printf("sender connected!\n");
    
    CC = "cubic"; // the defultive congestion control algorithm

    byteRecv = recv(client_sock , &sizepart1 , sizeof(size_t) , 0); // receiving the size of first part of file
    if(byteRecv < 0) // checkig if received
    {
        perror("send() failed");
        close(client_sock);
        close(server_sock);
        free(timeArray)
        exit(errno);
    }
    else if(byteRecv == 0)
    {
        printf("peer has closed the TCP connection prior to send()\n");
        close(client_sock);
        close(server_sock);
        free(timeArray)
        exit(EXIT_FAILURE);
    }

    byteRecv = recv(client_sock , &sizepart2 , sizeof(size_t) , 0); // receive the size of second part
    if(byteRecv < 0) // checkig if received
    { 
        perror("send() failed");
        close(client_sock);
        close(server_sock);
        free(timeArray)
        exit(errno);
    }
    else if(byteRecv == 0)
    {
        printf("peer has closed the TCP connection prior to send()\n");
        close(client_sock);
        close(server_sock);
        free(timeArray)
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        memset(buffer, '\0', BUFSIZE);
        byteRecv = recv(client_sock, buffer, BUFSIZE, 0); // receiving an ok or exit message
        if(byteRecv < 0) // checkig if received
        {
            perror("send() failed");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(errno);
        }
        else if(byteRecv == 0)
        {
            printf("peer has closed the TCP connection prior to send()\n");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(EXIT_FAILURE);
        }

        /* (10) if received exit message */
        else if(strcmp(buffer, "exit") == 0)
        {
            printf("exit message has been received!\n");
            printf("\n\n");
            printf("###################\n");
            printf("\n");
            printf("collected DATASET is:\n\n");

            /* (10-i) printing out times */
            for(i = 0; i < sizeTime - 2; i++)
            {
                if(i % 2 == 0)
                {
                    pf++;
                    avg1 += timeArray[i];
                }
                else
                {
                    avg2 += timeArray[i];
                }
                printf("time to receive part %d of file %d is %d [us]\n", (i % 2 == 0 ? 1 : 2), pf, timeArray[i]);
            }

                printf("\n");
                printf("###################\n");
                printf("\n");
                printf("the times of avg part: \n");
                avg1 /= (file - 1);
                avg2 /= (file - 1);
                /* (10-iii) printing the average times */
                printf("the average of sending first file - by \"cubic\" - is: %.3f [us]\n", avg1);
                printf("the average of sending second file - by \"reno\" - is: %.3f [us]\n", avg2);
                printf("\n");
                printf("###################\n");
            break;
        }

        if(send(client_sock, OK, strlen(OK) + 1, 0) <= 0) // sending ok message to the Sender
        {
            perror("send() failed");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(errno);
        }

        /* (3) receiving the first part */
        memset(buffer, '\0', BUFSIZE); // clearing buffer
        memset(&start, 0, sizeof(start)); // clearing times
        memset(&start, 0, sizeof(end));
        memset(&start, 0, sizeof(calc));
        sum = 0;
        gettimeofday(&start, NULL); // get start time 
        while (sum != sizepart1) // receive the first part
        {   byteRecv = recv(client_sock, buffer, BUFSIZE, 0);
            if(byteRecv < 0)
            {
                perror("send() failed");
                close(client_sock);
                close(server_sock);
                free(timeArray)
                exit(errno);
            }
            else if(byteRecv == 0)
            {
                printf("peer has closed the TCP connection prior to send()\n");
                close(client_sock);
                close(server_sock);
                free(timeArray)
                exit(EXIT_FAILURE);
            }
            sum += byteRecv;
        }
        gettimeofday(&end, NULL); // get end time
        printf("first message has been received!\n");

        /* (4) measuring the time */
        timersub(&end, &start, &calc); // calculate the diffrence

        /* (5) saving the time */
        timeArray[last] = ((calc.tv_sec * 1000000.0) + calc.tv_usec); //save the time in micro-seconds
        last++;

        /* (6) sending back the authentication */
        if(send(client_sock, AUTH, strlen(AUTH) + 1, 0) <= 0) //send the authentication
        {
            perror("send() failed");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(errno);
        }
        else
        {
            printf("authentication has been sent!\n"); 
        }

        /* (6-i) changing the CC algorithm */
        CC = (strcmp(CC, "reno") == 0 ? "cubic" : "reno"); // determinating congestion control algorithm
        if(setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, CC, sizeof(CC)) < 0) //changing congestion control algorithm
        {
            perror("setsockopt() failed");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(errno);
        }
        printf("CC algorithm has been changed to: \"%s\"\n", CC);

        /* (7) receiving the second part */
        memset(buffer, '\0', BUFSIZE); // clearing buffer
        memset(&start, 0, sizeof(start)); // clearing times
        memset(&start, 0, sizeof(end));
        memset(&start, 0, sizeof(calc));
        sum = 0;
        gettimeofday(&start, NULL); //set the start time
        while (sum != sizepart2)
        {
            byteRecv = recv(client_sock, buffer, BUFSIZE, 0);
            if(byteRecv < 0)
            {
                perror("send() failed");
                close(client_sock);
                close(server_sock);
                free(timeArray)
                exit(errno);
            }
            else if(byteRecv == 0)
            {
                printf("peer has closed the TCP connection prior to send()\n");
                close(client_sock);
                close(server_sock);
                free(timeArray)
                exit(EXIT_FAILURE);
            }
            sum += byteRecv;
        }
        gettimeofday(&end, NULL);
        printf("second message has been received!\n");

        /* (8) measuring the time */
        timersub(&end, &start, &calc);

        /* (9) saving the time*/
        timeArray[last] = ((calc.tv_sec * 1000000.0) + calc.tv_usec); // set the end time 
        last++;
        file++;
        sizeTime = 2 * file;
        timeArray = (int*)realloc(timeArray, sizeTime * sizeof(int));

        CC = (strcmp(CC, "reno") == 0 ? "cubic" : "reno"); // determinating congestion control algorithm
        if(setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, CC, sizeof(CC)) < 0) //changing congestion control algorithm for sync
        {
            perror("setsockopt() failed");
            close(client_sock);
            close(server_sock);
            free(timeArray)
            exit(errno);
        }
        printf("CC algorithm has been changed to: \"%s\"\n", CC);

    }
    
    // closing connection
    close(client_sock); // freeing recourses and closing socket
    close(server_sock);
    free(timeArray)

    exit(EXIT_SUCCESS);
}
