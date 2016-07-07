#include <string.h>

#include "server.h"
#include "client.h"
#include "messageprocessor.h"
#include "../share/containers/bintree.h"
#include "../share/containers/worklist.h"
#include "../share/network/packet.h"
#include "../share/system/log.h"
#include "../share/crc32.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	implementation of slave servers 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

typedef void*(*server_processor)(server*, packet*);

static bintree servers={0};
static t_sem_t sem=0;

void serversInit(){
	memset(&servers, 0,sizeof(servers));
	if ((sem=t_semGet(1))==0){
		perror("t_semGet");
		return;
	}
	t_semSet(sem,0,1);
}

void serversClear(){
	t_semSet(sem,0,-1);
		bintreeErase(&servers, (void(*)(void*))serverClear);
	t_semSet(sem,0,1);
	t_semRemove(sem);
}

server *serverNew(char* host, short port){
	server * s;
	if ((s=malloc(sizeof(*s)))==0){
		perror("malloc");
		return 0;
	}
	memset(s,0,sizeof(*s));
	sprintf(s->host, "%s", host);
	s->port=port;
	if ((s->sock=socketConnect(s->host, s->port))==0){
		serverClear(s);
	}
	//TODO: add auth 
	s->id=serverIdByAddress(host,port);
	serversAdd(s);
	return s;
}

server* serverReconnect(server *s){
	socketClear(s->sock);
	if ((s->sock=socketConnect(s->host, s->port))==0){
		return 0;
	}
	s->broken=0;
	return s;
}

void serverClear(server* s){
	socketClear(s->sock);
	free(s);
}

int serversAdd(server* s){
	if (s->id!=0){
		t_semSet(sem,0,-1);
			bintreeAdd(&servers, s->id, s);
		t_semSet(sem,0,1);
	}
	return s->id;
}

server *serversGet(int id){
	server *s;
	t_semSet(sem,0,-1);
		s=bintreeGet(&servers, id);
if (s!=0 && s->broken)
s=0;
	t_semSet(sem,0,1);
	return s;
}

static void* checkS(bintree_key k, void *v, void *arg){
	server *s=v;
	if (s->broken){
		if (s->sock){
			socketClear(s->sock);
			s->sock=0;
		}
		s->broken=(serverReconnect(s)==0);//if success => !=0 ==> broken ==0
	}
	return 0;
}

void serversCheck(){
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, checkS, 0);
	t_semSet(sem,0,1);
}

static void* findAuto(bintree_key k, void *v, void *arg){
	int *id=arg;
	server *s=v;
	if (!s->broken){
		*id=s->id;
		return &s->id;
	}
	return 0;
}

int serversGetIdAuto(){//TODO: change to find free
	int id=0;
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, findAuto, &id);
	t_semSet(sem,0,1);
	return id;
}

void serversRemove(server* s){
	t_semSet(sem,0,-1);
		bintreeDel(&servers, s->id, (void(*)(void*))serverClear);
	t_semSet(sem,0,1);
}

void serverPacketProceed(server *s, packet *p){
	void* buf=packetGetData(p);
	server_processor processor;
	if ((processor=messageprocessorServer(*((char*)buf)))==0){
		//remove client data from the end
		short size=packetGetSize(p);
		int client_id=*((int*)(buf+(size-=sizeof(client_id))));//check for write size size
		client* c=0;
		if (client_id==0 || (c=clientsGet(client_id))!=0){
			clientMessageAdd(c, clientMessageNew(buf, size));
		}
	}else{//proceed by self
		processor(s, p);
	}
}

int serverIdByAddress(char* address, short port){ //return 6 bytes integer
	char str[400];
	sprintf(str, "%s%d", address, port);
	return crc32(str, strlen(str));
}
