#include <string.h>
#include <GL/glui.h>
#include <iostream>
#include "../inc/network_codes.h"
#include "../inc/algorithms.h"

using namespace std;

int   main_window;
int   wireframe = 0;
int   obj = 0;
int   segments = 8;
string username_text = "";
string password_text = "";
string ip_addr_text = "";

void start(string, string);

GLUI_Checkbox   *checkbox;
GLUI_Spinner    *spinner;
GLUI_RadioGroup *radio;
GLUI_EditText   *username_box, *password_box, *ip_addr_box;
int cb[12];


string date_str = "", starttime_str = "", endtime_str = "";

int gi_userID, gi_requestID;

int createClient(const char *);
void ip(int);
void auth(int);


void control_cb( int control )
{
  printf( "callback: %d\n", control );
  for (int i = 0; i < 12; i++)
  {
  	cout << cb[i] << endl;
  }
}

void questionnaire(int, int);
void re_questionnaire(int control){
	questionnaire(gi_userID, gi_requestID);
}

void congos(int retval){

	GLUI_Master.close_all();
	GLUI *gluiauth = GLUI_Master.create_glui( "Workflow-based Room Booking System", 0, 400, 200 ); /* name, flags, x, and y */
	cout <<"in IP\n";
	GLUI_Panel *obj_panel = new GLUI_Panel( gluiauth, "Congratulations" );
	if (retval == 1){
		new GLUI_StaticText( obj_panel, "Your room request has been booked. You will be notified about the status tomorrow." );
	}
	else if (retval == 0){
		new GLUI_StaticText( obj_panel, "Couldn't book room. Please try again." );
	}
	GLUI_Button * connect_button = new GLUI_Button( obj_panel, "Make Another Request", 1, auth );
	cout <<"in IP 2\n";	
}



void makeRequest(Request);

void request(int control)
{
cout << "In request\n";

bool cb_b[12];
for (int i = 0; i < 12; i++){
	cb_b[i] = cb[i]==1?true:false;
}

int requirements_int = encode(cb_b);
cout << "requirements_int is "<< requirements_int<<endl;
int numM = 0;
if (requirements_int >= 32){
	numM = 1;
}
Request * re = new Request(gi_requestID, gi_userID, requirements_int, numM);
re->date = date_str;

vector<string> st_x = split_(starttime_str, ':');
vector<string> et_x = split_(endtime_str, ':');

re->starttime = Time(atoi(st_x[0].c_str()), atoi(st_x[1].c_str()));
    re->endtime = Time(atoi(et_x[0].c_str()), atoi(et_x[1].c_str()));

makeRequest(*re);

}
	
void questionnaire(int userID, int requestID)
{
	gi_userID = userID;
	gi_requestID = requestID;
	cout << "Constructing questionnaire\n";
	GLUI_Master.close_all();
	GLUI *gluiauth = GLUI_Master.create_glui( "Workflow-based Room Booking System", 0, 400, 200 ); /* name, flags, x, and y */

	GLUI_Panel *obj_panel = new GLUI_Panel( gluiauth, "Questionnaire" );
	
	//GLUI_RadioGroup *radio = new GLUI_RadioGroup( obj_panel,&obj,4,control_cb );
	
	const char * name[] = {
		"Air-Conditioner", 
		"White Board", 
		"LCD Display", 
		"Projector", 
		"Projector with audio facilities", 
		"Sound System", 
		"Sound System with Microphones for the Audience", 
		"Audio Recording Facility", 
		"Audio Recording Facility with Microphones for the Audience", 
		"Video Recording Facility", 
		"Video Recording Facility with Microphones for the Audience", 
		"Video Recording Facility with Camera for the Audience"
	};
	
	for (int i = 0; i < 12; i++){
		GLUI_Checkbox *checkbox = new GLUI_Checkbox(obj_panel, name[i], &cb[i], 10, control_cb);
	}
	

	GLUI_EditText *date = new GLUI_EditText( obj_panel, "Date (dd/mm/yyyy) :", date_str, 5, control_cb );
	GLUI_EditText *starttime = new GLUI_EditText( obj_panel, "Start Time (hh:mm) :", starttime_str, 5, control_cb );
	GLUI_EditText *endtime = new GLUI_EditText( obj_panel, "End Time (hh:mm) :", endtime_str, 5, control_cb );
	
	GLUI_Button * request_button = new GLUI_Button( obj_panel, "Make Request", 1, request );
	
  cout << "Here\n";
}

void authorize(int control)
{
	cout<<"In Submit !\n";
	start(username_text, password_text);
}


int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
  
  //GLUI *gluiauth = GLUI_Master.create_glui( "Workflow-based Room Booking System", 0, 400, 200 ); /* name, flags, x, and y */
  


	ip(0);
	  //GLUI_Master.set_glutIdleFunc( NULL );

	cout << "1\n";
  glutMainLoop();
  cout << "2\n";
  return EXIT_SUCCESS;

}


void ip(int control)
{
	
	GLUI_Master.close_all();
	GLUI *gluiauth = GLUI_Master.create_glui( "Workflow-based Room Booking System", 0, 400, 200 ); /* name, flags, x, and y */
	cout <<"in IP\n";
	GLUI_Panel *obj_panel = new GLUI_Panel( gluiauth, "Connection" );
	ip_addr_box = new GLUI_EditText( obj_panel, "IP Address:", ip_addr_text, 5, control_cb );
	GLUI_Button * connect_button = new GLUI_Button( obj_panel, "Connect", 1, auth );
	cout <<"in IP 2\n";
}

void auth(int control)
{
	GLUI_Master.close_all();
	GLUI *gluiauth = GLUI_Master.create_glui( "Workflow-based Room Booking System", 0, 400, 200 ); /* name, flags, x, and y */
	
	createClient(ip_addr_text.c_str());
	GLUI_Panel *obj_panel = new GLUI_Panel( gluiauth, "Authorization" );
	
  username_box = new GLUI_EditText( obj_panel, "Username:", username_text, 3, control_cb );
  password_box = new GLUI_EditText( obj_panel, "Password:", password_text, 4, control_cb );
  
  GLUI_Button * submit_button = new GLUI_Button( obj_panel, "Submit", 1, authorize ); // Want to call back a function
  GLUI_Button * quit_button = new GLUI_Button( obj_panel, "Quit", 0, (GLUI_Update_CB)exit );
 	
}
