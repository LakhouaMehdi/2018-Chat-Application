#ifndef CLIENTLIST_H
#define CLIENTLISt_H


/* Yuki Hayashi
 * 2018/03/01
 *
 * */

//#include "  "

typedef struct ClientEntry CENTRY;
typedef struct ClientList CLIST;
typedef struct ClientData CDATA;

struct ClientEntry
{
	CLIST* List;
	CENTRY* Next;
	CENTRY* Prev;
	CDATA* Clientdata;
};

struct ClientList
{
	CENTRY* First;
	CENTRY* Last;
	int Length;
};

struct ClientData
{
	char* username;
	char* password;
};

CENTRY* CreateClientEntry(CDATA* clientData);

void DeleteClientEntry(CENTRY* entry);

CLIST* CreateClientList();

void DeleteClient(CDATA* clientdata);

void DeleteClientList(CLIST* list);

void AppendClient(CLIST* list, CDATA* clientdata);

#endif

