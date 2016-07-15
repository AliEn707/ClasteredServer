#ifndef CHAT_HEADER
#define CHAT_HEADER

#include "client.h"
#include "../share/containers/bintree.h"
#include "../share/containers/worklist.h"
#include "../share/system/t_sem.h"

typedef 
struct chat{
	int id;
	bintree clients;
	t_sem_t sem;
	worklist history;
} chat;


void chatsInit();
void chatsClear();

chat* chatNew();
void chatClear(chat* c);

void chatClientsAdd(chat* c, void* client);
void* chatClientsGet(chat* c, int id);
void chatClientsRemove(chat* c, void* client);

int chatsAdd(chat* c);
chat* chatsGet(int id);
void chatsRemove(chat* c);
void chatsCheck();

#endif
