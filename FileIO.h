//*********************************************************************/
/* Project 2 QuickCHat, for EECS 22L, Winter 2018	                  */
/*                                                                	  */
/* Authors: David Tiao, Team 17                                       */
/* Date: 3/1/2018                                                 	  */
/*                                                               	  */
/* FileIO.h: header file for input/output			            	  */
/*								  									  */
/*	3/1/2018 David Tiao Initial version 		     			  	  */
/*												     				  */		
/*                                                                    */
//*********************************************************************/
#include <stdbool.h>
#include "src_server/ClientList.h"

char** Deconstruct(char* str, const char delim, int* totalNames);

void FreeArray(char** array);

void AddUser(char* username, char* password);
void AddContact (char* client, char* friend);
void AddFriendRequest(char* client, char* request);


void RemoveRequest(char* client, char* request);
void RemoveContact(char* client, char* friend);


char* GetRequests(char *client);
char* GetContacts(char *client);

bool IsInUsers(char *user, char *password);
bool IsInContactList(char* client, char* friend);
bool IsInFriendRequests(char* client, char* friend);
bool IsNotTakenUsername(char* username);

bool IsValidUsername(char* username);
bool IsValidPassword(char* password); 
