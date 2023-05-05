#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "myHeader.h"


int server_sock = 0, client_sock = 0;

double *reno = NULL, *cubic = NULL;

void signal_handler(int sid)
{
    fprintf(stdout, "\nClosing server...\n");

    close(client_sock);

    close(server_sock);

    free(cubic);
    cubic = NULL;

    free(reno);
    reno = NULL;

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock <= 0)
    {
        perror("socket() failed");
        exit(errno);
    }
    fprintf(stdout, "Socket created.\n");

    int yes = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        perror("setsockopt() failed");
    }

    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr = {0}, client_addr = {0};
    memset(&server_addr, 0, addr_size);
    memset(&client_addr, 0, addr_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(CONNECTION_PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind() failed");
        close(server_sock);
        exit(errno);
    }
    fprintf(stdout, "Socket bound on: (%s, %d).\n", SERVER_IP, CONNECTION_PORT);

    if (listen(server_sock, NUM_CONNECTIONS) < 0)
    {
        perror("listen() failed");
        close(server_sock);
        exit(errno);
    }
    fprintf(stdout, "Listening for connections...\n");

    char buffer[BUFSIZ] = {0};
    size_t bytes_recv = 0, bytes_sent = 0;

    int count = 1;

    reno = (double *)calloc(count, sizeof(double));
    if (reno == NULL)
    {
        perror("calloc() failed");
        close(server_sock);
        exit(errno);
    }

    cubic = (double *)calloc(count, sizeof(double));
    if (cubic == NULL)
    {
        perror("calloc() failed");

        close(server_sock);

        free(reno);
        reno = NULL;

        exit(errno);
    }

    struct timeval start = {0}, end = {0}, calc = {0};
    memset(&start, 0, sizeof(struct timeval));
    memset(&end, 0, sizeof(struct timeval));
    memset(&calc, 0, sizeof(struct timeval));

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock <= 0)
        {
            perror("accpet() failed");

            close(server_sock);

            free(cubic);
            cubic = NULL;

            free(reno);
            reno = NULL;

            exit(errno);
        }
        fprintf(stdout, "Accepted connection from: (%s, %d).\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1)
        {

            if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, CUBIC_CC, sizeof(CUBIC_CC)) < 0)
            {
                perror("setsockopt() failed");

                close(client_sock);

                close(server_sock);

                free(cubic);
                cubic = NULL;

                free(reno);
                reno = NULL;

                exit(errno);
            }

            gettimeofday(&start, NULL);
            while (1)
            {
                memset(buffer, 0, BUFSIZ);
                bytes_recv = recv(client_sock, buffer, BUFSIZ, 0);
                if (bytes_recv < 0)
                {
                    perror("recv() failed");

                    close(client_sock);

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }
                if (!strcmp(buffer, END_MSG))
                {
                    break;
                }
            }
            gettimeofday(&end, NULL);

            timersub(&end, &start, &calc);

            cubic[count - 1] = (((calc.tv_sec * 1000000.0) + calc.tv_usec) / 1000.0);

            memset(&start, 0, sizeof(struct timeval));
            memset(&end, 0, sizeof(struct timeval));
            memset(&calc, 0, sizeof(struct timeval));

            fprintf(stdout, "First message has been received!\n");

            memset(buffer, 0, BUFSIZ);
            memcpy(buffer, AUTHENTICATION, strlen(AUTHENTICATION));
            bytes_sent = send(client_sock, buffer, strlen(AUTHENTICATION) + 1, 0);
            if (bytes_sent <= 0)
            {
                perror("send() failed");

                close(client_sock);

                close(server_sock);

                free(cubic);
                cubic = NULL;

                free(reno);
                reno = NULL;

                exit(errno);
            }
            fprintf(stdout, "Authentication has been sent.\n");

            if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, RENO_CC, sizeof(RENO_CC)) < 0)
            {
                perror("setsockopt() failed");

                close(client_sock);

                close(server_sock);

                free(cubic);
                cubic = NULL;

                free(reno);
                reno = NULL;

                exit(errno);
            }

            gettimeofday(&start, NULL);
            while (1)
            {
                memset(buffer, 0, BUFSIZ);
                bytes_recv = recv(client_sock, buffer, BUFSIZ, 0);
                if (bytes_recv < 0)
                {
                    perror("recv() failed");

                    close(client_sock);

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }
                if (!strcmp(buffer, END_MSG))
                {
                    break;
                }
            }
            gettimeofday(&end, NULL);

            timersub(&end, &start, &calc);

            reno[count - 1] = (((calc.tv_sec * 1000000.0) + calc.tv_usec) / 1000.0);

            memset(&start, 0, sizeof(struct timeval));
            memset(&end, 0, sizeof(struct timeval));
            memset(&calc, 0, sizeof(struct timeval));

            fprintf(stdout, "Second message has been received!\n");

            memset(buffer, 0, BUFSIZ);
            bytes_recv = recv(client_sock, buffer, BUFSIZ, 0);
            if (bytes_recv < 0)
            {
                perror("recv() failed");

                close(client_sock);

                close(server_sock);

                free(cubic);
                cubic = NULL;

                free(reno);
                reno = NULL;

                exit(errno);
            }

            if (!strcmp(buffer, EXIT_MSG))
            {
                close(client_sock);

                fprintf(stdout, "\n\n DATASET:\n");
                int i = 0;
                for (i = 0; i < count; i++)
                {
                    fprintf(stdout, "%d) reno=%f cubic=%f\n", i + 1, reno[i], cubic[i]);
                }
                fprintf(stdout, "\n\n");

                count = 1;

                cubic = (double *)realloc(cubic, count * sizeof(double));
                if (cubic == NULL)
                {
                    perror("calloc() failed");

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }
                memset(cubic, 0, count * sizeof(double));

                reno = (double *)realloc(reno, count * sizeof(double));
                if (reno == NULL)
                {
                    perror("calloc() failed");

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }
                memset(cubic, 0, count * sizeof(double));

                break;
            }
            if (!strcmp(buffer, CONTINUE_MSG))
            {
                count++;

                cubic = (double *)realloc(cubic, count * sizeof(double));
                if (cubic == NULL)
                {
                    perror("calloc() failed");

                    close(client_sock);

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }

                reno = (double *)realloc(reno, count * sizeof(double));
                if (reno == NULL)
                {
                    perror("calloc() failed");

                    close(client_sock);

                    close(server_sock);

                    free(cubic);
                    cubic = NULL;

                    free(reno);
                    reno = NULL;

                    exit(errno);
                }

                continue;
            }
        }
    }
}
