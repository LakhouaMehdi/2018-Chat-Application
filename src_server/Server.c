/* ClockServer.c: simple TCP/IP server example with timeout support
 * Author: Rainer Doemer, 2/17/15
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include "../FileIO.h"
#include "Server.h"

#define DEBUG 	/* be verbose */

/*** type definitions ****************************************************/

typedef void (*ClientHandler)(int DataSocketFD);
typedef void (*TimeoutHandler)(void);

/*** global variables ****************************************************/

const char *Program = NULL;	/* program name for descriptive diagnostics */

int Shutdown = 0;		/* keep running until Shutdown == 1 */

/*** global functions ****************************************************/

/* print error diagnostics and abort */
void FatalError(const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

/* creates a socket on this server */
int MakeServerSocket( uint16_t PortNo )
{
    int ServSocketFD;
    struct sockaddr_in ServSocketName;

    /* create the socket */
    ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   
	FatalError("service socket creation failed");
    }

    /* bind the socket to this server */
    ServSocketName.sin_family = AF_INET;
    ServSocketName.sin_port = htons(PortNo);
    ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind(ServSocketFD, (struct sockaddr*)&ServSocketName,sizeof(ServSocketName) ) < 0)
    {  
	 FatalError("binding the server to a socket failed");
    }

    /* start listening to this socket */
    if (listen(ServSocketFD, 5) < 0)	/* max 5 clients in backlog */
    {   
	FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */


void TimeOutFunction(void)
{
    char ClockBuffer[26] = "";
    time_t CurrentTime; 
    char   *TimeString;	

    CurrentTime = time(NULL);	/* get current real time (in seconds) */
    TimeString = ctime(&CurrentTime); /* convert to printable format */
    strncpy(ClockBuffer, TimeString, 25);
    ClockBuffer[24] = 0;	/* remove unwanted '/n' at the end */
    printf("\r%s : Server has nothing to do.", ClockBuffer); /* print from the beginning of current line */
    fflush(stdout);
} /* end of TimeOutFunction */

void ProcessRequest( int DataSocketFD )	/* process a request by a client */
{
    char RecvBuf[256];	/* message buffer for receiving a message */
    char SendBuf[256];	/* message buffer for sending a response */

    int n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0) 
    {
	FatalError("reading from data socket failed");
    }

    RecvBuf[n] = 0;

#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
   
    int numberOfParameters = 0;
    char** command = Deconstruct(RecvBuf, '/',  &numberOfParameters);

    int currentIndex = 0;
    while(command[currentIndex] != NULL)
    {
 	printf("%s\n", command[currentIndex]);	
	currentIndex++;
    }

/************************************* LOGIN/username/password **************************************************/ 

    if (numberOfParameters == 3 && strcmp(command[0], "LOGIN") == 0)
    {
	//must check file if it is a valid user/password combination
	if( IsInUsers(command[1] , command[2]) )
	{
	    strncpy(SendBuf, "OK_LOGIN", sizeof(SendBuf)-1);
   	}
	else
	{
	    strncpy(SendBuf, "DENIED", sizeof(SendBuf)-1); 
	}
    }

/************************************* REGISTER/username/password ***********************************************/ 

    else if (numberOfParameters == 3 && strcmp(command[0], "REGISTER") == 0)
    {   
	if( !IsValidUsername(command[1]) )
	{
	    strncpy(SendBuf, "USERNAME_INVALID", sizeof(SendBuf)-1); 	
	}
	else if ( !IsValidPassword(command[2]) )
	{
	    strncpy(SendBuf, "PASSWORD_INVALID", sizeof(SendBuf)-1); 
	}
	else if ( !IsNotTakenUsername(command[1]))
	{
	    strncpy(SendBuf, "USERNAME_TAKEN", sizeof(SendBuf)-1); 
	}
	else
	{
	    //Must add account to the file
	    AddUser(command[1], command[2]);
	    strncpy(SendBuf, "OK_REGISTER", sizeof(SendBuf)-1);
	}
	
    }

/************************************* ADD/clientUsername/personToAdd *******************************************/ 

    else if (numberOfParameters == 3 && strcmp(command[0], "ADD") == 0)
    {
	if( (strcmp(command[1], command[2]) ==0) || IsNotTakenUsername(command[2]) )
	{
  	    strncpy(SendBuf, "USERNAME_DOES_NOT_EXIST", sizeof(SendBuf)-1);
	}
	else if ( IsInContactList( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "USER_ALREADY_ADDED", sizeof(SendBuf)-1);
	}
	else if ( IsInFriendRequests( command[2] , command[1] ) ) 
	{
 	    strncpy(SendBuf, "REQUEST_ALREADY_SENT", sizeof(SendBuf)-1);
	}
	else if ( IsInFriendRequests( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "REQUEST_ALREADY_RECEIVED", sizeof(SendBuf)-1);
	}
	else
	{
	    //add command[1] to command[2] 's friend requests file
	    AddFriendRequest(command[2], command[1]);
	    strncpy(SendBuf, "OK_ADD", sizeof(SendBuf)-1);
	}
    }

/************************************* REMOVE/clientUsername/personToRemove *************************************/ 
 
   else if (numberOfParameters == 3 && strcmp(command[0], "REMOVE") == 0)
   {
	if ( !IsInContactList( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "USER_NOT_IN_CONTACTS", sizeof(SendBuf)-1);
	}
	else
	{
	    //remove command[2] from command[1] 's contact file
	    //remove command[1] from command[2] 's contact file
	    RemoveContact(command[1], command[2]);
	    RemoveContact(command[2], command[1]);
	    strncpy(SendBuf, "OK_REMOVE", sizeof(SendBuf)-1);
	}
    }

/************************************* ACCEPT_REQUEST/clientUsername/personToAccept *******************************/ 
 
    else if (numberOfParameters == 3 && strcmp(command[0], "ACCEPT_REQUEST") == 0)
    {
	if ( !IsInFriendRequests( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "REQUEST_NOT_FOUND", sizeof(SendBuf)-1);
	}
	else if ( !IsInContactList( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "ALREADY_IN_CONTACTS", sizeof(SendBuf)-1);
	}
	else
	{
	    //remove command[2] from command[1] 's friend requests
	    //add command[2] to command[1] 's contacts
	    //add command[1] to command[2] 's contacts
	    RemoveRequest(command[1], command[2]);
	    AddContact(command[1], command[2]);
	    AddContact(command[2], command[1]);
	    strncpy(SendBuf, "OK_ACCEPT_REQUEST", sizeof(SendBuf)-1);
	}
    }

/************************************* IGNORE_REQUEST/clientUsername/personToIgnore *******************************/ 
 
    else if (numberOfParameters == 3 && strcmp(command[0], "IGNORE_REQUEST") == 0)
    {
	if ( !IsInFriendRequests( command[1] , command[2] ) ) 
	{
 	    strncpy(SendBuf, "REQUEST_NOT_FOUND", sizeof(SendBuf)-1);
	}
	else
	{
	    //remove command[2] from command[1] 's friend requests
	    RemoveRequest(command[1], command[2]);
    	    strncpy(SendBuf, "OK_IGNORE_REQUEST", sizeof(SendBuf)-1);
	}
    }

/************************************* GET_CONTACTS/clientUsername ************************************************/ 
 
    else if (numberOfParameters == 2 && strcmp(command[0], "GET_CONTACTS") == 0)
    {
	strcpy(SendBuf, "OK_CONTACTS");
	strcat(SendBuf, GetContacts(command[1]));
    }

/************************************* GET_FRIEND_REQUESTS/clientUsername *****************************************/  

    else if (numberOfParameters == 2 && strcmp(command[0], "GET_FRIEND_REQUESTS") == 0)
    {
	strcpy(SendBuf, "OK_REQUESTS");
	strcat(SendBuf, GetRequests(command[1]));	
    }

/************************************************ SHUTDOWN ********************************************************/  

    else if (0 == strcmp(RecvBuf, "SHUTDOWN"))
    {	
	strncpy(SendBuf, "OK_SHUTDOWN", sizeof(SendBuf)-1);
	Shutdown = 1;
    }

/************************************************ ELSE ************************************************************/  
    
    else
    { 
	strncpy(SendBuf, "ERROR unknown command", sizeof(SendBuf)-1);
    }

    SendBuf[sizeof(SendBuf)-1] = 0;

    int l = strlen(SendBuf);

#ifdef DEBUG
    printf("%s: Sending response: %s\n", Program, SendBuf);
#endif

    n = write(DataSocketFD, SendBuf, l);
    if (n < 0)
    {   
	FatalError("writing to data socket failed");
    }
} /* end of ProcessRequest */

