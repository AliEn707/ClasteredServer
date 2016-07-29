#ifndef CLIENT_HEADER
#define CLIENT_HEADER

#include <sys/time.h>

#include "storage.h"
#include "chat.h"
#include "../share/containers/worklist.h"
#include "../share/containers/bintree.h"
#include "../share/system/t_mutex.h"
#include "../share/network/socket.h"
#include "../share/network/packet.h"

#define CLIENT_CONN_SOCKET 1
#define MSG_CLIENT 0

typedef 
struct {
	int id;
	char name[40];
	char login[40];
	char passwd[40];
	
	short conn_type;
	short broken;
	short server_id;
	socket_t *sock;
	t_mutex_t mutex;
	worklist messages;
	bintree chats;
	time_t timestamp;
	char token[30];
} client;

typedef 
struct client{
//	packet packet;
	char * data;
	short $data;
	t_mutex_t mutex;
	int num;
	short ready;
} client_message;


void clientsInit();
void clientsClear();

client* clientNew(socket_t *sock);
void clientClear(client* c);

//work with clients container
int clientsAdd(client* c);
client* clientsGet(int id);
void clientsRemove(client* c);
void clientsCheck();

//processor for packet from client
int clientPacketProceed(client *c, packet* p);

//work with client message queue
void clientMessagesAdd(client *c, client_message *m);
client_message* clientMessageNew(void* buf, short size);
void clientMessageClear(client_message* m);

//chats
void clientChatsAdd(client* cl, void* chat);
void* clientChatsGet(client* cl, int id);
void clientChatsRemove(client* cl, void* chat);

//processor for client message queue
void clientMessagesProceed(client *c, void* (*me)(void* d, void * _c), void * arg);

int clientSetInfo(client *c, user_info *u);

void clientServerClear(client* c);

#define clientCritical(_$_c, action)\
	if(_$_c)\
		t_mutexCritical(_$_c->mutex, action)

#define clientCriticalAuto(_$_c, action) ({\
		typeof(action) _$_o=0;\
		if (_$_c){\
			t_mutexLock(_$_c->mutex);\
				_$_o=(action);\
			t_mutexUnlock(_$_c->mutex);\
		}\
		_$_o;\
	})

#endif
