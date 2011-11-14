#include<iostream>
#include<stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<pthread.h>
#include"networkFunctions.h"
#include"client.h"
#include"networkStructures.h"

using namespace std;

	void *Client::get_in_addr(struct sockaddr *sock){
		if(sock->sa_family==AF_INET){
		    return &(((struct sockaddr_in *)sock)->sin_addr);
		}
		else if(sock->sa_family==AF_INET6){
		    return &(((struct sockaddr_in6 *)sock)->sin6_addr);
		}
	}

    Client::Client(){}

    int Client::start(char *ip ,char *portno,void*(receivedDataProcessor)(void *),void *args){
        struct addrinfo *servinfo,hints,*p;
		this->sockets=(struct socketsOpened*)calloc(1,sizeof(struct socketsOpened));
		p=NULL;
		memset(&hints,0,sizeof hints);
		hints.ai_family=AF_UNSPEC;
		hints.ai_socktype=SOCK_STREAM;
		sock=getaddrinfo(ip,portno,&hints,&servinfo);
		if(sock!=0){
		    cout<<"Server not found.\n";
		    close(sock);
		    return HIDDEN_ERROR;
		}
		serv_sock=0;
        for(p=servinfo;p!=NULL;p=p->ai_next){
		    if((serv_sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
		        continue;
		    }
		    if(connect(serv_sock,p->ai_addr,p->ai_addrlen)==-1){
		        close(serv_sock);
		        continue;
		    }
		    else
		        break;
		}
		if(p==NULL){
		    cout<<"Client Failed to Connect.\n";
		    return CONNECT_FAILURE;
		}
		freeaddrinfo(servinfo);
        char servername[50];
		gethostname(servername,50);
		inet_ntop(p->ai_family,get_in_addr ((struct sockaddr *)p->ai_addr) ,servername,sizeof servername);
		cout<<"Connected to "<<servername<<" at port number "<<portno<<"\n";
		sockets->server_socket=sock;
		sockets->client_socket=serv_sock;
		sockets->function=receivedDataProcessor;
		sockets->additionalData=args;
        if(pthread_create(&recvThread,NULL,::recvData,sockets)==0)
		    cout<<"Client can receive.\n";
		else
		    cout<<"client cannot receive.\n";
		return 0;
	}


int Client::sendData(void *message,int size){
    string s=(char *)message;
    s+="~";
    int numbytes=send(serv_sock,(void *)(&s[0]),size+1,0);
    if(errno==EPIPE||numbytes==-1){
        //cout<<"sock is "<<sock<<endl;
        close(sock);
        //cout<<"serv_sock is "<<serv_sock<<endl;
        close(serv_sock);
        return SEND_ERROR;
    }
    return SEND_SUCCESS;
}

void Client::shut(void){
    cout<<"Client is shutting....\n";
    close(sock);
    close(serv_sock);
}

