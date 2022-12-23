#include "myLib.h"

int main()
{

    /*
        client_socket: the socket that use for communication
        server_socket: lestening socket 
        n:bind
        last: the last position of array
        file: the amount of file received
        pf: the currect file
        j: use for loop 
        server_addr: the address of the server
        client_addr:the address of the client
        addr_size: the size of the client's address
        buffer: use to recevie the file
        CC: the current cc algorithm
        check: save the ok that the sender send
        timeArray: array for saving time
        avg1: the average of receivng first part - cubic
        avg2: the average of receivng second part - reno
        srart: the current time before receving the file
        end: the current time after receiving the file
        calc: start - end 
        byteRecv: the amount of byte we received in each part
        sizePart1: the size of part 1
        sizePart2:  the size of part 2
    */

    int client_sock = 0, server_sock = 0, n = 0, last = 0, file = 1, pf = 0, j=0;
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    memset(&client_addr, '\0', sizeof(client_addr));
    socklen_t addr_size = 0;
    char buffer[BUFSIZE] = {'\0'}, *CC = "\0", check[BUFSIZ] = {'\0'};
    double timeArray[1000] = {0.0}, avg1 = 0, avg2 = 0;
    struct timeval start, end, calc;
    memset(&start, 0, sizeof(start));
    memset(&start, 0, sizeof(end));
    memset(&start, 0, sizeof(calc));
    size_t byteRecv = 0, sizepart1 = 0, sizepart2 = 0;

    server_sock = socket(AF_INET, SOCK_STREAM, 0); //create the socket
    if(server_sock < 0)
    {
        perror("socket() failed");
        exit(errno);
    }
    printf("socket created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); //bind socket
    if(n < 0)
    {
        perror("bind() failed");
        exit(errno);
    }
    printf("socket binded\n");

    listen(server_sock, 1); //listen to incoming connections
    printf("waiting for connection...\n");

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size); //accept a connection
    printf("client connected!\n");
    
    CC = "cubic"; // the defult cc 
    printf("wait for the size of part one...\n");
    recv(client_sock , &sizepart1 , sizeof(size_t) , 0); // receive the size of first part
    printf("size receive: %ld\n" , sizepart1);

    printf("wait for the size of part two...\n");
    recv(client_sock , &sizepart2 , sizeof(size_t) , 0); // receive the size of second part
    printf("size receive: %ld\n" , sizepart2);

    while (1)
    {
        memset(check, '\0', BUFSIZE);
        recv(client_sock, check, BUFSIZE, 0); //receive an ok/exit message 
        if(strcmp(check, "exit") == 0) // if received exit
        {
            printf("exit message received\n");
            printf("\n\n");
            printf("###################\n");
            printf("\n");
            printf("the times of each part: \n");


            for(j = 0; j < last ; j++)//print the times
            {
                if(j % 2 == 0)
                {
                    pf++;
                    avg1 += timeArray[j];
                }
                else
                {
                    avg2 += timeArray[j];
                }
                printf("time to receive part %d of file %d is %f [ms]\n", (j % 2 == 0 ? 1 : 2), pf, timeArray[j]);
            }

                printf("\n\n");
                printf("###################\n");
                printf("\n");
                printf("the times of avg part: \n");

                avg1 /= last-1;
                avg2 /= last-1;
                printf("the avg of send the first file - by cubic - is: %f\n" , avg1);
                printf("the avg of send the second file - by reno - is: %f\n" , avg2);

                printf("\n\n");
                printf("###################\n");

            break;
        }

        printf("send ok message...\n");
        send(client_sock, OK, strlen(OK) + 1, 0); // send ok message to the Sender
        printf("ok message has been sent\n");

        memset(buffer, '\0', BUFSIZE);
        byteRecv = 0;
        gettimeofday(&start, NULL); //set the start time 

        while (byteRecv != sizepart1)
        {
            byteRecv += recv(client_sock, buffer, BUFSIZE, 0);// receive the first part
        }
        
        gettimeofday(&end, NULL); // set the end time
        timersub(&end, &start, &calc);
        timeArray[last] = calc.tv_sec + calc.tv_usec / 1000.0; //save the time
        last++;
        printf("received first message...\n");

        printf("send authentication...\n");
        send(client_sock, AUTH, strlen(AUTH) + 1, 0);//send the authentication
        printf("authentication has been sent\n");

        printf("change the cc Algorithm...\n");
        if(strcmp(CC , "reno") == 0){//check which on to change to
            CC = "cubic"; 
        }
        else{
            CC = "reno";
        }
        setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, &CC, sizeof(CC)); // change the cc Algorithm
        printf("change the cc Algorithm to %s\n" , CC);

        memset(buffer, '\0', BUFSIZE);
        byteRecv = 0;
        gettimeofday(&start, NULL); //set the start time
        while (byteRecv != sizepart2)
        {
            byteRecv += recv(client_sock, buffer, BUFSIZE, 0); //receive the second part
        }
        gettimeofday(&end, NULL);
        timersub(&end, &start, &calc);
        timeArray[last] = calc.tv_sec + calc.tv_usec / 1000.0; // set the end time 
        last++;
        file++;
        printf("received second message...\n");

        printf("change the cc Algorithm...\n");
        if(strcmp(CC , "reno") == 0){//check which on to change to
            CC = "cubic"; 
        }
        else{
            CC = "reno";
        }
        setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, &CC, sizeof(CC));// change the cc Algorithm
        printf("change the cc Algorithm to %s\n" , CC);

    }
    
    //close the sockets
    close(client_sock);
    close(server_sock);
    

    return 0;
}
