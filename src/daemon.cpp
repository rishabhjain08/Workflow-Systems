#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "server.h"
#include "../inc/algorithms.h"
#include <iostream>
#include <pthread.h>
#include "../inc/network_codes.h"
using namespace std;

vector<Server *> servers;

vector<Request *> requests;
vector<Room *> rooms;
vector<User *> users;

int arrange(vector<Request *>, int);
vector<int> server_nos;

void unbookall(){
    for (int i = 0, n = rooms.size(); i < n; i++){
        for (int j = 0; j < rooms[i]->bookings.size(); j++){
            if (rooms[i]->bookings[j]->status == TEMP){
                rooms[i]->bookings.erase(rooms[i]->bookings.begin() + j);
                j--;
            }
        }
    }
}

int readRoomData(const char * filename){
    ifstream fp(filename);
    string line;
    if (fp.is_open()){
        getline(fp, line);
        while(fp.good()){
            getline(fp, line);
            vector<string> x = split_(line, ';');
            vector<string> x0 = split_(x[0], ',');
            vector<string> x1 = split_(x[1], ',');
            bool resources[12];
            for (int i = 0; i < 12; i++){
                resources[i] = (bool) atoi(x1[i].c_str());
            }
            Room * r = new Room(atoi(x0[0].c_str()), x0[1], encode(resources));
            rooms.push_back(r);
            cout<< line<<endl;
        }
        fp.close();
    }
    else{
        cout<<"unable to open file\n";
    }
    return 0;
    
}

int readUserData(const char * filename){
    ifstream fp(filename);
    string line;
    if (fp.is_open()){
        getline(fp, line);
        while(fp.good()){
            getline(fp, line);
            vector<string> x = split_(line, ',');
            User * usr = new User(atoi(x[0].c_str()), x[1], x[2], x[3], atoi(x[4].c_str()));
            users.push_back(usr);
            cout<< line<<endl;
        }
        fp.close();
    }
    else{
        cout<<"unable to open file\n";
    }
    return 0;
    
}


void daemonize(){
int i,lfp;
char str[10];
	if(getppid()==1) return;
	i=fork();
	if (i<0) exit(1);
	if (i>0) exit(0);
	setsid();
	for (i=getdtablesize();i>=3;--i) close(i);
	
	i=open("out",O_RDWR|O_CREAT); 
    close(1);
    dup(i);
    close(i);
	
	i=open("in",O_RDWR|O_CREAT); 
    close(0);
    dup(i);
    close(i);
    
    i=open("err",O_RDWR|O_CREAT); 
    close(2);
    dup(i);
    close(i);

	umask(027);
	
}

Request * objectify(char * re_s){
    string s(re_s);
    vector<string> x = split_(re_s, ',');
    Request * re = new Request(atoi(x[0].c_str()), atoi(x[1].c_str()), atoi(x[2].c_str()), atoi(x[3].c_str()));
    re->date = x[4];
    re->starttime = Time(atoi(x[5].c_str()), atoi(x[6].c_str()));
    re->endtime = Time(atoi(x[7].c_str()), atoi(x[8].c_str()));
    return re;    
}

void * serverStartCB(void * data){
    //printf("A client is connected\n");
    int server_no = (*(int * )data);
    cout<<"A client is connected\n";
    cout<<"server no is : "<<server_no<<"\n";
    //servers[server_no]->sendData((void *) ("Hi !"), 6);
    //cout<<"I ma here at this line\n";
}

int authorize(string up){
    cout<<"In authorize with "<<up<<"\n";
    
    vector<string> x = split_(up, ',');
    cout<<"In authorize with "<<x[0]<<" and "<<x[1]<<"\n";
    cout<<"users.size() is "<<users.size()<<endl;
    for (int i = 0, n = users.size(); i < n; i++){
        cout<<"user "<<users[i]->username<<" pass "<<users[i]->pass<<endl;
        if (users[i]->username.compare(x[0]) == 0){
            cout<<"Got user "<<users[i]->username<<endl;
            if (users[i]->pass.compare(x[1]) == 0){
                return users[i]->userID;
            }
            else return 0;
        }
    }
    return 0;
}

int newrequestID(){
    static int i;
    i++;
    cout << "newrequestID is "<<(i + 500)<<endl;
    return i + 500;
}

int cancel_ID(int requestID){
    bool got1 = false, got2 = false;
    for (int i = 0; i < requests.size(); i++){
        if (requests[i]->requestID == requestID){
            requests.erase(requests.begin() + i);
            got1 = true;
        }
    }
    for (int i = 0; i < rooms.size(); i++){
        for (int j = 0; j < rooms[i]->bookings.size(); j++){
            if (rooms[i]->bookings[j]->requestID == requestID){
                rooms[i]->bookings.erase(rooms[i]->bookings.begin() + j);
                got2 = true;
            }
        }
    }
    return (int) got1 && got2;
}

void confirmAllRequests(){
    for (int i = 0; i < requests.size(); i++){
        requests[i]->status = CONFIRMED;
    }
}

