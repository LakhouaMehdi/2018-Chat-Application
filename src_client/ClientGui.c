/* Lakhoua Mehdi
 * 2/24/2018
 * Yuki Hayashi
 * Ye Myat Kyaw 
 * ClientGui.h: 
 * Source file for client Gui 
 */

#include "ClientGui.h"
#include "Client.h"
#include "Constants.h"
#include "../FileIO.h"
#include <stdbool.h>
#include <string.h>
#include <gdk/gdkx.h>

/**************************** Global variables *******************************/
static char clientUsername[BUFFSIZE] = "";

/************************* Gui callback functions ****************************/

/*** callback functions that communicate with server *************************/

static void Log_In_Button_Clicked(GtkWidget* widget, gpointer data) 
{
// get the login information from the login box

    LOGIN_BOX_CONTROLS* lbc = (LOGIN_BOX_CONTROLS*)data;
    char* user = (char*)gtk_entry_get_text( GTK_ENTRY( lbc->usernameEntry) );
    char* pass = (char*)gtk_entry_get_text( GTK_ENTRY( lbc->passwordEntry) );
   
 // verify the fields are not empty
    if(strcmp(user, "") == 0)
    {
        gtk_label_set_text(GTK_LABEL(lbc->errorLabel), "Username field is empty.");
	return;
    }
 
    if(strcmp(pass, "") == 0)
    {
        gtk_label_set_text(GTK_LABEL(lbc->errorLabel), "Password field is empty.");
	return;
    }
    
// create the message to send to the server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "LOGIN/");
    strcat(SendBuf, user );
    strcat(SendBuf, "/");
    strcat(SendBuf, pass );

    char RecvBuf[BUFFSIZE];
    const char* Response;

//  send the message to server and wait for response
#ifdef DEBUG
    printf("Sending to server: %s\n", SendBuf);	
#endif

    Response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to LOGIN", Response);
#endif

//  allow or refuse the login depending on server response
    if ( strcmp(Response, "OK_LOGIN") == 0 )
    {
        gtk_label_set_text(GTK_LABEL(lbc->errorLabel), ""); 
 
	//store the username in global variable
        strcpy(clientUsername, user);

	// login was accepted, switch to after log in window
        GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
        if(gtk_widget_is_toplevel (topLevel))
        {
  	    Set_New_Container_Content( GTK_CONTAINER(topLevel) , Create_Main_Box() ); 
        }
	
    }
    else if ( strcmp(Response, "DENIED") == 0 )
    {
        // login was rejected, update the error label
        gtk_label_set_text(GTK_LABEL(lbc->errorLabel), "Username/Password is invalid."); 
    }
    else 
    {
	//unexpected response from server. do nothing.	
        gtk_label_set_text(GTK_LABEL(lbc->errorLabel), "");
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }
}

static void Confirm_Button_Clicked(GtkWidget* widget, gpointer data) 
{
//  get the registration information from the registration box

    REGISTRATION_BOX_CONTROLS* rbc = (REGISTRATION_BOX_CONTROLS*)data;
    char* user = (gchar*)gtk_entry_get_text( GTK_ENTRY( rbc->usernameEntry) );
    char* pass = (gchar*)gtk_entry_get_text( GTK_ENTRY( rbc->passwordEntry) );
    char* confirmPassword = (gchar*)gtk_entry_get_text( GTK_ENTRY( rbc->confirmPasswordEntry) );


//  verify password and confrim password match

    if(strcmp(confirmPassword, pass) != 0)
    {
	//update error label with "password do not match"
	gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Passwords do not match.");
	return;
    }

// verify the fields are not empty
    if(strcmp(user, "") == 0)
    {
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Username field is empty.");
	return;
    }
 
    if(strcmp(pass, "") == 0)
    {
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Password field is empty.");
	return;
    }
 
// create the message to send to server 
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "REGISTER/");
    strcat(SendBuf, user);       //user as 2nd entry
    strcat(SendBuf, "/");
    strcat(SendBuf, pass);       //pass as 2nd entry

  
    char RecvBuf[BUFFSIZE];
    const char* Response;

//  send the  message to server and wait for response 
#ifdef DEBUG
    printf("Sending to server: %s\n",SendBuf);
#endif

    Response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to REGISTER", Response);
#endif

//  allow or refuse the registration depending on server response
    if ( strcmp(Response, "OK_REGISTER") == 0 )
    {
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "");
	
        Quick_Info_Message(widget, "Account Created", "Your account has been created successfully! You can now login.");

        // registration was accepted, switch to after log in window
        GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
        if(gtk_widget_is_toplevel (topLevel))
        {
  	    Set_New_Container_Content( GTK_CONTAINER(topLevel) , Create_Login_Box() ); 
        }
    }
    else if ( strcmp(Response, "USERNAME_TAKEN") == 0 )
    {
        // registration was rejected, update the error label
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Username is taken.");
    }
    else if ( strcmp(Response, "USERNAME_INVALID") == 0 )
    {
        // registration was rejected, update the error label
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Username is invalid (5-12 characters, no space).");
    }
    else if ( strcmp(Response, "PASSWORD_INVALID") == 0 )
    {
        // registration was rejected, update the error label
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "Password is invalid (5-12 characters, no space).");   
    }
    else 
    {
	//unexpected response from server. do nothing.
        gtk_label_set_text(GTK_LABEL(rbc->errorLabel), "");	
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }
}

