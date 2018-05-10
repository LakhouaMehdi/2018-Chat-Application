/* Lakhoua Mehdi
 * 2/24/2018
 *
 * ClientGui.h: 
 * Header file for client Gui 
 */

#ifndef CLIENT_GUI_H
#define CLIENT_GUI_H

#include <gtk/gtk.h>


/******************************** Structs ************************************/

typedef struct ChatBoxControls {
    GtkWidget* messageEntry;
    GtkWidget* textView;
} CHAT_BOX_CONTROLS;

typedef struct LoginBoxControls
{
    GtkWidget* usernameEntry;
    GtkWidget* passwordEntry;
    GtkWidget* errorLabel;
} LOGIN_BOX_CONTROLS;

typedef struct RegistrationBoxControls
{
    GtkWidget* usernameEntry;
    GtkWidget* passwordEntry;
    GtkWidget* confirmPasswordEntry;
    GtkWidget* errorLabel;
} REGISTRATION_BOX_CONTROLS;

/************************** Gui creation methods *****************************/

GtkWidget* Create_Registration_Box();
GtkWidget* Create_Login_Box();
GtkWidget* Create_Testing_Box();
GtkWidget* Create_Main_Box();
GtkWidget* Create_Chat_Box();

/************************** Gui helper methods *******************************/

void Remove_All_Children (GtkContainer* container);
void Set_New_Container_Content ( GtkContainer* container, GtkWidget* widget );
void Quick_Info_Message (GtkWidget *parent, const char* title, const char* message);
void Quick_Error_Message (GtkWidget* widget, const char* title, const char* message);

int Quick_Yes_No_Message (GtkWidget* widget, const char* title, const char* message);

void Add_Background(GtkWidget* widget, char* path);
#endif



