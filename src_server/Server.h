#ifndef SERVER_H
#define SERVER_H

void FatalError(const char *ErrorMsg);

int MakeServerSocket( uint16_t PortNo );

void TimeOutFunction(void);

/* process a request by a client */
void ProcessRequest( int DataSocketFD );

/* simple server main loop */
void ServerMainLoop();

#endif