static void Add_Button_Clicked(GtkWidget* widget, gpointer data)
{
// get the name of the friend to add from the data entry		
    const char* friendName = gtk_entry_get_text(GTK_ENTRY(data));

    if( strcmp (friendName, "") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request could not be sent to \"");
        strcat (text, friendName);
        strcat (text,"\". The username does not exist.");

        Quick_Error_Message(widget, "Friend could not be added", text); 
	return;
    }

// create the message to send to server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "ADD/");
    strcat(SendBuf, clientUsername );
    strcat(SendBuf, "/");
    strcat(SendBuf, friendName );

    char RecvBuf[BUFFSIZE];
    const char* Response;

// send the message to server and wait for response
#ifdef DEBUG
    printf("Sending to server: %s\n",SendBuf);
#endif

    Response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to ADD", Response);
#endif

    if ( strcmp(Response, "OK_ADD") == 0 )
    {
        //friend was added successfully
        char text[BUFFSIZE];
        strcpy(text, "A friend request has been sent to \"");
        strcat (text, friendName);
        strcat (text,"\".");

        Quick_Info_Message(widget, "Friend added", text);
    }
    else if ( strcmp(Response,"USERNAME_DOES_NOT_EXIST") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request could not be sent to \"");
        strcat (text, friendName);
        strcat (text,"\". The username does not exist.");

        Quick_Error_Message(widget, "Friend could not be added", text);
    }
    else if ( strcmp(Response,"REQUEST_ALREADY_SENT") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request could not be sent to \"");
        strcat (text, friendName);
        strcat (text,"\". Friend request has already been sent.");

        Quick_Error_Message(widget, "Friend could not be added", text);
    }
    else if ( strcmp(Response,"USER_ALREADY_ADDED") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request could not be sent to \"");
        strcat (text, friendName);
        strcat (text,"\". User is already in contact list.");

        Quick_Error_Message(widget, "Friend could not be added", text);
    }
    else if ( strcmp(Response,"REQUEST_ALREADY_RECEIVED") == 0 )
    {   
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request could not be sent to \"");
        strcat (text, friendName);
        strcat (text,"\". This user has already sent you a friend request.");

        Quick_Error_Message(widget, "Friend could not be added", text);
    }
    else 
    {
	//unexpected response from server. do nothing.
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }
}

static void Remove_Button_Clicked(GtkWidget* widget, gpointer data)
{
// get the name of the friend from the data entry		
    const char* friendName = gtk_entry_get_text(GTK_ENTRY(data));

    if( strcmp (friendName, "") == 0 )
    {
 	//friend could not be removed
        char text[BUFFSIZE];
        strcpy(text, "User \"");
        strcat (text, friendName);
        strcat (text,"\" could not be removed from contact list. User is not in your contact list.");

        Quick_Error_Message(widget, "Contact could not be removed", text);
 	return;
    }

// create the message to send to server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "REMOVE/");
    strcat(SendBuf, clientUsername );
    strcat(SendBuf, "/");
    strcat(SendBuf, friendName );

    char RecvBuf[BUFFSIZE];
    const char* Response;

// send the message to server and wait for response
#ifdef DEBUG
    printf("Sending to server: %s\n",SendBuf);
#endif

    Response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to REMOVE", Response);
#endif

    if ( strcmp(Response, "OK_REMOVE") == 0 )
    {
        //friend was removed successfully
        char text[BUFFSIZE];
        strcpy(text, "\"");
        strcat (text, friendName);
        strcat (text,"\" has been removed from your contact list.");

        Quick_Info_Message(widget, "Friend removed", text);
    }
    else if ( strcmp(Response,"USER_NOT_IN_CONTACTS") == 0 )
    {
	//friend could not be removed
        char text[BUFFSIZE];
        strcpy(text, "User \"");
        strcat (text, friendName);
        strcat (text,"\" could not be removed from contact list. User was not found in your contact list.");

        Quick_Error_Message(widget, "Contact could not be removed", text);
    }
    else 
    {
	//unexpected response from server. do nothing.
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }

}

