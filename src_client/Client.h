#include <stdbool.h>
#ifndef CLIENT_H
#define CLIENT_H

void FatalError( const char *ErrorMsg );		/* print error diagnostics and abort */
char* Talk2Server( const char *Message, char *RecvBuf);  /*send a message to the server */ 
bool PasswordsMatch(char *p1, char *p2);               /* check if two passwords match for registration */

#endif
