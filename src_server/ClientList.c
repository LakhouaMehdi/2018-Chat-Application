/*
 * Yuki Hayashi
 * 2018/03/01
 *
*/

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ClientList.h"



int main ()
{
	/*
	CreateClientList();
	*/
	return 0;

	
}


CENTRY* CreateClientEntry(CDATA* clientdata)
{
	CENTRY *NewClientData;
	NewClientData = malloc(sizeof(CENTRY));
	NewClientData -> List = NULL;
	NewClientData -> Next = NULL;
	NewClientData -> Prev = NULL;
	NewClientData -> Clientdata = clientdata;
	
	return NewClientData;
}

	
void DeleteClientEntry(CENTRY* entry)
{
	if (entry == NULL)
	{
		return;
	}
	
	entry -> List = NULL;
	entry -> Next = NULL;
	entry -> Prev = NULL;
	
	if (entry -> Clientdata != NULL)
	{
		DeleteClient(entry->Clientdata);
	}
	
	free(entry);
	entry = NULL;
}

CLIST* CreateClientList()
{
	CLIST *ClientList;
	ClientList = malloc(sizeof(CLIST));
	assert(ClientList);
	ClientList -> Length = 0;
	ClientList -> First = NULL;
	ClientList -> Last = NULL;
	
	return ClientList;
}

void DeleteClient(CDATA* clientdata)
{
	if(clientdata != NULL)
	{
		free(clientdata);
		clientdata = NULL;
	}
}

void DeleteClientList(CLIST* list)
{
	if (list == NULL)
	{
		return;
	}
	
	CENTRY *curr;
	CENTRY *next;
	
	curr = list -> First;
	
	list -> First = NULL;
	list -> Last = NULL;
	list -> Length = 0;
	
	while(curr!=NULL)
	{
		next = curr -> Next;
		DeleteClientEntry(curr);
		curr = next;
	}
	
	free(list);
	list = NULL;
}

void AppendClient(CLIST* list, CDATA* clientdata)
{
	assert(list);
	CENTRY* curr = CreateClientEntry(clientdata);
	
	assert(curr);
	
	if(list -> Last != NULL)
	{
		curr -> List = list;
		curr -> Next = NULL;
		curr -> Prev = list -> Last;
		list->Last->Next = curr;
		list->Last = curr;
	}
	else
	{
		curr->List = list;
		curr->Next = NULL;
		curr->Prev = NULL;
		list->First = curr;
		list->Last = curr;
	}
	list -> Length++;
}