static void Accept_Friend_Request_Button_Clicked(GtkWidget* widget, gpointer data)
{
    char* friendName = (char*)gtk_label_get_text(GTK_LABEL(data)); 
 
// create the message to send to server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "ACCEPT_REQUEST/");
    strcat(SendBuf, clientUsername );
    strcat(SendBuf, "/");
    strcat(SendBuf, friendName );
 
    char RecvBuf[BUFFSIZE];
    const char* Response;

    // send the message to server and wait for response
#ifdef DEBUG
    printf("Sending to server: %s\n",SendBuf);
#endif

    Response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to ACCEPT", Response);
#endif

    if ( strcmp(Response, "OK_ACCEPT_REQUEST") == 0 )
    {
        //friend was added successfully
        char text[BUFFSIZE];
        strcpy (text, "\"");
        strcat (text, friendName);
        strcat (text,"\" has been added to your contact list.");

        Quick_Info_Message(widget, "Friend added", text);

    }
    else if ( strcmp(Response,"REQUEST_NOT_FOUND") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request from \"");
        strcat (text, friendName);
        strcat (text,"\" could not be found.");

        Quick_Error_Message(widget, "Friend request could not be found", text);
    }
    else if ( strcmp(Response,"ALREADY_IN_CONTACTS") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request from \"");
        strcat (text, friendName);
        strcat (text,"\" could not be accepted. User is already in contact list.");

        Quick_Error_Message(widget, "Friend request could not be accepted", text);
    }   
    else 
    {
	//unexpected response from server. do nothing.
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }

     GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
     if(gtk_widget_is_toplevel (topLevel))
    {
    	gtk_widget_destroy(topLevel); 
    }
}

static void Ignore_Friend_Request_Button_Clicked(GtkWidget* widget, gpointer data)
{
    char* friendName = (char*)gtk_label_get_text(GTK_LABEL(data)); 
 
// create the message to send to server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "IGNORE_REQUEST/");
    strcat(SendBuf, clientUsername );
    strcat(SendBuf, "/");
    strcat(SendBuf, friendName );
 
    char RecvBuf[BUFFSIZE];
    const char* response;

    // send the message to server and wait for response
#ifdef DEBUG
    printf("Sending to server: %s\n",SendBuf);
#endif

    response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to IGNORE", response);
#endif

    if ( strcmp(response, "OK_IGNORE_REQUEST") == 0 )
    {
        //friend was added successfully
        char text[BUFFSIZE];
        strcpy (text, "\"");
        strcat (text, friendName);
        strcat (text,"\" has been removed from friend requests list.");

        Quick_Info_Message(widget, "Friend requet removed", text);

    }
    else if ( strcmp(response,"REQUEST_NOT_FOUND") == 0 )
    {
	//friend could not be added
        char text[BUFFSIZE];
        strcpy(text, "Friend request from \"");
        strcat (text, friendName);
        strcat (text,"\" could not be found.");

        Quick_Error_Message(widget, "Friend request could not be found", text);
    }
    else 
    {
	//unexpected response from server. do nothing.
        Quick_Error_Message(widget, "Error", "An error has occured: Unexpected response from server.");
    }

     
     GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
     if(gtk_widget_is_toplevel (topLevel))
    {
    	gtk_widget_destroy(topLevel); 
    } 

}

static void Friend_Requests_Button_Clicked(GtkWidget* widget, gpointer data)
{
 
// create the message to send to server
    char SendBuf[BUFFSIZE];
    strcpy(SendBuf, "GET_FRIEND_REQUESTS/");
    strcat(SendBuf, clientUsername);

    char RecvBuf[BUFFSIZE];
    const char* response;

// send the message to server and wait for response 
#ifdef DEBUG
    printf("Sending to server: %s\n", SendBuf);
#endif

    response = Talk2Server(SendBuf, RecvBuf);

#ifdef DEBUG
    Quick_Info_Message(widget, "server response to GET_REQUESTS", response);
#endif

// deconstruct the server's response
    
    int numberOfNames = 0;
    char** names = Deconstruct(RecvBuf, '/', &numberOfNames); 
    
    if(numberOfNames <= 1)
    {
	Quick_Info_Message(widget, "Friend Requests", "You have no friend requests.");
	return;
    }
    
// Create the table
    GtkWidget* table = gtk_table_new(numberOfNames, 11, false);

    int currentIndex = 1;
    while (currentIndex < numberOfNames)
    {
// Create the accept and ignore buttons	
	GtkWidget* accept_button = gtk_button_new_with_label("ACCEPT");
	GtkWidget* ignore_button = gtk_button_new_with_label("IGNORE");

// Create the label
	GtkWidget* label = gtk_label_new("");
	gtk_label_set_text(GTK_LABEL(label) , names[currentIndex]);
	
// Create the box
	GtkWidget* hbox = gtk_hbox_new(false, 0);
	
	gtk_box_pack_start( GTK_BOX(hbox), accept_button, false, false, 0);
	gtk_box_pack_start( GTK_BOX(hbox), ignore_button, false, false, 0);
	
// Create the table and attach the components	
	gtk_table_attach( GTK_TABLE(table), label, 0, 10, currentIndex-1, currentIndex, GTK_EXPAND| GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach( GTK_TABLE(table), hbox, 10, 11, currentIndex-1, currentIndex, GTK_EXPAND| GTK_FILL, GTK_SHRINK, 0, 0);
	
// Connect the buttons with callback functions	
	g_signal_connect(accept_button, "clicked", G_CALLBACK(Accept_Friend_Request_Button_Clicked), label);
	g_signal_connect(ignore_button, "clicked", G_CALLBACK(Ignore_Friend_Request_Button_Clicked), label);	
	
	currentIndex ++;	
    }

// Create the scroll window
    GtkWidget* scrollWindow = gtk_scrolled_window_new(NULL, NULL);
	
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW (scrollWindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollWindow), table);

// Create the window
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
    gtk_window_set_title(GTK_WINDOW(window), "Friend Request");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(window), 300, (25 * numberOfNames) < 300 ? 25 * numberOfNames : 300 );
   
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), window);
	
    gtk_container_add(GTK_CONTAINER(window), scrollWindow);

    gtk_widget_show_all(GTK_WIDGET(window));

}

