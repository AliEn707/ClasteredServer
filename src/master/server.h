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
	short broken;
	socket_t *sock;
	char host[100];
	short port;
	
	bintree clients;
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

//get id of less busy server
int serversGetIdAuto();

//processor for packet from server
void serverPacketProceed(server* s, packet* p);
void serversPacketSendAll(server *s, packet* p);

//create uniq id by server address and port
int serverIdByAddress(char* address, short port); 

#endif
