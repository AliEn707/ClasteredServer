#include <string.h>

#include "chat.h"
#include "client.h"
#include "../share/containers/bintree.h"
#include "../share/containers/worklist.h"

static bintree chats={0};
static t_sem_t sem=0;

void chatsInit(){
	memset(&chats, 0, sizeof(chats));
	if ((sem=t_semGet(1))==0){
		perror("t_semGet");
		return;
	}
	t_semSet(sem,0,1);
}

void chatsClear(){
	t_semSet(sem,0,-1);
		bintreeErase(&chats, (void(*)(void*))chatClear);
	t_semSet(sem,0,1);
	t_semRemove(sem);
}

chat* chatNew(){
	chat *c;
	if ((c=malloc(sizeof(*c)))==0){
		perror("malloc");
		return 0;
	}
	memset(c,0,sizeof(*c));
	if ((c->sem=t_semGet(1))==0){
		perror("t_semGet");
		chatClear(c);
		return 0;
	}
	t_semSet(c->sem,0,1);
	return c;
}

static void* clearR(bintree_key k, void *v, void *arg){
	worklist *l=arg;
	client *c=v;	
	worklistAdd(l,c);
	return 0;
}

static void* removeR(void *_c, void *arg){
	client *cl=_c;
	chat* c=arg;
	clientChatsRemove(cl, c);
	return c;
}

void chatClear(chat* c){
	if (c==0)
		return;
	worklist list;
	memset(&list,0,sizeof(list));
	t_semSet(c->sem,0,-1);
		bintreeForEach(&c->clients, clearR, &list);
	t_semSet(c->sem,0,1);
	worklistForEachRemove(&list, removeR, c);
	t_semRemove(c->sem);
	free(c);
}

void chatClientsAdd(chat* c, void* _client){
	client* cl=_client;
	t_semSet(c->sem, 0, -1);
	if (bintreeAdd(&c->clients, cl->id,  cl)){
		t_semSet(c->sem,0,1);
		clientChatsAdd(cl, c);
		return;
	}
	t_semSet(c->sem,0,1);
}

void* chatClientsGet(chat* c, int id){
	client* cl=0;
	t_semSet(c->sem, 0, -1);
		cl=bintreeGet(&c->clients, id);
	t_semSet(c->sem,0,1);
	return cl;
}

void chatClientsRemove(chat* c, void* _client){
	client* cl=_client;
	client* found=0;
	if (cl==0 || c==0)
		return;
	void chatClientsRemoveClient(void* data){
		found=data;
	}
	t_semSet(c->sem, 0, -1);
		bintreeDel(&c->clients, cl->id,  chatClientsRemoveClient);//check for deadlock
	t_semSet(c->sem,0,1);
	clientChatsRemove(found, c);
}

int chatsAdd(chat* c){
	if (c && c->id!=0){
		t_semSet(sem,0,-1);
			bintreeAdd(&chats, c->id, c);
		t_semSet(sem,0,1);
	}
	return c->id;
}

chat* chatsGet(int id){
	chat *c;
	t_semSet(sem,0,-1);
		c=bintreeGet(&chats, id);
	t_semSet(sem,0,1);
	return c;
}

static void* checkC(bintree_key k, void *v, void *arg){
	worklist *l=arg;
	chat *c=v;	
	if (0){//add check for chat
		worklistAdd(l,c);
	}
	return 0;
}

static void* removeC(void *_c, void *arg){
	chat *c=_c;
	printf("chat %d removed\n", c->id);
	t_semSet(sem,0,-1);
		bintreeDel(&chats, c->id, (void(*)(void*))chatClear);
	t_semSet(sem,0,1);
	return c;
}

void chatsCheck(){
	worklist list;
	memset(&list,0,sizeof(list));
	t_semSet(sem,0,-1);
		bintreeForEach(&chats, checkC, &list);
	t_semSet(sem,0,1);
	worklistForEachRemove(&list, removeC, 0);
}

void chatsRemove(chat* c){
	t_semSet(sem,0,-1);
		bintreeDel(&chats, c->id, (void(*)(void*))chatClear);
	t_semSet(sem,0,1);
}


/*

void chats_test(){
	void* chatP(bintree_key k, void* v, void* arg){
		client* c=v;
		printf("chat %d\n", c->id);
		return 0;
	}
	void* clientP(bintree_key k, void* v, void* arg){
		client* c=v;
		printf("client %d\n", c->id);
		return 0;
	}
	printf("chats test\n");
	client* c[3]={
		clientNew(socketNew(0)),
		clientNew(socketNew(0)),
		clientNew(socketNew(0))
	};
	chat * ch=chatNew();
	chat * ch2=chatNew();
	c[0]->id=1;
	c[1]->id=2;
	c[2]->id=3;
	ch->id=1;
	ch2->id=2;
	clientChatsAdd(c[0], ch);
//	clientChatsAdd(c[1], ch);
	bintreeForEach(&c[0]->chats, chatP, 0);
	bintreeForEach(&c[1]->chats, chatP, 0);
	bintreeForEach(&ch->clients, clientP, 0);
	chatClientsRemove(ch,c[0]);
	printf("removed client 0\n");
	bintreeForEach(&c[0]->chats, chatP, 0);
	bintreeForEach(&c[1]->chats, chatP, 0);
	bintreeForEach(&ch->clients, clientP, 0);
	clientChatsAdd(c[0], ch);
	clientChatsAdd(c[0], ch2);
	chatClientsAdd(ch,c[2]);
	printf("added 2\n");
	bintreeForEach(&c[0]->chats, chatP, 0);
	bintreeForEach(&c[1]->chats, chatP, 0);
	bintreeForEach(&ch->clients, clientP, 0);
	printf("clear\n");
	clientClear(c[0]);
	bintreeForEach(&c[0]->chats, chatP, 0);
	bintreeForEach(&c[1]->chats, chatP, 0);
	
	printf("success\n");
}

log_config config={0};

log_config* mainLogConfig(){
	return &config;
}

int main(int argc,char* argv[]){
	config.log.debug=1;
	chats_test();
	return 0;

*/

