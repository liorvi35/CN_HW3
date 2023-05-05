#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "myHeader.h"
#include <time.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <signal.h>

int client_sock = 0;

char *partA = NULL, *partB = NULL;

FILE *file = NULL;


void signal_handler(int sid)
{
    fprintf(stdout, "\nClosing client...\n");

    close(client_sock);

    free(partA);
    partA = NULL;

    free(partB);
    partB = NULL;

    if(file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./Sender <file>\n");
        exit(EXIT_FAILURE);
    }

    size_t file_size = 0, bytes_recv = 0, bytes_sent = 0, first_half = 0, second_half = 0;
    file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        perror("fopen() failed");
        exit(errno);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    first_half = file_size / 2;
    second_half = file_size - first_half;

    partA = (char *)calloc(first_half + 1, sizeof(char));
    if (partA == NULL)
    {
        perror("calloc() failed");

        fclose(file);
        file = NULL;

        exit(errno);
    }

    partB = (char *)calloc(second_half + 1, sizeof(char));
    if (partB == NULL)
    {
        perror("calloc() failed");

        fclose(file);
        file = NULL;

        free(partA);
        partA = NULL;

        exit(errno);
    }

    if(fread(partA, 1, first_half, file) < 0)
    {
        perror("fread() failed");

        fclose(file);
        file = NULL;

        free(partA);
        partA = NULL;

        free(partB);
        partB = NULL;

        exit(errno);
    }

    if(fread(partB, 1, second_half, file) < 0)
    {
        perror("fread() failed");

        fclose(file);
        file = NULL;

        free(partA);
        partA = NULL;

        free(partB);
        partB = NULL;

        exit(errno);
    }

    fclose(file);
    file = NULL;

    client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock <= 0)
    {
        perror("socket() failed");

        free(partA);
        partA = NULL;

        free(partB);
        partB = NULL;

        exit(errno);
    }
    fprintf(stdout, "Socket created.\n");

    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr = {0};
    memset(&server_addr, 0, addr_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(CONNECTION_PORT);

    if (connect(client_sock, (struct sockaddr *)&server_addr, addr_size) < 0)
    {
        perror("connect() failed");

        close(client_sock);

        free(partA);
        partA = NULL;

        free(partB);
        partB = NULL;

        exit(errno);
    }
    fprintf(stdout, "Connected to the server.\n");

    char buffer[BUFSIZ] = {0}, user_desicion = 0;

    while (1)
    {
        if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, CUBIC_CC, sizeof(CUBIC_CC)) < 0)
        {
            perror("setsockopt() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }

        bytes_sent = send(client_sock, partA, strlen(partA) + 1, 0);
        if (bytes_sent < 0)
        {
            perror("send() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }

        sleep(TIMEOUT);
        bytes_sent = send(client_sock, END_MSG, strlen(END_MSG) + 1, 0);
        if (bytes_sent < 0)
        {
            perror("send() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }
        fprintf(stdout, "First part has been sent!\n");

        memset(buffer, 0, BUFSIZ);
        bytes_recv = recv(client_sock, buffer, BUFSIZ, 0);
        if (bytes_recv <= 0)
        {
            perror("recv() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }
        if (strcmp(buffer, AUTHENTICATION))
        {
            fprintf(stderr, "Authentication doesnt match!\n");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }
        fprintf(stdout, "Authentication match.\n");

        if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, RENO_CC, sizeof(RENO_CC)) < 0)
        {
            perror("setsockopt() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }

        bytes_sent = send(client_sock, partB, strlen(partB) + 1, 0);
        if (bytes_sent < 0)
        {
            perror("send() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }

        sleep(TIMEOUT);
        bytes_sent = send(client_sock, END_MSG, strlen(END_MSG) + 1, 0);
        if (bytes_sent < 0)
        {
            perror("send() failed");

            close(client_sock);

            free(partA);
            partA = NULL;

            free(partB);
            partB = NULL;

            exit(errno);
        }
        fprintf(stdout, "Second part has been sent!\n");

        do
        {
            fprintf(stdout, "Send file again? (y/n) : ");
            scanf(" %c", &user_desicion);
        } while (user_desicion != 'y' && user_desicion != 'n' && user_desicion != 'Y' && user_desicion != 'N');

        if (user_desicion == 'y' || user_desicion == 'Y')
        {
            bytes_sent = send(client_sock, CONTINUE_MSG, strlen(CONTINUE_MSG) + 1, 0);
            if (bytes_sent < 0)
            {
                perror("send() failed");

                close(client_sock);

                free(partA);
                partA = NULL;

                free(partB);
                partB = NULL;

                exit(errno);
            }

            continue;
        }
        else
        {
            bytes_sent = send(client_sock, EXIT_MSG, strlen(EXIT_MSG) + 1, 0);
            if (bytes_sent < 0)
            {
                perror("send() failed");

                close(client_sock);

                free(partA);
                partA = NULL;

                free(partB);
                partB = NULL;

                exit(errno);
            }

            shutdown(client_sock, SHUT_RDWR);

            break;
        }
    }

    close(client_sock);

    free(partA);
    partA = NULL;

    free(partB);
    partB = NULL;

    return 0;
}
