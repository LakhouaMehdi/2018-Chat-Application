#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "Client.h"

/*** network imports *****************************************************/
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

/*** GUI imports *********************************************************/
#ifndef CLIENTMODULETEST
    #include <gtk/gtk.h>
    #include <stdbool.h>
    #include "ClientGui.h"
#endif
/*** global variables ****************************************************/

const char *Program = NULL;	/* program name for descriptive diagnostics */

struct sockaddr_in ServerAddress; /* server address we connect with */

#ifndef CLIENTMODULETEST
GtkWidget* window;
#endif

/*** global functions ****************************************************/

/* print error diagnostics anad abort */
void FatalError( const char *ErrorMsg )
{
#ifndef CLIENTMODULETEST
    Quick_Error_Message(window, "Fatal Error!", ErrorMsg);
    exit(20);
#else
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
#endif

} /* end of FatalError */

/* communicate with the server */
char* Talk2Server( const char *Message, char *RecvBuf)
{
    int n;
    int SocketFD;

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {
	FatalError("socket creation failed");
    }

#ifdef DEBUG
    printf("%s: Connecting to the server at port %d...\n",
	Program, ntohs(ServerAddress.sin.port));
#endif

    if (connect(SocketFD, (struct sockaddr*)&ServerAddress, 
	sizeof(struct sockaddr_in)) <0)
    {
	FatalError("connecting to server failed");
    }

#ifdef DEBUG
    printf("%s: Sending message '%s'...\n", Program, Message);
#endif

    n = write(SocketFD, Message, strlen(Message));
    if (n<0)
    {
	FatalError("writing to socket failed");
    }

#ifdef DEBUG
    printf("%s: Waiting for response...\n", Program);
#endif

    n = read(SocketFD, RecvBuf, BUFFSIZE-1);
    if (n < 0)
    {
	FatalError("reading from socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received response: %s\n", Program, RecvBuf);
    printf("%s: Closing the connection...\n", Program);
#endif
    close(SocketFD);
    return(RecvBuf);
} /* end of Talk2Server */

#ifndef CLIENTMODULETEST
/* the main function */
int main(int argc, char *argv[])
{
    int	PortNo;		/* port number */
    struct hostent* Server;	/* server host */
    
    Program = argv[0];	/* publish program name (for diagnostics) */
    if (argc < 3)
    {   
	fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }

    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   
	fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }

    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   
	fprintf(stderr, "%s: invalid port number %d, should be >2000\n", Program, PortNo);
        exit(10);
    }

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];


    /*Build the GUI*/
    gtk_init(&argc, &argv); //Initialize the GTK libraries. 

    // Create the window
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL ); 		       // Create the main top level window
    gtk_window_set_title( GTK_WINDOW(window), argv[0] );           
    gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER ); 

    gtk_widget_set_size_request( window, WINDOW_WIDTH, WINDOW_HEIGHT );                  
    gtk_window_set_resizable( GTK_WINDOW(window), RESIZE_WINDOW );
    gtk_container_set_border_width( GTK_CONTAINER(window), 0 );

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

 //   Add_Background(window,"../res/WorldMap.PNG");
    // Add the content box to the window
    gtk_container_add( GTK_CONTAINER(window), Create_Login_Box() );

    gtk_widget_show_all(window);

    // start the main loop, handle all registered events
    gtk_main();

    return 0;
}
#else
int main(int argc, char *argv[])
{
    int	PortNo;		/* port number */
    struct hostent* Server;	/* server host */

    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];  /* message buffer for receiving a message*/   
    const char* Response;
    
    Program = argv[0];	/* publish program name (for diagnostics) */

    if (argc < 3)
    {   
	fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }

    Server = gethostbyname(argv[1]);

    if (Server == NULL)
    {   
	fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }

    PortNo = atoi(argv[2]);

    if (PortNo <= 2000)
    {   
	fprintf(stderr, "%s: invalid port number %d, should be >2000\n", Program, PortNo);
        exit(10);
    }

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];

    do
    {	
	printf("%s: Enter a command to send to the server:\n"
		"	  'LOGIN' to log into an account,\n"
		"	  'REGISTER' to register an account,\n"
		"	  'ADD' to send a friend request,\n"
		"	  'REMOVE' to remove a contact,\n"
		"	  'ACCEPT' to accept a friend request,\n"
		"	  'IGNORE' to ignore a friend request,\n"
		"	  'CONTACTS' to view friend list,\n"
		"	  'REQUESTS' to view friend requests,\n"
		"         'SHUTDOWN' to terminate the server,\n"
		"         or 'LOGOUT' to quit this client\n"
		"command: ", argv[0]);

	fgets(SendBuf, sizeof(SendBuf), stdin);
	int l = strlen(SendBuf);

	if (SendBuf[l-1] == '\n')
	{   
	    SendBuf[--l] = 0; 
	}

	if (0 == strcmp("LOGOUT", SendBuf))
	{   
	    break; 
        }

	if (l != 0)
	{   
            Response = Talk2Server(SendBuf, RecvBuf); 
	    printf("%s: Received response: %s\n", Program, Response);	    
	}

    } while( strcmp("SHUTDOWN", SendBuf) != 0 );

    printf("%s: Exiting...\n", Program);
    return 0;
}

#endif

bool PasswordsMatch(char *p1, char *p2)
{
	if(strcmp(p1, p2) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* EOF Client.c */
