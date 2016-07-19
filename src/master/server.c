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
		perror("socketConnect");
		serverClear(s);
		return 0;
	}
	if ((s->sem=t_semGet(1))==0){
		perror("t_semGet");
		serverClear(s);
		return 0;
	}
	t_semSet(s->sem,0,1);
	//TODO: add auth 
	s->id=serverIdByAddress(host,port);
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
	if (s==0)
		return;
	t_semSet(s->sem,0,-1);
		bintreeErase(&s->clients, (void(*)(void*))clientClearServer);
	t_semSet(s->sem,0,1);
	t_semRemove(s->sem);
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

typedef
struct server_search_params{
	int id;
	int val;
} server_search_params;

static void* findAuto(bintree_key k, void *v, void *arg){
	server_search_params *d=arg;
	server *s=v;
	if (!s->broken && d->val>=s->$clients){
		d->id=s->id;
		d->val=s->$clients;
//		return &s->id;
	}
	return 0;
}

int serversGetIdAuto(){//TODO: change to find free
	server_search_params d={0};
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, findAuto, &d);
	t_semSet(sem,0,1);
	return d.id;
}

static void* setUncheck(bintree_key k, void *v, void *arg){
	server *s=v;
	s->checked=0;
	return 0;
}

static void* checkS(bintree_key k, void *v, void *arg){
	server *s=v;
	if (s->checked==0){
		worklistAdd(arg, v);
	}
	return 0;
}

static int checkSlaves(slave_info *si,void *arg){
	int id=serverIdByAddress(si->host, si->port);
	server *s=bintreeGet(&servers, id);
	if (s==0){
		s=serverNew(si->host, si->port);
		bintreeAdd(&servers, id, s);
	}else{
		if (s->broken)
			serverReconnect(s);
	}
	if (s && !s->broken){
		s->checked=1;
		//add message server created
	}
	return 0;
}

static void* checkR(void *v, void *arg){
	server *s=v;
	serverClear(s);
	//add move users
	return v;
}

void serversCheck(){
	worklist l;
	memset(&l,0,sizeof(l));
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, setUncheck, 0);
	t_semSet(sem,0,1);
	t_semSet(sem,0,-1);
		storageSlaves(checkSlaves, 0);
	t_semSet(sem,0,1);
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, checkS, &l);
	t_semSet(sem,0,1);
	worklistForEachRemove(&l,checkR, 0);
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
		int _id=*((typeof(_id)*)(buf+(size-=sizeof(_id))));//check for write size size
		char dir=*((typeof(dir)*)(buf+(size-=sizeof(dir))));
		packetSetSize(p, size);
		if (dir==MSG_CLIENT){ //redirect packet to client
			client* c=0;
			if (_id==0 || (c=clientsGet(_id))!=0){
				clientMessageAdd(c, clientMessageNew(buf, size));
			}
		}else if (dir==MSG_SERVER){ //redirect packet to server
			server* s=serversGet(_id);
			packetAddChar(p, MSG_SERVER);//message from server
			packetAddNumber(p, s->id);
			if (s){
				packetSend(p, s->sock);
			}else if (_id==0){
				serversPacketSendAll(s, p);
			}
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

static void* serversSendPacket(bintree_key k, void* v, void * p){
	server* s=v;
	packetSend(p, s->sock);
	return 0;
}

void serversPacketSendAll(server *s, packet* p){
	t_semSet(sem,0,-1);
		bintreeForEach(&servers, serversSendPacket, p);
	t_semSet(sem,0,1);
}

int serverClientsAdd(server *s, void *_c){
	client *c=_c;
	c->server_id=s->id;
	t_semSet(s->sem,0,-1);
		bintreeAdd(&s->clients, c->id, c);
		s->$clients++;
	t_semSet(s->sem,0,1);
	return 0;
}

int serverClientsRemove(server *s, void *_c){
	client *c=_c;
	t_semSet(s->sem,0,-1);
		bintreeDel(&s->clients, c->id, (void(*)(void*))clientClearServer);
	s->$clients--;
	t_semSet(s->sem,0,1);
	return 0;
}
