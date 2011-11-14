#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdio.h>
#include <vector>
#include <string>
#include <queue>
#include <stdlib.h>
#include <queue>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define TEMP 10
#define CONFIRMED 20
#define UNMET 5

using namespace std;

//extern vector<Request *> requests;
//extern vector<Room *> rooms;

class Room;
class User;
class Request;
class Institute;
class Operator;
class Time;

bool isAvailable(Request *, Room *);
int encode(bool[]);
void print(vector<Room * >);
void print(Institute *);
int unbook(Request *, Room *);
int book(Request *, Room *);
int populate(vector<Request *>, vector<Room *>);

vector<string> &split_(const string &, char, vector<string> &);
vector<string> split_(const string &, char );

class Time{
public:
    int hours;
    int minutes;
    bool operator< (const Time& other){
        if (hours < other.hours){
            return true;
        }
        else if (hours == other.hours){
            if (minutes < other.minutes){
                return true;
            }
        }
        return false;
    }
    bool operator> (const Time& other){
        if (hours > other.hours){
            return true;
        }
        else if (hours == other.hours){
            if (minutes > other.minutes){
                return true;
            }
        }
        return false;
    }
    bool operator== (const Time& other){
        return hours == other.hours && minutes == other.minutes;
    }
    bool operator>= (const Time& other){
        return *this > other || *this == other;
    }
    bool operator<= (const Time& other){
        return *this < other || *this == other;
    }
    
    Time(int hours, int minutes){
        this->hours = hours;
        this->minutes = minutes;
    }
    
    Time(){
        
    }
};

class User{
public:
    string username;
    string completename;
    int userID;
    int priority;
    string pass;
    User(int userID, string username, string pass, string completename, int priority){
        this->userID = userID;
        this->username = username;
        this->completename = completename;
        this->priority = priority;
        this->pass = pass;
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

class Request{
public:
    int requestID;
    int userID;
    int requestroomID;
    int requirements;
    int numM;
    int status;
    Time starttime, endtime;
    string date;
    vector<Room *> candidates;
    Operator * oper;
    Room * room;
    Request(int requestID, int userID, int requirements, int numM){
        this->requestID = requestID;
        this->userID = userID;
        this->requirements = requirements;
        this->numM = numM;
        this->requestroomID = 0;
        this->status = UNMET;
    }
    void print(){
        cout<<"requestID is : "<<requestID<<endl;
        cout<<"userID is : "<<userID<<endl;
        cout<<"requestroomID is : "<<requestroomID<<endl;
        cout<<"requirements is : "<<requirements<<endl;
        cout<<"numM is : "<<numM<<endl;
        cout<<"starttime is : "<<starttime.hours<<":"<<starttime.minutes<<endl;
        cout<<"endtime is : "<<endtime.hours<<":"<<endtime.minutes<<endl;
        cout<<"date is : "<<date<<endl;
        
    }
};

class Room{
public:
    string name;
    int roomID; 
    vector<Request *> bookings;
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

#endif