void ServerMainLoop(		/* simple server main loop */
	int ServSocketFD,		/* server socket to wait on */
	ClientHandler HandleClient,	/* client handler to call */
	TimeoutHandler HandleTimeout,	/* timeout handler to call */
	int Timeout)			/* timeout in micro seconds */
{
    int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in ClientAddress;	/* client address we connect with */
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    fd_set ReadFDs;	/* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */

    while(!Shutdown)
    {
	ReadFDs = ActiveFDs;
	TimeVal.tv_sec  = Timeout / 1000000;	/* seconds */
	TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
	/* block until input arrives on active sockets or until timeout */
	res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (res < 0)
	{   
	    FatalError("wait for input or timeout (select) failed");
	}

	if (res == 0)	/* timeout occurred */
	{
	    HandleTimeout();
	}
	else		/* some FDs have data ready to read */
	{ 
	    for(i=0; i<FD_SETSIZE; i++)
	    {   if (FD_ISSET(i, &ReadFDs))
		{   if (i == ServSocketFD)
		    {	
			/* connection request on server socket */
#ifdef DEBUG
			printf("%s: Accepting new client...\n", Program);
#endif

			ClientLen = sizeof(ClientAddress);
			DataSocketFD = accept(ServSocketFD,
				(struct sockaddr*)&ClientAddress, &ClientLen);
			if (DataSocketFD < 0)
			{   
			    FatalError("data socket creation (accept) failed");
			}
#ifdef DEBUG
			printf("%s: New client connected from %s:%hu.\n", 
				Program, 
				inet_ntoa(ClientAddress.sin_addr), 
				ntohs(ClientAddress.sin_port));
#endif 

			FD_SET(DataSocketFD, &ActiveFDs);
		    }
		    else
		    {   /* active communication with a client */
#ifdef DEBUG
			printf("%s: Dealing with client on FD%d...\n", Program, i);
#endif

			HandleClient(i);

#ifdef DEBUG
			printf("%s: Closing client connection FD%d.\n", Program, i);
#endif
			close(i);
			FD_CLR(i, &ActiveFDs);
		    }
		}
	    }
	}
    }
} /* end of ServerMainLoop */

/*** main function *******************************************************/

int main(int argc, char *argv[])
{
    int ServSocketFD;	/* socket file descriptor for service */
    int PortNo;		/* port number */

    Program = argv[0];	/* publish program name (for diagnostics) */

#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    if (argc < 2)
    {   
	fprintf(stderr, "Usage: %s port\n", Program);
	exit(10);
    }

    PortNo = atoi(argv[1]);	/* get the port number */

    if (PortNo <= 2000)
    {   
	fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }

#ifdef DEBUG
    printf("%s: Creating the server socket...\n", Program);
#endif
    ServSocketFD = MakeServerSocket(PortNo);
    printf("%s: Chat server ready at port %d...\n", Program, PortNo);
    ServerMainLoop(ServSocketFD, ProcessRequest, TimeOutFunction, 250000);
    printf("\n%s: Shutting down.\n", Program);
    close(ServSocketFD);
    return 0;
}


/* EOF Server.c */
