#ifndef NETWORKSTRUCTURES_H
#define NETWORKSTRUCTURES_H

#define SETSOCKOPT_ERROR -1
#define BIND_ERROR -2
#define LISTEN_ERROR -3
#define ACCEPT_ERROR -4
#define SERVER_SUCCESS 1

#define SEND_ERROR -5
#define SEND_SUCCESS 1
#define HIDDEN_ERROR -6
#define CONNECT_FAILURE -7

struct socketsOpened{
    int server_socket;
    int client_socket;
    void*(*function)(void *);
    void *additionalData;
    int clientnumber;
    //void *argument;
};

struct data{
    void *msg;
    void *args;
    int clientnumber;
};

#endif // NETWORKSTRUCTURES_H
