#include "myLib.h"

int main()
{

    /* 
        file: the file that we need to send
        size: the file's size
        half: half of the file's size
        byteSend: save the amount of the byte we send - (only for the file send use)
        size1: the size of the first part we send 
        size2: the size of the second part we send
        addr: the server address setting
        buffer: the message holder , use to save what the server send *us*
        d: the user desicion 
        CC: the current cc algorithm 
        partA: the first part to send
        partB: the second part to send
        check: save the ok 
        sock: the socket file descriptor
        loss: how much byte loss by precent
    */

    FILE* file = NULL;
    size_t size = 0, half = 0 , bytesSent = 0, size1 = 0, size2 = 0;   
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    char buffer[BUFSIZE] = {'\0'}, d = '\0', *CC = NULL , *partA =NULL,*partB = NULL, check[BUFSIZ] = {'\0'};   
    int sock = 0 ;
    double loss = 0.0;

    file = fopen("msg.txt" , "r"); //opening file
    if(file == NULL) // check if file exist
    {
        perror("fopen()");
        exit(errno);
    }
    printf("the file is open\n");

    
    fseek(file , SEEK_SET , SEEK_END);//calculate the size of the file
    size = ftell(file);
    rewind(file);
    half = size/2;

    partA = (char*)malloc((half+1) * sizeof(char)); //allocating memmory for saving parts 
    partB = (char*)malloc((size - half+1) * sizeof(char));
    memset(partA , '\0' ,(half+1)*sizeof(char) );
    memset(partB , '\0' ,(size - half+1)*sizeof(char) );

    fread(partA , 1 , half + 1 , file); //coping the parts to memmory
    fread(partB , 1 , size - half + 1 , file); 
    fclose(file); //closing the file

    sock = socket(AF_INET, SOCK_STREAM, 0); // create the socket
    if(sock < 0) //check if socket created
    {
        perror("socket() failed");
        exit(errno);
    }
    printf("socket created\n");

    addr.sin_family = AF_INET;//setting up server address
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) // connect to the receiver
    {
        perror("connect() failed");
        exit(errno);
    }
    printf("connected to server!\n");

    CC = "cubic"; //the defult of the cc algorithm

    size1 = strlen(partA) + 1; //save the size of each part
    size2 = strlen(partB) + 1; 

    send(sock, &size1, sizeof(size1), 0); //send the size of the parts to the receiver
    send(sock, &size2 , sizeof(size2), 0); 
    
    REPEAT: //label for repeating process

    printf("wait for ok message from the receiver...\n"); 
    send(sock, "ok", strlen("ok") + 1, 0); //send ok message to the receiver
    recv(sock, check, BUFSIZE, 0); // receive back ok message
    if(strcmp("ok", check) != 0) //check if the receiver send back ok
    {
        printf("didnt receiver ok...\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("ok message - continue\n");

    printf("send the first half...\n");
    bytesSent = send(sock, partA, strlen(partA) + 1, 0); //send the first part
    if(bytesSent < 0)
    {
        perror("send() failed");
        exit(errno);
    }
    if (bytesSent == 0) //check if the part sent clearly
    { 
        printf("peer has closed the TCP connection prior to send().\n");
        exit(EXIT_FAILURE);
    } 
    else if (bytesSent < strlen(partA) + 1 ) 
    {
        printf("sent only %ld bytes from the required %ld.\n", bytesSent , strlen(partA) + 1);
        loss = (bytesSent / (strlen(partA) + 1)) * 100.0;
        printf("loss %.2f %%bytes\n" , loss);
    }
    else
    {
        printf("first half has been sent\n");
    }

    printf("wait for authentication...\n");
    recv(sock, buffer, BUFSIZE, 0); //receive the authentication
    if(strcmp(AUTH, buffer) != 0) //check if the authentication is correct
    {
        printf("authentication doesn match\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("authentication match\n");

    printf("change the cc Algorithm...\n");
    if(strcmp(CC , "reno") == 0){ //check to which cc should change - "reno" or "cubic"
        CC = "cubic"; 
    }
    else{
        CC = "reno";
    }
    setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, &CC, sizeof(CC)); //change the cc algorithm
    printf("change the cc Algorithm to %s\n" , CC);

    printf("send the second part...\n");
    bytesSent = 0;
    bytesSent = send(sock, partB , strlen(partB) + 1, 0); // send the second part
    if(bytesSent < 0)
    {
        perror("send() failed");
        exit(errno);
    }
    if (bytesSent == 0) //check if the part sent clearly
    { 
        printf("peer has closed the TCP connection prior to send().\n");
        exit(EXIT_FAILURE);
    } 
    else if (bytesSent < strlen(partB) + 1 ) 
    {
        printf("sent only %ld bytes from the required %ld.\n", bytesSent , strlen(partB) + 1);
        loss = (bytesSent / (strlen(partB) + 1)) * 100.0;
        printf("loss %.2f %%bytes\n" , loss);
    }
    else
    {
        printf("second half has been sent\n");
    }

    printf("send again? (y/n)\n"); // ask if send again
    scanf(" %c", &d);
    while(d != 'y' && d != 'n')
    {
        printf("please enter y or n only!\n");
        scanf(" %c", &d);
    }

    
    if(d == 'n')//user choose to exit
    {
        send(sock, "exit", strlen("exit") + 1, 0); // send an exit message to the receiver
        printf("exit message has been sent\n");
    }
    else //user choose to repeat
    {
        printf("change the cc Algorithm...\n");
        if(strcmp(CC , "reno") == 0){ //check to which cc should change - "reno" or "cubic"
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
