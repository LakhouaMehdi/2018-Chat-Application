//*********************************************************************/
/* Project 2 QuickCHat, for EECS 22L, Winter 2018	              */
/*                                                                    */
/* Authors: David Tiao, Team 17                                       */
/* Date: 3/1/2018                                                     */
/*                                                                    */
/* FileIO.h: header file for input/output			      */
/*			 					      */
/*	3/1/2018 David Tiao Initial version 		              */
/*				  				      */		
/*                                                                    */
//*********************************************************************/

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FileIO.h"

char** Deconstruct(char* a_str, const char a_delim, int* totalNames)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    *totalNames = count;
   
    /* Add space for terminating null string so caller
 *        knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = token;
 
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    
    return result;
}

void AddUser(char* username, char* password)
{
	// Pointers to filename formatted data
	char fileName[256];
	char formattedData[512];
	
	// Checks to see if directory exists, if not, creates directory
	struct stat st = {0};
	if (stat("UserData", &st) == -1) 
	{
		mkdir("UserData", 0755);
	}	
	
	// Change to the new directory
	chdir("UserData");

	// Formats the file name

	strcpy(fileName, "UserData.txt");
	
	// Formats the data for writing
	strcpy(formattedData, username);
	strcat(formattedData, "/");
	strcat(formattedData, password);
	
	
	// Define File Pointers
	FILE *currentContactsFile = NULL;
  
    // Open/create file in append/read mode
    currentContactsFile = fopen(fileName, "a");
	
	assert(currentContactsFile);
	
	fputs(formattedData, currentContactsFile); // Put the contact to the end of the file
	fputs("\n", currentContactsFile);	// Put a new line character
	fclose(currentContactsFile);		// Close the File
	
	// Change back to the previous directory
	chdir("../");

	//Create the contacts file for the user
	AddContact(username, "");
	
	//Create the requests file for the user
	AddFriendRequest(username, "");
	
}

void AddContact(char* client, char* friend)
{
	// Pointer to filename
	char fileName[256];
	
	// Checks to see if directory exists, if not, creates directory
	struct stat st = {0};
	if (stat("UserContacts", &st) == -1) 
	{
		mkdir("UserContacts", 0755);
	}	
	
	// Change to the new directory
	chdir("UserContacts");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_contacts.txt");
	
	// Define File Pointers
	FILE *currentContactsFile = NULL;
     
    // Open/create file in append/read mode
    currentContactsFile = fopen(fileName, "a");
	if(strcmp (friend, "") != 0 )
	{
	    fputs(friend, currentContactsFile); // Put the contact to the end of the file
	    fputs("\n", currentContactsFile);	// Put a new line character
	}
	fclose(currentContactsFile);		// Close the File
	
	// Change back to the previous directory
	chdir("../");
	
}


void AddFriendRequest(char* client, char* request)
{
	// Pointer to filename
	char fileName[256];
	
	// Checks to see if directory exists, if not, creates directory
	struct stat st = {0};
	if (stat("UserRequests", &st) == -1) 
	{
		mkdir("UserRequests", 0755);
	}	
	
	// Change to the new directory
	chdir("UserRequests");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_requests.txt");
	
	// Define File Pointers
	FILE *currentContactsFile = NULL;
     
    // Open/create file in append/read mode
    currentContactsFile = fopen(fileName, "a");
	if(strcmp(request, "") != 0)
	{
	    fputs(request, currentContactsFile); // Put the contact to the end of the file
	    fputs("\n", currentContactsFile);	// Put a new line character
	}
	fclose(currentContactsFile);		// Close the File
	
	// Change back to the previous directory
	chdir("../");
	
}

void RemoveRequest(char* client, char* request)
{
	// Pointer to filename
	char fileName[256];
	char buffer[512];
	int lineCount = 0;
	char character;
	
	// Change to the new directory
	chdir("UserRequests");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_requests.txt");
	
	// Define File Pointers
	FILE *currentRequestFile, *newRequestFile = NULL;
     
    // Open/create file in append/read mode
    currentRequestFile = fopen(fileName, "r");
	
	// Create a new temporary storage file
	newRequestFile = fopen("temp.txt", "w");
	
	// Counts total number of lines
	for (character = getc(currentRequestFile); character != EOF; character = getc(currentRequestFile))
		
        if (character == '\n') // Increment count if this character is newline
		
		{
            lineCount = lineCount + 1;
		}
	
	
	// Go back to the start of the file
	rewind(currentRequestFile);
	
	
	// While not end of the file, get each line of the existing requests, copies all except the entry to be removed
	for (int x = 0; x < lineCount; x++)
	{
		fgets(buffer, 512, currentRequestFile);
		
		if(strstr(buffer, request) != NULL)
		{
			continue;
		}
		
		else
		{
			fputs(buffer, newRequestFile);
		}
	}
	

	// Close the File
	fclose(currentRequestFile);		
	
	// If the new request file was valid
	if(newRequestFile != NULL)
	{
		// Close the File
		fclose(newRequestFile);	
		// Delete Original File
		remove(fileName);
		// Rename the file currentContactsFile to original name
		rename("temp.txt", fileName);

	}
	 
	// Change back to the previous directory
	chdir("../");
	 
}

void RemoveContact(char* client, char* friend)
{
	// Pointer to filename
	char fileName[256];
	char buffer[512];
	int lineCount = 0;
	char character;
	
	// Change to the new directory
	chdir("UserRequests");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_contacts.txt");
	
	// Define File Pointers
	FILE *currentRequestFile, *newRequestFile = NULL;
     
    // Open/create file in append/read mode
    currentRequestFile = fopen(fileName, "r");
	
	// Create a new temporary storage file
	newRequestFile = fopen("temp.txt", "w");
	
	// Counts total number of lines
	for (character = getc(currentRequestFile); character != EOF; character = getc(currentRequestFile))
		
        if (character == '\n') // Increment count if this character is newline
		
		{
            lineCount = lineCount + 1;
		}
	
	
	// Go back to the start of the file
	rewind(currentRequestFile);
	
	
	// While not end of the file, get each line of the existing requests, copies all except the entry to be removed
	for (int x = 0; x < lineCount; x++)
	{
		fgets(buffer, 512, currentRequestFile);
		
		if(strstr(buffer, friend) != NULL)
		{
			continue;
		}
		
		else
		{
			fputs(buffer, newRequestFile);
		}
	}
	

	// Close the File
	fclose(currentRequestFile);		
	
	// If the new request file was valid
	if(newRequestFile != NULL)
	{
		// Close the File
		fclose(newRequestFile);	
		// Delete Original File
		remove(fileName);
		// Rename the file currentContactsFile to original name
		rename("temp.txt", fileName);

	}
	 
	// Change back to the previous directory
	chdir("../");
	 
}

char *GetContacts(char *client)
{
	// Initialize Pointers and Variables
	FILE *contactsFilePointer;
	char buffer[512];
	char fileName[256];
	char *strUser = malloc(sizeof(char) * 6000);
	int lineCount = 0;
	char character;
	
	// Initialize strUser
	strcpy(strUser, "\0");
	
	// Change to the new directory
	chdir("UserContacts");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_contacts.txt");
	
	// Opens the UserData File as read only
	contactsFilePointer = fopen(fileName, "r");
	
	
	// If the file cannot be opened
	if((contactsFilePointer) == NULL) 
	{
		perror("Unable to read file. Aborting...");
		exit(5);
	}
	
	// Counts total number of lines
	for (character = getc(contactsFilePointer); character != EOF; character = getc(contactsFilePointer))
		
        if (character == '\n') // Increment count if this character is newline
		
		{
            lineCount = lineCount + 1;
		}
	
	
	// Go back to the start of the file
	rewind(contactsFilePointer);
	
	
	// While not end of the file, get each line of the existing requests, copies all except the entry to be removed
	for (int x = 0; x < lineCount; x++)
	{
		fgets(buffer, 512, contactsFilePointer);
		strtok(buffer, "\n");
		strcat(strUser, "/");
		strcat(strUser, buffer);
	}

	// Add terminating 0
	strcat(strUser, "\0");
	
	// Close the file if still open.
	if(contactsFilePointer) 
	{
		fclose(contactsFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	return strUser;
	
}


char *GetRequests(char *client)
{
	// Initialize Pointers and Variables
	FILE *requestsFilePointer;
	char buffer[512];
	char fileName[256];
	char *strRequests = malloc(sizeof(char) * 6000);
	int lineCount = 0;
	char character;
	
	// Initialize strRequests
	strcpy(strRequests, "\0");
	
	// Change to the new directory
	chdir("UserRequests");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_requests.txt");
	
	// Opens the UserData File as read only
	requestsFilePointer = fopen(fileName, "r");
	
	
	// If the file cannot be opened
	if((requestsFilePointer) == NULL) 
	{
		perror("Unable to read file. Aborting...");
		exit(5);
	}
	
	// Counts total number of lines
	for (character = getc(requestsFilePointer); character != EOF; character = getc(requestsFilePointer))
		
        if (character == '\n') // Increment count if this character is newline
		
		{
            lineCount = lineCount + 1;
		}
	
	
	// Go back to the start of the file
	rewind(requestsFilePointer);
	
	
	// While not end of the file, get each line of the existing requests, copies all except the entry to be removed
	for (int x = 0; x < lineCount; x++)
	{
		fgets(buffer, 512, requestsFilePointer);
		strtok(buffer, "\n");
		strcat(strRequests, "/");
		strcat(strRequests, buffer);
	}

	// Add terminating 0
	strcat(strRequests, "\0");
	
	// Close the file if still open.
	if(requestsFilePointer) 
	{
		fclose(requestsFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	return strRequests;
	
}



bool IsNotTakenUsername(char *username)
{
	// Initialize Pointers and Variables
	FILE *userFilePointer;
	int lineNum = 1;
	char buffer[512];
	char filename[256];
	
	// Set filename 
	strcpy(filename, "UserData.txt");

	// Checks to see if directory exists, if not, creates directory
	struct stat st = {0};
	if (stat("UserData", &st) == -1) 
	{
		mkdir("UserData", 0755);
	}	
	
	// Change to the new directory
	chdir("UserData");
	
	// Opens the UserData File as read only
	userFilePointer = fopen(filename, "r");
	
	
	// If the file cannot be opened
	if((userFilePointer) == NULL) 
	{
	
	    printf("Unable to read file 'UserData.txt'. Creating file...\n");
            FILE* currentFile = fopen("UserData.txt", "a");
    	    fclose(currentFile);

  	    userFilePointer = fopen("UserData.txt", "r");
	    if(userFilePointer == NULL)
	    {
		printf("File creation failed.\n");
		exit(5);
	    }	
	}
	


	/* Gets the next line in the file, copies it to the buffer, and as long as there is data,
	 continue checking for a match. If there is a match, return false.*/
	while(fgets(buffer, 512, userFilePointer) != NULL) 
	{
		if((strstr(buffer, username)) != NULL) 
		{
			
			// Change back to the previous directory
			chdir("../");
			return false;
		}
		lineNum++;
	}


	// Close the file if still open.
	if(userFilePointer) 
	{
		fclose(userFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	// Returns true if there is no match
   	return true;
	
}

bool IsInUsers(char *user, char *password)
{
	// Initialize Pointers and Variables
	FILE *usersFilePointer;
	char buffer[512];
	char fileName[256];
	
	// Change to the new directory
	chdir("UserData");

	// Formats the file name
	strcpy(fileName, "UserData.txt");

	
	// Opens the UserData File as read only
	usersFilePointer = fopen(fileName, "r");
	
	
	// If the file cannot be opened
	if((usersFilePointer) == NULL) 
	{
		chdir("../");
		printf("Unable to read file %s. Aborting...",fileName);
		exit(5);
	}
	
  
  /* Gets the next line in the file, copies it to the buffer, and as long as there is data,
	 continue checking for a match. If there is a match, return true.*/
	while(fgets(buffer, 512, usersFilePointer) != NULL) 
	{
		if((strstr(buffer, user)) != NULL && (strstr(buffer, password)) != NULL) 
		{
			
			// Change back to the previous directory
			chdir("../");
			// Close the file if still open.
			if(usersFilePointer) 
			{
				fclose(usersFilePointer);
			}
			return true;
			
		}
	}

	// Close the file if still open.
	if(usersFilePointer) 
	{
		fclose(usersFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	// Returns false if there is no match
   	return false;
	
}


bool IsInContactList(char *client, char *friend)
{
	// Initialize Pointers and Variables
	FILE *contactsFilePointer;
	char buffer[512];
	char fileName[256];
	
	// Change to the new directory
	chdir("UserContacts");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_contacts.txt");
	
	// Opens the UserData File as read only
	contactsFilePointer = fopen(fileName, "r");
	
	
	// If the file cannot be opened
	if((contactsFilePointer) == NULL) 
	{
		printf("Unable to read file %s. Aborting...",fileName);
		exit(5);
	}
	
  
  /* Gets the next line in the file, copies it to the buffer, and as long as there is data,
	 continue checking for a match. If there is a match, return true.*/
	while(fgets(buffer, 512, contactsFilePointer) != NULL) 
	{
		if((strstr(buffer, friend)) != NULL) 
		{
			// Change back to the previous directory
			chdir("../");
			
			// Close the file if still open.
			if(contactsFilePointer) 
			{
				fclose(contactsFilePointer);
			}
			return true;
		}
	}

	// Close the file if still open.
	if(contactsFilePointer) 
	{
		fclose(contactsFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	// Returns false if there is no match
   	return false;
}

bool IsInFriendRequests(char *client, char *friend)
{
	// Initialize Pointers and Variables
	FILE *requestsFilePointer;
	char buffer[512];
	char fileName[256];
	
	// Change to the new directory
	chdir("UserRequests");

	// Formats the file name
	strcpy(fileName, client);
	strcat(fileName, "_requests.txt");
	
	// Opens the UserData File as read only
	requestsFilePointer = fopen(fileName, "r");
	
	
	// If the file cannot be opened
	if((requestsFilePointer) == NULL) 
	{
		printf("Unable to read file %s. Aborting...",fileName);
		exit(5);
	}
	
  
  /* Gets the next line in the file, copies it to the buffer, and as long as there is data,
	 continue checking for a match. If there is a match, return true.*/
	while(fgets(buffer, 512, requestsFilePointer) != NULL) 
	{
		if((strstr(buffer, friend)) != NULL) 
		{
			
			// Change back to the previous directory
			chdir("../");
				// Close the file if still open.
			if(requestsFilePointer) 
			{
				fclose(requestsFilePointer);
			}
			return true;
		}
	}

	// Close the file if still open.
	if(requestsFilePointer) 
	{
		fclose(requestsFilePointer);
	}
	
	// Change back to the previous directory
	chdir("../");
	
	// Returns false if there is no match
   	return false;	
}



bool IsValidUsername(char* username)
{
    if(strlen(username) >= 5 && strlen(username) <= 12)
    {
	return true;
    }
    else
    {
	return false;
    }
}

bool IsValidPassword(char* password)
{
    if(strlen(password) >= 5 && strlen(password) <= 12)
    {
	return true;
    }
    else
    {
	return false;
    } 
}


