//r.candidates should be populated before calling arrange
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

using namespace std;

#include "../inc/algorithms.h"

extern vector<Request *> requests;
extern vector<Room *> rooms;
extern vector<User *> users;

int arrange(vector<Request *> requests, int requestindex){
    //cout<<"Entering `arrange` with requestindex "<<requestindex<<"\n";
    if (requestindex == -1){
        return 1;
    }
    
    Request * r = requests[requestindex];
    //cout<<"Starting for loop\n";
    for (int j = 0, m = r->candidates.size(); j < m; j++){
        //cout<<"Iteration number "<<j<<"\n";
        if (!isAvailable(r, r->candidates[j])){
            //cout<<"Room unavailable. Continuing...\n";
            continue;
        }
        //cout<<"Got a Hit ! Booking\n";
        book(r, r->candidates[j]);
        ////cout<<"Calling `arrange` with requestindex "<<(requestindex - 1)<<"\n";
        int ret = arrange(requests, requestindex - 1);
        //cout<<"Returning from `arrange` of requestindex"<<(requestindex - 1)<<"\n";
        if (ret == 1){
            //cout<<"Exiting from `arrange` of requestindex "<<requestindex<<"\n";
            return 1;
        }
        else{
            //cout<<"Unbooking...\n";
            unbook(r, r->candidates[j]);
        }
    }
    //cout<<"Couldn't find any match\n";
    return -1;
}

bool isAvailable(Request * request, Room * room){
    for (int i = 0, n = room->bookings.size(); i < n; i++){
        Request * request2 = room->bookings[i];
        if ((request->date.compare(request2->date) == 0) && ((request->starttime >= request2->starttime && request->starttime < request2->endtime) || (request->endtime > request2->starttime && request->endtime <= request2->endtime))){
            return false;
        }
    }
    return true;
}

int populate(vector<Request *> requests, vector<Room *> rooms){
    for (int i = 0, n = requests.size(); i < n; i++){
        //cout<<"Processing request number "<<i<<"\n";
        Request * re = requests[i];
        for (int j = 0, m = rooms.size(); j < m; j++){
            //cout<<"Checking room number "<<j<<"\n";
            Room * ro = rooms[j];
            if ((ro->resources | re->requirements) == ro->resources){
                //cout<<"ro->resources is "<<ro->resources<<"\n";
                //cout<<"re->requirements is "<<re->requirements<<"\n";
                //cout<<"ro->resources | re->requirements is "<<(ro->resources | re->requirements)<<"\n";
                //cout<<"Found candidate room !\n";
                re->candidates.push_back(ro);
            }
        }
    }
    return 1;
}

int book(Request * request, Room * room){
    room->bookings.push_back(request);
    request->room = room;
    request->status = TEMP;
    return 1;
}

int unbook(Request * request, Room * room){
    request->room = NULL;
    for (int i = 0, n = room->bookings.size(); i < n; i++){
        if (room->bookings[i] == request && room->bookings[i]->status == TEMP){
            room->bookings.erase(room->bookings.begin() + i);
            return 1;
        }
    }
    return -1;
}

void print(vector<Room *> rooms){
    cout<<"Printing room data\n";
    for (int i = 0, n = rooms.size(); i < n; i++){
        Room * r = rooms[i];
        cout<<r->roomID<<" "<<r->name<<" "<<r->resources;
        cout<<"\n";
        for (int i = 0, n = r->bookings.size(); i < n; i++){
            cout<<"\t"<<r->bookings[i]->starttime.hours<<":"<<r->bookings[i]->starttime.minutes<<" "<<r->bookings[i]->endtime.hours<<":"<<r->bookings[i]->endtime.minutes<<" "<<r->bookings[i]->requestID<<" "<<r->bookings[i]->requirements<<"\n";
        }
        cout<<"\n";
    }
    cout<<"Exiting from Room\n";
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

vector<string> &split_(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split_(const string &s, char delim) {
    vector<string> elems;
    return split_(s, delim, elems);
}

/*
int main(int argc, char * argv[]){
    
    for (int i = 0; i < 70; i++){
        requests.push_back(new Request(i + 1, new User(100 + i, "sherjilozair", "Sherjil Ozair"), pow(2, i % 12), 2));
        requests[i]->starttime = Time(4 + abs(12 - i) % 60, i%2?30:0);
        requests[i]->endtime = Time(5 + abs(12 - i) % 60, i%2?30:0);
    }
    
    readRoomData("roomdata");
    
    for (int i = 0; i < 36; i++){
        rooms.push_back(new Room(i + 1, "name", -1));
    }
    
    
    populate(requests, rooms);
    //cout<<"1\n";
    arrange(requests, requests.size() - 1);
    //arrangeForMics(requests);
    //arrangeForOperators(requests, operators);

    print(rooms);
    
    return -10;
}
*/

// Mics and operators
// Waiting List