static void Send_Button_Clicked(GtkWidget* widget, gpointer data)
{

    CHAT_BOX_CONTROLS* cbc = (CHAT_BOX_CONTROLS*)data;
    char* text = (char*)gtk_entry_get_text( GTK_ENTRY( cbc->messageEntry)) ;
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(cbc->textView));
    GtkTextIter iter ;
    gtk_text_buffer_get_end_iter (buffer, &iter);
    gtk_text_buffer_insert_with_tags_by_name( GTK_TEXT_BUFFER(buffer) ,&iter, 
	clientUsername , -1, "italic", "bold" , NULL); 
    gtk_text_buffer_insert( GTK_TEXT_BUFFER(buffer) ,&iter, ": " , -1); 
    gtk_text_buffer_insert( GTK_TEXT_BUFFER(buffer) ,&iter, text , -1); 
    gtk_text_buffer_insert( GTK_TEXT_BUFFER(buffer) ,&iter, "\n", -1); 
 
}


static void Shut_Down_Button_Clicked(GtkWidget* widget, gpointer data)
{

    char RecvBuf[BUFFSIZE];
    const char* Response;

    Response = Talk2Server("SHUTDOWN", RecvBuf);

    Quick_Info_Message(widget, "server response to SHUTDOWN", Response);
}

/*** callback functions that do not communate with server ********************/
static void Cancel_Button_Clicked(GtkWidget* widget, gpointer data) 
{
    GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
    
    if(gtk_widget_is_toplevel (topLevel))
    {
	Set_New_Container_Content( GTK_CONTAINER(topLevel) , Create_Login_Box() ); 
    }
}

static void Register_Button_Clicked(GtkWidget* widget, gpointer data) 
{
    GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
    
    if(gtk_widget_is_toplevel (topLevel))
    {
	Set_New_Container_Content( GTK_CONTAINER(topLevel) , Create_Registration_Box() );
    }
}

static void Exit_Button_Clicked(GtkWidget* widget, gpointer data)
{
	int response = Quick_Yes_No_Message(widget, "Confirm Exit", "Are you sure you want to exit?");

	if (response == GTK_RESPONSE_YES)
	{
		gtk_main_quit();
	}	
}

static void Log_Out_Button_Clicked(GtkWidget* widget, gpointer data)
{
        strcpy(clientUsername, "");

	int response = Quick_Yes_No_Message(widget, "Confirm Log Out", "Are you sure you want to log out?");

	if (response == GTK_RESPONSE_YES)
	{
            GtkWidget* topLevel = gtk_widget_get_toplevel(widget);
    
            if(gtk_widget_is_toplevel (topLevel))
            {
	        Set_New_Container_Content( GTK_CONTAINER(topLevel) , Create_Login_Box() ); 
            }
	}	
}

static void Add_Remove_Button_Clicked(GtkWidget* widget, gpointer data)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window), "Add/Remove a Contact");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	
	GtkWidget *add_button = gtk_button_new_with_label("   ADD   ");
	GtkWidget *remove_button = gtk_button_new_with_label("REMOVE");
	GtkWidget *entry = gtk_entry_new();
	GtkWidget *entry_label = gtk_label_new("Type your friend's name");

	GtkWidget* hbox = gtk_hbox_new(false, 0);
	GtkWidget* vbox = gtk_vbox_new(false, 0);
	
	gtk_box_pack_start( GTK_BOX(hbox), entry, false, false, 10);
	gtk_box_pack_start( GTK_BOX(hbox), add_button, false, false, 0);
	gtk_box_pack_start( GTK_BOX(hbox), remove_button, false, false, 0);
	
	gtk_box_pack_start( GTK_BOX(vbox), entry_label, false, false, 5);
	gtk_box_pack_start( GTK_BOX(vbox), hbox, false, false, 0);
	
	gtk_container_add(GTK_CONTAINER(window), vbox);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), window);
	g_signal_connect(add_button, "clicked", G_CALLBACK(Add_Button_Clicked), entry);
	g_signal_connect(remove_button, "clicked", G_CALLBACK(Remove_Button_Clicked), entry);
	
	gtk_widget_show_all(GTK_WIDGET(window));
	
}

/************************** Gui creation methods *****************************/

