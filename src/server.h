#ifndef SERVER_H
#define SERVER_H
#include"networkStructures.h"
#include<pthread.h>
#include <vector>
using namespace std;
class Server{
	pthread_t hostThread;
    vector<struct ::socketsOpened *>sockets;
    int hosting(void);
    void *get_in_addr(struct sockaddr *);
    char *portno;
    void*(*function)(void*);
    void*(*onConnection)(void *);
    void *additionalData;
    void *conn_args;
	vector<int> client_sock;
	int sock;
    public :
    Server();
    int start(char *,void*(connected)(void *),void *,void *(receivedDataProcessor)(void *),void *);
    int start_unthreaded(char *,void*(connected)(void *),void *,void *(receivedDataProcessor)(void *),void *);
    int sendData(void *,int ,int);
    void shut(void);
    pthread_t *getThreadID();
    bool accept_condition;
};

#endif // SERVER_H
