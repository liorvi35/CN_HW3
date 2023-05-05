#ifndef _MY_HEADER_H_

    #define _MY_HEADER_H_

    #define SERVER_IP "127.0.0.1"
    #define CONNECTION_PORT 9091
    #define NUM_CONNECTIONS 300
    #define AUTHENTICATION "0000000111111001"
    #define END_MSG "END"
    #define CONTINUE_MSG "CONTINUE"
    #define EXIT_MSG "EXIT"
    #define TIMEOUT 1
    #define CUBIC_CC "cubic"
    #define RENO_CC "reno"

    void signal_handler(int);

#endif