GtkWidget* Create_Login_Box()
{
 
    static LOGIN_BOX_CONTROLS lbc;

// Create the logo image
    GtkWidget* logo = gtk_image_new_from_file("../res/QuickChatLogo.PNG");

// Create the labels 
    GtkWidget* usernameLabel = gtk_label_new("USERNAME");
    GtkWidget* passwordLabel = gtk_label_new("PASSWORD");
    GtkWidget* registerLabel = gtk_label_new("Don't have an account?");

// Create and set the color of the error label
    lbc.errorLabel = gtk_label_new("");

    GdkColor color; 
    gdk_color_parse("red", &color);
    gtk_widget_modify_fg(lbc.errorLabel, GTK_STATE_NORMAL, &color);

// Create the entry fields
    lbc.usernameEntry = gtk_entry_new();
    lbc.passwordEntry = gtk_entry_new();

    gtk_entry_set_visibility( GTK_ENTRY(lbc.passwordEntry), false );

// Create the buttons
    GtkWidget* logInButton = gtk_button_new_with_label("Log In");
    g_signal_connect(logInButton, "clicked", G_CALLBACK(Log_In_Button_Clicked), &lbc);

    GtkWidget* exitButton = gtk_button_new_with_label("Exit");
    g_signal_connect(exitButton, "clicked", G_CALLBACK(Exit_Button_Clicked), NULL);

    GtkWidget* registerButton = gtk_button_new_with_label("Register...");
    g_signal_connect(registerButton, "clicked", G_CALLBACK(Register_Button_Clicked), NULL);

// Create alignment widget for each label and entry
    GtkWidget* leftAlign1 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* leftAlign2 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* leftAlign3 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* fillAlign1 =  gtk_alignment_new(0, 0.5, 1, 0);
    GtkWidget* fillAlign2 =  gtk_alignment_new(0, 0.5, 1, 0);

    gtk_container_add(GTK_CONTAINER(leftAlign1), usernameLabel);    
    gtk_container_add(GTK_CONTAINER(leftAlign2), passwordLabel);    
    gtk_container_add(GTK_CONTAINER(leftAlign3), lbc.errorLabel); 
    gtk_container_add(GTK_CONTAINER(fillAlign1), lbc.usernameEntry);    
    gtk_container_add(GTK_CONTAINER(fillAlign2), lbc.passwordEntry);    

// Create the register box
    GtkWidget* registerBox = gtk_hbox_new(false,60);

    gtk_box_pack_start( GTK_BOX(registerBox), registerLabel, true, true, 0); //expand, fill, spacing
    gtk_box_pack_start( GTK_BOX(registerBox), registerButton, true, true, 0);

// Create the horizontal separator
   GtkWidget* hSeparator = gtk_hseparator_new ();

// Create the button box
    GtkWidget* buttonBox = gtk_hbox_new(false,0); //homogenous, spacing 

    gtk_box_pack_start( GTK_BOX(buttonBox), logInButton, true, true, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox), exitButton, true, true, 0);

    gtk_widget_set_size_request(buttonBox, BUTTON_BOX_WIDTH, BUTTON_BOX_HEIGHT);

// Create the vbox    
    GtkWidget* verticalBox = gtk_vbox_new( false, 0);

    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign1, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), fillAlign1, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), gtk_label_new(""), true,true,0); //padding
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign2, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), fillAlign2, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign3, true, true, 0);  
    gtk_box_pack_start( GTK_BOX(verticalBox), registerBox, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), hSeparator, true, true, 0);
    gtk_box_pack_end( GTK_BOX(verticalBox), buttonBox, true, true, 0); //adds from the bottom up
    
// Create table to contain the logo and the vbox
    GtkWidget* middleTable = gtk_table_new(1, 3, false); //number of rows, number of columns, homogenous     

    gtk_table_attach ( GTK_TABLE(middleTable), logo,0,1,0,1,GTK_EXPAND, GTK_SHRINK,0,0);
    gtk_table_attach ( GTK_TABLE(middleTable), verticalBox,1,2,0,1, GTK_EXPAND, GTK_EXPAND, 0,0); 
    gtk_table_attach ( GTK_TABLE(middleTable), gtk_label_new(""), 2,3,0,1,GTK_SHRINK, GTK_EXPAND,0,0);
    
// Create alignment for the table
    GtkWidget* centerAlign = gtk_alignment_new( 0.5,0.5,0,0);
    gtk_container_add( GTK_CONTAINER(centerAlign), middleTable );

// Draw the background
    GtkWidget* eventBox = gtk_event_box_new();
    gtk_container_add( GTK_CONTAINER(eventBox), centerAlign);   
    Add_Background(eventBox,"../res/WorldMap.PNG");

    return eventBox;
}

