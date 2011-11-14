#ifndef CLIENT_H
#define CLIENT_H
#include"networkStructures.h"
#include<pthread.h>

class Client{
	pthread_t recvThread;
    int serv_sock,sock;
    struct ::socketsOpened *sockets;
    void *get_in_addr(struct sockaddr *);
    void *additionalArgs;
public:
    Client();
    int start(char * ,char *,void*(receivedDataProcessor)(void *),void *);
    int sendData(void *,int );
    void shut(void);
};
#endif // CLIENT_H
