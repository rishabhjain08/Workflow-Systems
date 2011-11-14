#include <stdio.h>
#include <vector>
#include <string>
#include <queue>
#include <stdlib.h>
#include <queue>
#include <math.h>
#include <iostream>

using namespace std;

class Room;
class User;
class Request;
class Institute;
class Operator;

int encode(bool[]);
void print(vector<Room * >);
void print(Institute *);

class User{
public:
    string username;
    string completename;
    int userID;
    User(int userID, string username, string completename){
        this->userID = userID;
        this->username = username;
        this->completename = completename;
    }
};

class Request{
public:
    int requestID;
    User * user;
    //Room * occupying;
    vector<Room *> candidates;
    int requirements;       // Bits to represent individual requirements 
    int numOfMicrophones;
    Operator * oper;
    Request(int requestID, User * user, int requirements, int numOfMicrophones){
        this->requestID = requestID;
        this->user = user;
        this->requirements = requirements;
        this->numOfMicrophones = numOfMicrophones;
    }
};
class Room{
public:
    string name;
    int roomID; 
    Request * occupiedBy;
    vector<Request *> candidates;
    int resources;           // Bits to represent individual requirements
    Room(int roomID, string name, int resources){
        this->roomID = roomID;
        this->name = name;
        this->resources = resources;
    }
    Room(int roomID, string name, bool resources[]){
        this->roomID = roomID;
        this->name = name;
        this->resources = encode(resources);
    }
};

class Operator{
public:
    int operatorID;
    string name;
    int contact;
    Request  * bookedTo;
    Operator(int operatorID, string name, int contact){
        this->operatorID = operatorID;
        this->name = name;
        this->contact = contact;
    }
};

class Institute{
public:
    vector<Room * > rooms;
    int numOfMicrophones;
    vector<Operator * > operators;
    //vector<User> users;
    int search(Request *, vector<Room *>, int);
    int insert(Request *, vector<Room *> rooms); // Returns -2, if microphones not found, -1 if rooms unavailable, -3 if operators unavailable, 0 if success
    Institute(){
        Room * r = new Room(11, "Bharti_501", 0 + 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048);
        rooms.push_back(r);
        r = new Room(12, "Bharti_204", 0 + 1 + 2 + 3 + 4 + 5 + 6);
        rooms.push_back(r);
        r = new Room(13, "MS_545", 1 + 2);
        rooms.push_back(r);
        numOfMicrophones = 100;
    }

};



int Institute::search(Request * request, vector<Room *> rooms, int op){
    bool hit = false;
    cout<<"Search: rooms.size() is "<<rooms.size()<<"\n";
    for (int i = 0, n = rooms.size(); i < n; i++){
        cout<<"Search: Considering room "<<rooms[i]->name<<"\n";
        if (rooms[i]->occupiedBy == NULL && !hit){
            rooms[i]->occupiedBy = request;
            numOfMicrophones -= request->numOfMicrophones;
            if (op != -1){
                operators[op]->bookedTo = request;
                request->oper = operators[op];
            }
            hit = true;
        }
    }
    if (hit){
        return 0;
    }
    for (int i = 0, n = rooms.size(); i < n; i++){
        int v = search(request, rooms[i]->occupiedBy->candidates, op);
        if (v == 0){
            return 0;
        }
    }
    return -1;
}

int Institute::insert(Request * request, vector<Room *> rooms){
    cout<<"In Insert\n";
    if (request->numOfMicrophones > numOfMicrophones){
        cout<<"Error in Microphones\n";
        return -2;
    }
    int op = -1;
    if (request->requirements >= (int) pow(2, 7)){
        cout<<"Operator requirement felt\n";
        int n;
        for (op = 0, n = operators.size(); op < n; op++){
            if (operators[op]->bookedTo == NULL){
                break;
            }
        }
        if (op == n){
            return -3;
        }
    }
    
    bool hit = false;
    cout<<"Insert: rooms.size() is "<<rooms.size()<<"\n";
    
    for (int i = 0, n = rooms.size(); i < n; i++){
        cout<<"Insert: Considering room "<<rooms[i]->name<<"\n";
        if ((rooms[i]->resources | request->requirements) != rooms[i]->resources){
            bool t = rooms[i]->resources | request->requirements != rooms[i]->resources;
            cout<<"rooms[i]->resources is "<<rooms[i]->resources<<"\n";
            cout<<"request->requirements is "<<request->requirements<<"\n";
            cout<<"rooms[i]->resources | request->requirements is "<<(rooms[i]->resources | request->requirements)<<"\n";
            cout<<t<<"\n";
            continue;
        }
        cout<<"Insert: Room "<<rooms[i]->name<<" meets requirements.\n";
        request->candidates.push_back(rooms[i]);
        cout<<"Pushed to candidates stack\n";
        if (rooms[i]->occupiedBy == NULL && !hit){
            cout<<"HIT!\n";
            rooms[i]->occupiedBy = request;
            cout<<rooms[i]->occupiedBy->requestID<<"\n";
            numOfMicrophones -= request->numOfMicrophones;
            if (op != -1){
                operators[op]->bookedTo = request;            
                request->oper = operators[op];
            }
            hit = true;
        }
    }
    if (hit){
        return 0;
    }
    cout<<"Could not find empy rooms, considering replacements"
    return search(request, request->candidates, op);
}

int main(){
    printf("Hello world!\n");
    User * usr1 = new User(666, "sherjilozair", "Sherjil Ozair");
    
    Institute * i = new Institute();
    int v = i->insert(new Request(10, usr1, 1, 5), i->rooms);
    v = i->insert(new Request(20, usr1, 1 + 2 + 4, 5), i->rooms);
    v = i->insert(new Request(30, usr1, 1 + 2 + 4 + 8, 5), i->rooms);
    print(i);
    cout<<"return val is "<<v<<"\n";
    return 0;
}

int encode(bool resource[]){
    int e = 0;
    for (int i = 0; i < 12; i++){
        if (resource[i]){
            e += (int) pow(2, i);
        }
    }
    return e;
}

void print(Institute * day){
    for (int i = 0, n = day->rooms.size(); i < n; i++){
        Room * r = day->rooms[i];
        cout<<r->roomID<<" "<<r->name<<" "<<r->resources;
        if (r->occupiedBy != NULL){
            cout<<" "<<r->occupiedBy->requestID;
        }
        cout<<"\n";
        //printf("%d %s %d %d\n", r.roomID, r.name, r.occupiedBy->requestID, r.resources);
    }
}

void print(vector<Room *> rooms){
    cout<<"1\n";
    for (int i = 0, n = rooms.size(); i < n; i++){
        Room * r = rooms[i];
        cout<<r->roomID<<" "<<r->name<<" "<<r->occupiedBy->requestID<<" "<<r->resources<<"\n";
    }
}


// Algorithm for room allocation. in MM.
// Netoworking
// GUI

// 0 White Board
// 1 Projector
// 2 Projector with audio facilities
// 3 LCD Display
// 4 Sound System
// 5 Sound System with Microphone
// 6 Air-conditioning
// 7 Audio recording facility
// 8 Audio recording facility + Microphones for audience side
// 9 Video recording facility
// 10 Video recording facility + Microhopne for audience side
// 11 Video recording facility + Microhopne/Camera for audience side


// Microphones