GtkWidget* Create_Registration_Box()
{
    static REGISTRATION_BOX_CONTROLS rbc;

// Create the logo image
    GtkWidget* logo = gtk_image_new_from_file("../res/QuickChatLogo.PNG");

// Create the labels
    GtkWidget* usernameLabel = gtk_label_new("USERNAME");
    GtkWidget* passwordLabel = gtk_label_new("PASSWORD");
    GtkWidget* confirmPasswordLabel = gtk_label_new("CONFIRM PASSWORD");

// Create and set the color of the error label
    rbc.errorLabel = gtk_label_new("");

    GdkColor color; 
    gdk_color_parse("red", &color);
    gtk_widget_modify_fg(rbc.errorLabel, GTK_STATE_NORMAL, &color);

// Create the Entry field
    rbc.usernameEntry = gtk_entry_new();
    rbc.passwordEntry = gtk_entry_new();
    rbc.confirmPasswordEntry = gtk_entry_new();

    gtk_entry_set_visibility( GTK_ENTRY(rbc.passwordEntry), false );
    gtk_entry_set_visibility( GTK_ENTRY(rbc.confirmPasswordEntry), false );

// Create the buttons
    GtkWidget* confirmButton = gtk_button_new_with_label("Confirm");
    g_signal_connect(confirmButton, "clicked", G_CALLBACK(Confirm_Button_Clicked), &rbc);

    GtkWidget* cancelButton = gtk_button_new_with_label("Cancel");
    g_signal_connect(cancelButton, "clicked", G_CALLBACK(Cancel_Button_Clicked), NULL);

// Create alignment widget for each label and entry
    GtkWidget* leftAlign1 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* leftAlign2 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* leftAlign3 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* leftAlign4 =  gtk_alignment_new(0, 0.5, 0, 0);
    GtkWidget* fillAlign1 =  gtk_alignment_new(0, 0.5, 1, 0);
    GtkWidget* fillAlign2 =  gtk_alignment_new(0, 0.5, 1, 0);
    GtkWidget* fillAlign3 =  gtk_alignment_new(0, 0.5, 1, 0);

    gtk_container_add(GTK_CONTAINER(leftAlign1), usernameLabel);    
    gtk_container_add(GTK_CONTAINER(leftAlign2), passwordLabel);    
    gtk_container_add(GTK_CONTAINER(leftAlign3), confirmPasswordLabel);    
    gtk_container_add(GTK_CONTAINER(leftAlign4), rbc.errorLabel); 
    gtk_container_add(GTK_CONTAINER(fillAlign1), rbc.usernameEntry);    
    gtk_container_add(GTK_CONTAINER(fillAlign2), rbc.passwordEntry); ;    
    gtk_container_add(GTK_CONTAINER(fillAlign3), rbc.confirmPasswordEntry);   

// Create the horizontal separator
    GtkWidget* hSeparator = gtk_hseparator_new ();

// Create the button box
    GtkWidget* buttonBox = gtk_hbox_new(true,0); //homogenous, spacing

    gtk_box_pack_start( GTK_BOX(buttonBox), confirmButton, true, true, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox), cancelButton, true, true, 0);
 
    gtk_widget_set_size_request(buttonBox, BUTTON_BOX_WIDTH, BUTTON_BOX_HEIGHT);
  
// Create the vbox    
    GtkWidget* verticalBox = gtk_vbox_new( false, 0);
    
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign1, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), fillAlign1, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign2, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), fillAlign2, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign3, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), fillAlign3, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), leftAlign4, true, true, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), hSeparator, true, true, 0);
    gtk_box_pack_end( GTK_BOX(verticalBox), buttonBox, false, false, 0); //adds from the bottom up
 
// Create table to contain the logo and the vbox    
    GtkWidget* middleTable = gtk_table_new(1, 3, false); //number of rows, number of columns, homogenous  
   
    gtk_table_attach ( GTK_TABLE(middleTable), logo,0,1,0,1,GTK_EXPAND, GTK_SHRINK,0,0);
    gtk_table_attach ( GTK_TABLE(middleTable), verticalBox,1,2,0,1, GTK_EXPAND, GTK_EXPAND, 0,0); 
    gtk_table_attach ( GTK_TABLE(middleTable), gtk_label_new(""), 2,3,0,1,GTK_SHRINK, GTK_EXPAND,0,0);
    
// Create alignment for the table
    GtkWidget* centerAlign = gtk_alignment_new( 0.5,0.5,0,0);
    gtk_container_add( GTK_CONTAINER(centerAlign), middleTable );

// Draw the background
    GtkWidget* eventBox = gtk_event_box_new();
    gtk_container_add( GTK_CONTAINER(eventBox), centerAlign);   
    Add_Background(eventBox,"../res/WorldMap.PNG");

    return eventBox;
}