void * msgRcvdCB(void * data){
    //printf("Received message: %s", (char *)(((struct data*) data)->msg));
    char * msg = (char *)(((struct data*) data)->msg);
    int server_no = (*(int * )(((struct data*) data)->args));
    int client_no =((struct data*) data)->clientnumber;
    //cout<<"client number : "<<client_no<<"\n";
    string msg_s(msg);
    
    //cout<<"object stringified is "<<msg<<endl;
    cout<<"Received message: "<<msg<<" from client "<<client_no<<"\n";
    
    vector<string> x = split_(msg, ':');
    
    Request * re;
    ostringstream sstream;
    int userID, requestID, v;
    switch(atoi(x[0].c_str())){
        case CONFIRM_REQS:
        confirmAllRequests();
        cout<<"All Requests confirmed !\n";
        break;
        case CANCEL_REQ:
        requestID = atoi(x[1].c_str());
        v = cancel_ID(requestID);
        if (v == 1){
            cout<<"Request deleted";
        }
        sstream<<ALL_DATA;
        sstream<<":";
        for (int i = 0, n = rooms.size(); i < n; i++){
            Room * r = rooms[i];
            sstream<<r->roomID<<" "<<r->name<<" "<<r->resources;
            sstream<<"\n";
            for (int i = 0, n = r->bookings.size(); i < n; i++){
                sstream<<"\t"<<r->bookings[i]->starttime.hours<<":"<<r->bookings[i]->starttime.minutes<<" "<<r->bookings[i]->endtime.hours<<":"<<r->bookings[i]->endtime.minutes<<" "<<r->bookings[i]->requestID<<" "<<r->bookings[i]->requirements<<"\n";
            }
            sstream<<"\n";
        }
        
        servers[server_no]->sendData((void *)sstream.str().c_str(), sstream.str().size(), client_no);
        break;
        case REQUEST_ALL_A:
        sstream<<ALL_DATA;
        sstream<<":";
        for (int i = 0, n = rooms.size(); i < n; i++){
            Room * r = rooms[i];
            sstream<<r->roomID<<" "<<r->name<<" "<<r->resources;
            sstream<<"\n";
            for (int i = 0, n = r->bookings.size(); i < n; i++){
                sstream<<"\t"<<r->bookings[i]->starttime.hours<<":"<<r->bookings[i]->starttime.minutes<<" "<<r->bookings[i]->endtime.hours<<":"<<r->bookings[i]->endtime.minutes<<" "<<r->bookings[i]->requestID<<" "<<r->bookings[i]->requirements<<"\n";
            }
            sstream<<"\n";
        }
        
        servers[server_no]->sendData((void *)sstream.str().c_str(), sstream.str().size(), client_no); 
        break;    
        case REQUEST_FOR_ID:
        cout<<"Authorizing...\n";
            userID = authorize(x[1]);
            if (userID == 0){
                sstream << AUTH_FAIL;
                sstream << ":";
                sstream << "sorry";
            }
            else{
                cout<<"Authorized with "<<userID<<"\n";
                sstream << ID_FOR_REQUEST;
                sstream << ":";
                sstream << userID;
                sstream << ",";
                sstream << newrequestID();
                cout<<"Sending : "<<sstream.str().c_str()<<endl;
                servers[server_no]->sendData((void *) sstream.str().c_str(), sstream.str().size(),client_no);
                }
            break;
        case INCOMING_REQUEST:{
            re = objectify((char *)x[1].c_str());
            re->print();
            requests.push_back(re);
            unbookall();
            populate(requests, rooms);
            cout<<"Starting Algorithm\n";
            print(rooms);
            int retval = arrange(requests, requests.size() - 1);
            print(rooms);
            
            if (retval == 1){
                cout<<"Successfully booked a room!\n";
                print(rooms);
                sstream << BOOK_SUCCESS;
                sstream << ":";
                cout<<"Sending : "<<sstream.str().c_str()<<endl;
                
                servers[server_no]->sendData((void *) sstream.str().c_str(), sstream.str().size(),client_no); 
            }
            else if (retval == -1){
                cout<<"Cannot book room\n";
                requests.pop_back();
                populate(requests, rooms);
                arrange(requests, requests.size() - 1);
                sstream << BOOK_FAILURE;
                sstream << ":";
                sstream << "sorry";
                cout<<"Sending : "<<sstream.str().c_str()<<endl;
                
                servers[server_no]->sendData((void *) sstream.str().c_str(), sstream.str().size(),client_no);
            }
            
            break;
            }
        case HANDSHAKE:
            ostringstream ss;
            ss<<CONFIRM_HANDSHAKE;
            ss<<":hi123";
            cout<<"Sending : "<<sstream.str().c_str()<<endl;
            int v = servers[server_no]->sendData((void *) ss.str().c_str(), ss.str().size(),client_no);
            break;
    }
}

void * port_hosting(void * args){
    int std_port = 5000;
    int server_no = 0;
	//daemonize();
    while(1){
        Server * server = new Server();
        ostringstream s;
        s << std_port;
        //pthread_mutex_lock(&mutex);
        servers.push_back(server);
        server_nos.push_back(server_no);
        
        cout<<"server_nos[server_nos.size()-1] is "<<server_nos[server_nos.size()-1]<<endl;
        int v = server->start_unthreaded((char *)s.str().c_str(), serverStartCB, &server_nos[server_nos.size()-1], msgRcvdCB, &server_nos[server_nos.size()-1]);
        if (v != SERVER_SUCCESS){
            server_nos.erase(server_nos.begin() + server_nos.size() - 1);
            servers.erase(servers.begin() + servers.size() - 1);
            std_port++;
            
            continue;
        }
        server_no++;
        cout<<"SUCCESS!\n";
        
        std_port++;
        
    
    }
    return NULL;
}


int main(int argc, char * argv[])
{
    daemonize();
    readUserData("userdata");
    readRoomData("roomdata");
    pthread_t p;
    pthread_create(&p, NULL, port_hosting, NULL);
    void ** retval;
    pthread_join(p, retval);
    
    return 0;
    
}
