#include <string.h>

#include "client.h"
#include "server.h"
#include "storage.h"
#include "messageprocessor.h"
#include "../share/containers/worklist.h"
#include "../share/containers/bintree.h"
#include "../share/network/socket.h"
#include "../share/network/packet.h"
#include "../share/system/log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	implementation of connected clients 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

typedef void*(*client_processor)(client*, packet*);

static bintree clients={0};
static t_sem_t sem=0;

void clientsInit(){
	memset(&clients, 0, sizeof(clients));
	if ((sem=t_semGet(1))==0){
		perror("t_semGet");
		return;
	}
	t_semSet(sem,0,1);
}

void clientsClear(){
	t_semSet(sem,0,-1);
		bintreeErase(&clients, (void(*)(void*))clientClear);
	t_semSet(sem,0,1);
	t_semRemove(sem);
}

client* clientNew(socket_t *sock){
	client *c;
	if ((c=malloc(sizeof(*c)))==0){
		perror("malloc");
		return 0;
	}
	memset(c,0,sizeof(*c));
	c->sock=sock;
	if ((c->sem=t_semGet(1))==0){
		perror("t_semGet");
		clientClear(c);
		return 0;
	}
	t_semSet(c->sem,0,1);
	return c;
}

void clientClear(client* c){
	if (c==0)
		return;
	t_semSet(c->sem,0,-1);
		socketClear(c->sock);
		worklistErase(&c->messages, (void(*)(void*))clientMessageClear);
	t_semSet(c->sem,0,1);
	t_semRemove(c->sem);
	free(c);
}

int clientsAdd(client* c){
	if (c->id!=0){
		t_semSet(sem,0,-1);
			bintreeAdd(&clients, c->id, c);
		t_semSet(sem,0,1);
	}
	return c->id;
}

client* clientsGet(int id){
	client *c;
	t_semSet(sem,0,-1);
		c=bintreeGet(&clients, id);
if (c!=0 && c->broken)
c=0;
	t_semSet(sem,0,1);
	return c;
}

static void* checkC(bintree_key k, void *v, void *arg){
	worklist *l=arg;
	client *c=v;
	if (c->broken)
		if (abs(time(0)-c->timestamp)>10)//10 seconds for reconnect
			worklistAdd(l,c);
	return 0;
}

static void* removeC(void *_c, void *arg){
	client *c=_c;
	bintreeDel(&clients, c->id, (void(*)(void*))clientClear);
	return c;
}

void clientsCheck(){
	worklist list;
	memset(&list,0,sizeof(list));
	t_semSet(sem,0,-1);
		bintreeForEach(&clients, checkC, &list);
		worklistForEachRemove(&list, removeC, 0);
	t_semSet(sem,0,1);
}

void clientsRemove(client* c){
	t_semSet(sem,0,-1);
		bintreeDel(&clients, c->id, (void(*)(void*))clientClear);
	t_semSet(sem,0,1);
}

int clientPacketProceed(client *c, packet *p){
	char* buf;
	buf=packetGetData(p);
	client_processor processor;
	//void*(*processor)(packet*);
	if ((processor=messageprocessorClient(*buf))==0){
//	if (*buf<0){//proxy
		//add client data to the end
		packetAddNumber(p, c->id);
		server* s=serversGet(c->server_id);
		if (s){
			packetSend(p, s->sock);
		}else{
			printf("client %d server %d error\n", c->id, c->server_id);
			return 1;
		}
	}else{//proceed by self
		processor(c, p);
	}
	return 0;
}

static void* clientAddEach(bintree_key k,void *v,void *arg){
	client *c=v;
	client_message *m=arg;
	t_semSet(c->sem,0,-1);
		worklistAdd(&c->messages, m);
	t_semSet(c->sem,0,1);
	t_semSet(m->sem, 0,-1);
		m->num++;
	t_semSet(m->sem, 0,1);
	return 0;
}

void clientMessageAdd(client* c, client_message *m){
	if (m){
		if (c){
			m->num=1;
			m->ready=1;
			t_semSet(c->sem,0,-1);
				worklistAdd(&c->messages, m);
			t_semSet(c->sem,0,1);
			//find client and add
		}else{
			//add to all, and then
			t_semSet(sem,0,-1);
				bintreeForEach(&clients, clientAddEach, m);
			t_semSet(sem,0,1);
			t_semSet(m->sem, 0,-1);
				m->ready=1;
			t_semSet(m->sem, 0,1);
		}
	}
}

client_message* clientMessageNew(void* buf, short size){
	client_message* m;
	if ((m=malloc(sizeof(*m)))==0){
		perror("malloc");
		return 0;
	}
	memset(m,0,sizeof(*m));
	if ((m->sem=t_semGet(1))==0){
		free(m);
		return 0;
	}
	t_semSet(m->sem,0,1);
	packetAddData(&m->packet,buf,size);
	return m;
}

void clientMessageClear(client_message* m){
	t_semSet(m->sem,0,-1);
		m->num--;
	t_semSet(m->sem,0,1);
	if (m->num==0){	
		t_semRemove(m->sem);
		free(m);
	}
}

void clientMessagesProceed(client *c, void* (*me)(void* d, void * _c)){
	t_semSet(c->sem,0,-1);
		worklistForEachRemove(&c->messages, me,c);
	t_semSet(c->sem,0,1);
}

int clientSetInfo(client *c, user_info *u){
	c->id=u->id;
	sprintf(c->name,"%s",u->name);
	//add other
	return 0;
}