GtkWidget* Create_Main_Box()
{
// Create the labels
    GtkWidget *contactNameLabel = gtk_label_new("NAME");
    GtkWidget *statusLabel      = gtk_label_new("STATUS");

// Create the buttons
    GtkWidget *addRemoveButton = gtk_button_new_with_label("Add/Remove a Contact");
    g_signal_connect(addRemoveButton, "clicked", G_CALLBACK(Add_Remove_Button_Clicked), NULL);

    GtkWidget *friendRequestsButton = gtk_button_new_with_label("Friend Requests");
    g_signal_connect(friendRequestsButton, "clicked", G_CALLBACK(Friend_Requests_Button_Clicked), NULL);
	
    GtkWidget *logOutButton = gtk_button_new_with_label("Log out");
    g_signal_connect(logOutButton, "clicked", G_CALLBACK(Log_Out_Button_Clicked), NULL);

// Create the button box and add the components
    GtkWidget *buttonBox = gtk_hbox_new(false, 48);

    gtk_box_pack_start( GTK_BOX(buttonBox), addRemoveButton, false, false, 30);
    gtk_box_pack_start( GTK_BOX(buttonBox), friendRequestsButton, false, false, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox), logOutButton, false, false, 30);
	
// Create the separator
    GtkWidget* vSeparator = gtk_vseparator_new();
    GtkWidget* hSeparator = gtk_hseparator_new();

// Create the table
    GtkWidget* contactsTable = gtk_table_new(2, 13, false); // rows, columns, homogenous

    gtk_table_attach( GTK_TABLE(contactsTable), contactNameLabel, 0, 10, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach( GTK_TABLE(contactsTable), gtk_label_new(" "), 10, 11, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);         // space for numbers of messages
    gtk_table_attach( GTK_TABLE(contactsTable), vSeparator, 11, 12, 0, 1, GTK_EXPAND| GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach( GTK_TABLE(contactsTable), statusLabel, 12, 13, 0, 1, GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_table_attach( GTK_TABLE(contactsTable), hSeparator, 0, 13, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

// Create the scrolled window1
    GtkWidget* scrolledWindow1 = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW (scrolledWindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow1), contactsTable);

// Create the Pane
    GtkWidget* hPane = gtk_hpaned_new(); 

    gtk_paned_add1(GTK_PANED(hPane), scrolledWindow1);
    gtk_paned_add2(GTK_PANED(hPane), Create_Chat_Box());

    gtk_paned_set_position(GTK_PANED(hPane), PANE_SIZE);

//Create the vertical box and add the components
    GtkWidget *verticalBox = gtk_vbox_new(false, 0);

    gtk_box_pack_start( GTK_BOX(verticalBox), buttonBox, false, false, 0);
    gtk_box_pack_start( GTK_BOX(verticalBox), hPane, true, true, 0);   

    return verticalBox;
}

GtkWidget* Create_Chat_Box()
{

    static CHAT_BOX_CONTROLS cbc;

// Create the entry
    cbc.messageEntry = gtk_entry_new();

// Create the button
    GtkWidget *sendButton = gtk_button_new_with_label("Send");
    g_signal_connect(sendButton, "clicked", G_CALLBACK(Send_Button_Clicked), &cbc);

// Create the chat box and add the components
    GtkWidget *entryBox = gtk_hbox_new(false, 20);

    gtk_box_pack_start( GTK_BOX(entryBox), cbc.messageEntry, true, true, 10);
    gtk_box_pack_start( GTK_BOX(entryBox), sendButton, false, false, 10);

// Create the label
    GtkWidget *chatLabel = gtk_label_new("CHAT");

// Create the text view
    cbc.textView = gtk_text_view_new_with_buffer(gtk_text_buffer_new(NULL));

    GtkTextBuffer* buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(cbc.textView));
    gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);

// edit the text
    gtk_text_view_set_editable(GTK_TEXT_VIEW(cbc.textView), false);

// Create the separator
    GtkWidget *hSeparator1 = gtk_hseparator_new();
    GtkWidget *hSeparator2 = gtk_hseparator_new();
// Create the table for the chat area
    GtkWidget* chatTable = gtk_table_new(1, 3, false);

    gtk_table_attach( GTK_TABLE(chatTable), chatLabel, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach( GTK_TABLE(chatTable), hSeparator1, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach( GTK_TABLE(chatTable), cbc.textView, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

// Create the scrolled window2
    GtkWidget* scrolledWindow2 = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW (scrolledWindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow2), chatTable);

// Create the box for entry and button
    GtkWidget* verticalBox = gtk_vbox_new(false, 0);
    
    gtk_box_pack_start  ( GTK_BOX(verticalBox), scrolledWindow2, true, true, 0);
    gtk_box_pack_end  ( GTK_BOX(verticalBox), gtk_label_new(""), false, false, 0);
    gtk_box_pack_end  ( GTK_BOX(verticalBox), entryBox, false, false, 0);  
    gtk_box_pack_end  ( GTK_BOX(verticalBox), gtk_label_new(""), false, false, 0);
    gtk_box_pack_end  ( GTK_BOX(verticalBox), hSeparator2, false, false, 0);

    return verticalBox;
}

GtkWidget* Create_Testing_Box()
{
    GtkWidget* verticalBox;

    GtkWidget* addRemoveButton;
    GtkWidget* friendRequestsButton;
    GtkWidget* logOutButton, *shutDownButton;

    GtkWidget* hSeparator;
    GtkWidget* buttonBox1, *buttonBox2;

    addRemoveButton = gtk_button_new_with_label("Add/remove a Contact");
    g_signal_connect(addRemoveButton, "clicked", G_CALLBACK(Add_Remove_Button_Clicked), NULL);    

    friendRequestsButton = gtk_button_new_with_label("Friend Requests");
    g_signal_connect(friendRequestsButton, "clicked", G_CALLBACK(Friend_Requests_Button_Clicked), NULL);    

    shutDownButton = gtk_button_new_with_label("Shut Down");
    g_signal_connect(shutDownButton, "clicked", G_CALLBACK(Shut_Down_Button_Clicked), NULL);    

    logOutButton = gtk_button_new_with_label("Log out");
    g_signal_connect(logOutButton, "clicked", G_CALLBACK(Log_Out_Button_Clicked), NULL);

// create the horizontal separator
    hSeparator = gtk_hseparator_new();

// create the button box
    buttonBox1 = gtk_vbox_new( false, 0);      // homogenous, spacing
    
    buttonBox2 = gtk_vbox_new( false, 0);

// add buttons to the button box
    gtk_box_pack_start( GTK_BOX(buttonBox1), addRemoveButton, true, true, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox1), friendRequestsButton, true, true, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox2), logOutButton, true, true, 0);
    gtk_box_pack_start( GTK_BOX(buttonBox2), shutDownButton, true, true, 0);

// create the vertical box
    verticalBox = gtk_hbox_new( false, 10);   // homogenous, spacing

// add the components and separatros to the verticalBox
    gtk_box_pack_start( GTK_BOX(verticalBox), buttonBox1, true, true, 10); // adds from the top down

    gtk_box_pack_start( GTK_BOX(verticalBox), buttonBox2, true, true, 10); 

    gtk_box_pack_start( GTK_BOX(verticalBox), hSeparator, false, false, 0);

    return verticalBox;
}

