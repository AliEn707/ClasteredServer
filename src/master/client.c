#include <string.h>

#include "client.h"
#include "chat.h"
#include "server.h"
#include "storage.h"
#include "messageprocessor.h"
#include "../share/containers/worklist.h"
#include "../share/containers/bintree.h"
#include "../share/network/socket.h"
#include "../share/network/packet.h"
#include "../share/base64.h"
#include "../share/system/types.h"
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

static void* clientChatsRemoveEach(bintree_key k, void *v, void *arg){
	worklistAdd(arg, v);
	return 0;
}

static void* clientChatsRemoveList(void *c, void *arg){
	chatClientsRemove(c,arg);
	return arg;
}

void clientClear(client* c){
	if (c==0)
		return;
	worklist list;
	memset(&list, 0, sizeof(list));
	t_semSet(c->sem,0,-1);
		bintreeForEach(&c->chats, clientChatsRemoveEach, &list);//check for deadlock
	t_semSet(c->sem,0,1);
	worklistForEachRemove(&list, clientChatsRemoveList, c);
	t_semSet(c->sem,0,-1);
		if (c->sock)
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
	if (clientCriticalAuto(c, c->broken) || c->id==0)
		if (abs(time(0)-c->timestamp)>=0){//add 10 seconds for reconnect
			worklistAdd(l,c);
		}
	return 0;
}
static void* removeC(void *_c, void *arg){
	client *c=_c;
	printf("client %d removed\n", c->id);
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
	char* buf=packetGetData(p);
	client_processor processor;
	//void*(*processor)(packet*);
	printf("got message %d\n", *buf);
	if ((processor=messageprocessorClient(*buf))==0){
//	if (*buf<0){//proxy
//		printf("redirect message\n");
		//add client data to the end
		packetAddChar(p, MSG_CLIENT);
		packetAddNumber(p, c->id);
		server* s=serversGet(clientCriticalAuto(c, c->server_id));
		if (s==0){
			int id=serversGetIdAuto();
			if ((s=serversGet(id))!=0){
				serverClientsAdd(s, c);
			}else{
				socketClose(c->sock);
				printf("client %d server %d error\n", c->id, clientCriticalAuto(c, c->server_id));
				return 1;
			}
		}
		packetSend(p, s->sock);
	}else{//proceed by self
		return processor(c, p)!=0;
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

void clientMessagesAdd(client* c, client_message *m){
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
	m->$data=size;
	if ((m->data=malloc(sizeof(*m->data)*(m->$data+1)))==0){
		perror("malloc");
		free(m);
		return 0;
	}
	memcpy(m->data, buf, m->$data);
	m->data[m->$data]=0;
	if ((m->sem=t_semGet(1))==0){
		free(m->data);
		free(m);
		return 0;
	}
	t_semSet(m->sem,0,1);
	//packetAddData(&m->packet,buf,size);
	return m;
}

void clientMessageClear(client_message* m){
	t_semSet(m->sem,0,-1);
		m->num--;
	t_semSet(m->sem,0,1);
	if (m->num==0){	
		t_semRemove(m->sem);
		free(m->data);
		free(m);
	}
}

void clientChatsAdd(client* cl, void* _c){
	chat *c=_c;
	t_semSet(cl->sem,0,-1);
	if (bintreeAdd(&cl->chats, c->id, c)){
		t_semSet(cl->sem,0,1);
		chatClientsAdd(c, cl);
		return;
	}
	t_semSet(cl->sem,0,1);
}

void* clientChatsGet(client* cl, int id){
	chat *c=0;
	t_semSet(cl->sem,0,-1);
		c=bintreeGet(&cl->chats, id);
	t_semSet(cl->sem,0,1);
	return c;
}

void clientChatsRemove(client* cl, void* _c){
	chat *c=_c;
	chat* found=0;
	if (cl==0 || c==0)
		return;
	void clientChatsRemoveChat(void* data){
		found=data;
	}
	t_semSet(cl->sem,0,-1);
		bintreeDel(&cl->chats, c->id, clientChatsRemoveChat);//check for deadlock
	t_semSet(cl->sem,0,1);
	chatClientsRemove(found, cl);
}

void clientMessagesProceed(client *c, void* (*me)(void* d, void * _c), void *a){
	voidp2_t arg={c, a};
	t_semSet(c->sem,0,-1);
		worklistForEachRemove(&c->messages, me, &arg);
	t_semSet(c->sem,0,1);
}

int clientSetInfo(client *c, user_info *u){
	c->id=u->id;
	sprintf(c->name,"%s",u->name);
	sprintf(c->login,"%s",u->login);
	base64_decode((void*)u->passwd, (void*)c->passwd, strlen(u->passwd));
	//add other
	return 0;
}

void clientServerClear(client* c){
	if (c){
		t_semSet(c->sem,0,-1);
			c->server_id=0;
		t_semSet(c->sem,0,1);
	}
}

