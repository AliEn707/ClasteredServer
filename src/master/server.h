#ifndef SERVER_HEADER
#define SERVER_HEADER


#include "../share/containers/bintree.h"
#include "../share/containers/worklist.h"
#include "../share/network/socket.h"
#include "../share/network/packet.h"

#define MSG_SERVER 1

typedef 
struct {
	int id;
	short checked;
	socket_t *sock;
	char host[100];
	short port;
	t_mutex_t mutex;
	bintree clients;
	int $clients;
} server;

//initialization
void serversInit();
void serversClear();

//connect to specified server
server *serverNew(char* host, short port);
void serverClear(server* s);

//work with servers container
int serversAdd(server* s);
server *serversGet(int id);
void serversRemove(server* s);
void serversCheck();
void serversForEach(void*(*f)(bintree_key k, void *v, void *arg), void *a);

short serversTotal();
void serversTotalInc();
void serversTotalDec();

//get id of less busy server
int serversGetIdAuto();

//processor for packet from server
void serverPacketProceed(server* s, packet* p);
void serversPacketSendAll(packet* p);

//create uniq id by server address and port
int serverIdByAddress(char* address, short port); 

//
int serverClientsAdd(server *s, void* c);
void* serverClientsGet(server *s, int id);
int serverClientsRemove(server *s, void* c);
void serverClientsErase(server *s);

#endif
