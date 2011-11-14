#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H
#include"networkStructures.h"

using namespace std;

static void *recvData(void *n){
    char message[50];
    struct data *recvExecParam;
    recvExecParam=(struct data *)calloc(1,sizeof(struct data));
    recvExecParam->args=(((socketsOpened *)n)->additionalData);
    void *args=(((socketsOpened *)n)->additionalData);
    int clnumber=(((socketsOpened *)n)->clientnumber);
    int numbytes;
    string msg="";
    char *c;
    while(1){
        numbytes=recv(((socketsOpened *)n)->client_socket,(void *)message,50,0);
        if(errno==ENOTCONN||numbytes==0|errno==88){
            //close(((socketsOpened *)n)->server_socket);
            close(((socketsOpened *)n)->client_socket);
        }
        message[numbytes]='\0';
        msg+=message;
        while(msg.find("~")!=string::npos){
            int msg_len=msg.find_first_of('~')+1;
            c=(char *)calloc(msg_len,sizeof(char));
            for(int i=0;i<msg_len;i++){
                c[i]=msg[i];
            }
            c[msg_len-1]='\0';
            recvExecParam=(struct data*)calloc(1,sizeof(struct data));
            recvExecParam->msg=c;
            recvExecParam->args=args;
            recvExecParam->clientnumber=clnumber;
            ((socketsOpened *)n)->function((void*)recvExecParam);
            msg=msg.substr(msg_len);
        }
    }
}

#endif // NETWORKFUNCTIONS_H
