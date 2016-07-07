
#include "client.h"
#include "server.h"
#include "../client.h"
#include "../server.h"
#include "../messageprocessor.h"
#include "../../share/network/packet.h"
#include "../../share/system/log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	clients messages processors 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/



#define clientMessageProcessor(id)\
	messageprocessorClientAdd(id, &message ## id)

#define clientCheckAuth(c)\
	if(c->id==0){\
		c->broken=1;\
		printf("unauthorized client");\
		return c;\
	}\

//AUTH
static void *message1(client*c, packet* p){
	c->id=rand();//for tests
	c->server_id=serversGetIdAuto();//for tests
	//auth client
	clientsAdd(c);
	return 0;
}

static void *message2(client*c, packet* p){
	clientCheckAuth(c);//client must have id already
	//some work
	return 0;
}

void clientMessageProcessorInit(){
	clientMessageProcessor(1);
	clientMessageProcessor(2);
}