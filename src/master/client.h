#ifndef CLIENT_HEADER
#define CLIENT_HEADER

#include <sys/time.h>

#include "../share/containers/worklist.h"
#include "../share/system/t_sem.h"
#include "../share/network/socket.h"
#include "../share/network/packet.h"

typedef 
struct {
	int id;
	short broken;
	short server_id;
	socket_t *sock;
	t_sem_t sem;
	worklist messages;
	time_t timestamp;
} client;

typedef 
struct {
	packet packet;
	t_sem_t sem;
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
void clientPacketProceed(client *c, packet* p);

//work with client message queue
void clientMessageAdd(int id, client_message *m);
client_message* clientMessageNew(void* buf, short size);
void clientMessageClear(client_message* m);

//processor for client message queue
void clientMessagesProceed(client *c, void* (*me)(void* d, void * _c));

#endif