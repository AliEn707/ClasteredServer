
#include "server.h"
#include "client.h"
#include "../server.h"
#include "../client.h"
#include "../messageprocessor.h"
#include "../../share/network/packet.h"
#include "../../share/system/log.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ 	servr messages processors 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

//functions to proceed messages from slave servers, named messageN where N is message type, messages without processors will be sent to client
static void *message1(server *s, packet *p){
	return 0;
}

#define serverMessageProcessor(id)\
	messageprocessorServerAdd(id, &message ## id)

void serverMessageProcessorInit(){
	serverMessageProcessor(1);
}