/************************** Gui helper methods *******************************/

/* removes all widgets from a container and places the passed widget instead*/
void Set_New_Container_Content ( GtkContainer* container, GtkWidget* widget )
{

    Remove_All_Children( container );
    gtk_container_add( container, widget );

    gtk_widget_show_all(GTK_WIDGET(container));

}

/* removes all widgets from a container */
void Remove_All_Children (GtkContainer* container)
{
    GList* children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(container));
    
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
	gtk_widget_destroy(GTK_WIDGET(iter->data));
    }

    g_list_free(children);
}


void Quick_Info_Message (GtkWidget* widget, const char* title, const char* message)
{
    GtkWidget* dialog;

    // Create the dialog
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    GtkWindow* window = (widget == NULL)? NULL : GTK_WINDOW( gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW) );
    
    dialog = gtk_message_dialog_new ( window,
			 	      flags,
				      GTK_MESSAGE_INFO,
				      GTK_BUTTONS_CLOSE,
				      message,
				      title,
				      NULL);
    /*result discarded*/
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void Quick_Error_Message (GtkWidget* widget, const char* title, const char* message)
{
    GtkWidget* dialog;

    // Create the dialog
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    GtkWindow* window = (widget == NULL)? NULL : GTK_WINDOW( gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW) );
    
    dialog = gtk_message_dialog_new ( window,
			 	      flags,
				      GTK_MESSAGE_ERROR,
				      GTK_BUTTONS_CLOSE,
				      message,
				      title,
				      NULL);
    /*result discarded*/
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

int Quick_Yes_No_Message (GtkWidget* widget, const char* title, const char* message)
{
    GtkWidget* dialog;

    // Create the dialog
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    GtkWindow* window = (widget == NULL)? NULL : GTK_WINDOW( gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW) );
    
    dialog = gtk_message_dialog_new ( window,
			 	      flags,
				      GTK_MESSAGE_QUESTION,
				      GTK_BUTTONS_YES_NO,
				      message,
				      title,
				      NULL);
 
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return response;

}


//will work for gtk windows and gtk event boxes
void Add_Background(GtkWidget* widget, char* path)
{ 

    GdkPixbuf* image = NULL;
    GdkPixmap* background = NULL;
    GtkStyle* style = NULL;
    GError* error = NULL;

    image = gdk_pixbuf_new_from_file(path, &error);
    if (image == NULL) 
    {
	char message[80];
	strcpy(message, "Error: \"");
	strcat(message, path);
	strcat(message, "\" not found.");
	Quick_Error_Message (NULL, "Missing resources", message);
	return;
    }

    image = gdk_pixbuf_scale_simple (image, WINDOW_WIDTH, WINDOW_HEIGHT, GDK_INTERP_BILINEAR);

    gdk_pixbuf_render_pixmap_and_mask(image, &background, NULL, 0);
    
    style = gtk_style_new();
    style->bg_pixmap[0] = background; 
   
    gtk_widget_set_style(GTK_WIDGET(widget), GTK_STYLE(style)); 
    g_object_unref(style);    

}
