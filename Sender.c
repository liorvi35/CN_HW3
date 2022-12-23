#include "myLib.h" // including our librar

int main()
{
    /*
        Variables Dictionary:
        (01) FILE *file: the file that will be send from Sender to Receiver
        (02) size_t size: file's size
        (03) size_t hakf: half of file's size
        (04) size_t byteSend: holder of amount of bytes that were sent
        (05) size_t size1: size of first file's part
        (06) size_t size2: size of second file's part
        (07) struct sockaddr_in addr: socket's settings for storing ip and port in IPV4 connectiom
        (08) char buffer[]: received information holed
        (09) char d: user decision holder 
        (10) char *CC: current congestion control algorithm 
        (11) char *partA: firs part of file holder
        (12) char *partb: second part of file holder
        (13) int sock: socket's file descriptor
        (14) double loss: loss precentage rate
    */

    /* declaring and reseting variables */
    FILE* file = NULL;
    size_t size = 0, half = 0, bytesSent = 0, size1 = 0, size2 = 0;   
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    char buffer[BUFSIZE] = {'\0'}, d = '\0', *CC = NULL, *partA =NULL, *partB = NULL;   
    int sock = 0 ;
    double loss = 0.0;

    /* (1) opening and reading the file */
    file = fopen("msg.txt" , "r");
    if(file == NULL) // checking if file exists
    {
        perror("fopen() failed");
        close(sock);
        exit(errno);
    }
    printf("file opened!\n");

    fseek(file , SEEK_SET , SEEK_END); //calculating the size of file
    size = ftell(file);
    rewind(file);
    half = size / 2;

    partA = (char*)malloc((half +1 ) * sizeof(char)); // allocating and reseting memmory for saving parts of file 
    partB = (char*)malloc((size - half + 1) * sizeof(char));
    memset(partA, '\0' ,(half+1)*sizeof(char));
    memset(partB, '\0' ,(size - half+1)*sizeof(char));

    fread(partA, 1, half + 1, file); // coping the parts to allocated memmory
    fread(partB , 1, size - half + 1, file);

    size1 = strlen(partA) + 1; // saving size of each part
    size2 = strlen(partB) + 1;

    fclose(file); //closing the file

    /* (2) creating TCP connection */
    sock = socket(AF_INET, SOCK_STREAM, 0); // creating the communication socket
    if(sock <= 0) // checking if socket created
    {
        perror("socket() failed");
        close(sock);
        free(partA);
        free(partB);
        exit(errno);
    }
    printf("socket created!\n");

    addr.sin_family = AF_INET; //setting up socket's used protocol, port and ip
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) // checking if could connect to server
    {
        perror("connect() failed");
        close(sock);
        free(partA);
        free(partB);
        exit(errno);
    }
    printf("connected to server!\n");

    bytesSent = send(sock, &size1, sizeof(size1), 0); // sending the size of each part to the Receiver
    if(bytesSent < 0)
    {
        perror("send() failed");
        close(sock);
        free(partA);
        free(partB);
        exit(errno);
    }
    else if(bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send()\n");
        close(sock);
        free(partA);
        free(partB);
        exit(EXIT_FAILURE);
    }

    bytesSent = send(sock, &size2 , sizeof(size2), 0); // sending the size of each part to the Receiver
    if(bytesSent < 0)
    {
        perror("send() failed");
        close(sock);
        free(partA);
        free(partB);
        exit(errno);
    }
    else if(bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send()\n");
        close(sock);
        free(partA);
        free(partB);
        exit(EXIT_FAILURE);
    }
    
    CC = "cubic"; // setting up the defultive congestion control algorithm

    REPEAT: // label for repeating process

        bytesSent = send(sock, OK, strlen(OK) + 1, 0); // send ok message to the Receiver
        if(bytesSent < 0) 
        {
            perror("send() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }
        else if(bytesSent == 0)
        {
            printf("peer has closed the TCP connection prior to send()\n");
            close(sock);
            free(partA);
            free(partB);
            exit(EXIT_FAILURE);
        }

        if(recv(sock, buffer, BUFSIZE, 0) <= 0) // receiving Receiver's answer 
        {
            perror("recv() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }

        if(strcmp(OK, buffer) != 0) // checking if receiver returns answers ok
        {
            printf("error occurred\n");
            close(sock);
            free(partA);
            free(partB);
            exit(EXIT_FAILURE);
        }

        /* (3) sending the first part */
        bytesSent = send(sock, partA, size1, 0);
        if(bytesSent < 0)
        {
            perror("send() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }
        else if (bytesSent == 0) //check if the part sent clearly
        { 
            printf("peer has closed the TCP connection prior to send()\n");
            close(sock);
            free(partA);
            free(partB);
            exit(EXIT_FAILURE);
        } 
        else if (bytesSent < size1) 
        {
            loss = (bytesSent / size1) * 100.0;
            printf("could sent only %ld bytes from the required %ld bytes.\nlost %.2f %%bytes.", bytesSent, size1, loss);
        }
        else
        {
            printf("first half has been sent!\n"); 
        }
        
        /* (4) checking for authentication */
        printf("waiting for authentication...\n"); 
        if(recv(sock, buffer, BUFSIZE, 0) <= 0) // receiving the authentication
        {
            perror("recv() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }
        if(strcmp(AUTH, buffer) != 0) // checking if the authentication is correct
        {
            printf("authentication does not match\n");
            close(sock);
            free(partA);
            free(partB);
            exit(EXIT_FAILURE);
        }
        printf("authentication match!\n");

        /* (5) changing CC algorithm */
        CC = (strcmp(CC, "reno") == 0 ? "cubic" : "reno"); // determinating congestion control algorithm
        if(setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CC, sizeof(CC)) < 0) //changing congestion control algorithm
        {
            perror("setsockopt() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }
        printf("CC algorithm has been changed to: \"%s\"\n", CC);

        /* (6) sending the second part */
        bytesSent = send(sock, partB , size2, 0); // send the second part
        if(bytesSent < 0)
        {
            perror("send() failed");
            close(sock);
            free(partA);
            free(partB);
            exit(errno);
        }
        else if (bytesSent == 0) //check if the part sent clearly
        { 
            printf("peer has closed the TCP connection prior to send()\n");
            close(sock);
            free(partA);
            free(partB);
            exit(EXIT_FAILURE);
        } 
        else if (bytesSent < size2) 
        {
            loss = (bytesSent / size2) * 100.0;
            printf("could sent only %ld bytes from the required %ld bytes.\nlost %.2f %%bytes.", bytesSent, size2, loss);
        }
        else
        {
            printf("second half has been sent!\n");
        }

        /* (7) user decision */
        printf("send file again? (y/n)\n"); // asking the user if send again
        scanf(" %c", &d);
        while(d != 'y' && d != 'n')
        {
            printf("please enter \'y\' to send or \'n\' to exit!\n");
            scanf(" %c", &d);
        }
        if(d == 'n') // (7-b-i) user chose to exit
        {   /* (7-b-ii) sending exit message */
            bytesSent = send(sock, "exit", strlen("exit") + 1, 0); // send an exit message to the receiver
            if(bytesSent < 0)
            {
                perror("send() failed");
                close(sock);
                free(partA);
                free(partB);
                exit(errno);
            }
            else if (bytesSent == 0) //check if the part sent clearly
            { 
                printf("peer has closed the TCP connection prior to send()\n");
                close(sock);
                free(partA);
                free(partB);
                exit(EXIT_FAILURE);
            } 
            else
            {
                printf("exit message has been sent\n");
            }
            
        }
        else // (7-a-i) user chose to send file again
        {
            /* (7-a-ii) changing back the CC algrithm */
            CC = (strcmp(CC, "reno") == 0 ? "cubic" : "reno"); // determinating congestion control algorithm
            if(setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CC, sizeof(CC)) < 0) //changing congestion control algorithm
            {
                perror("setsockopt() failed");
                close(sock);
                free(partA);
                free(partB);
                exit(errno);
            }
            printf("CC algorithm has been changed to: \"%s\"\n", CC);
            goto REPEAT; // (7-a-iii) repeating process
        }

    /* (7-b-iii) closing connection */
    close(sock); // freeing recourses and closing socket
    free(partA);
    free(partB);

    exit(EXIT_SUCCESS);
}
