#include <string.h>

#include "client.h"
#include "server.h"
#include "../client.h"
#include "../server.h"
#include "../storage.h"
#include "../messageprocessor.h"
#include "../../share/network/packet.h"
#include "../../share/system/log.h"
#include "../../share/base64.h"
#include "../../share/md5.h"

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
static void *message1(client*cl, packet* p){
	FILE* f=packetGetStream(p);
	char c;
	int i, size;
	short s;
	char buf[100];
	if (f){	
		size=fread(&c,sizeof(c),1,f);//
		size=fread(&c,sizeof(c),1,f);
		for(i=c;i>0;i--){
			size=fread(&c,sizeof(c),1,f);
		}
		size=fread(&c,sizeof(c),1,f);
		do{
			if (c==1){
				user_info u;
				size=fread(&s,sizeof(s),1,f);//size
				size=fread(buf,s,1,f);//name
				buf[s]=0;
				//find client by name
				if(storageUserByName(buf, &u)==0){//if we found user
					clientSetInfo(cl,&u);
					clientsAdd(cl);
					char md5[20];
					MD5_Create((void*)&s,sizeof(s),md5);
					base64_encode((void*)md5, (void*)cl->token,16, 0);
					size=0;
					buf[0]=2;size++;
					buf[1]=1;size++;
					buf[2]=6;size++;
					s=2;//real sizeof token
					memcpy(buf+size,&s,sizeof(s));size+=sizeof(s);
					memcpy(buf+size,cl->token,s);size+=s;
					clientMessageAdd(cl, clientMessageNew(buf, size));
					break;
				}
			}else if(c==2){
				size=fread(&s,sizeof(s),1,f);//size
				size=fread(buf,s,1,f);//hash
				buf[s]=0;
				if (strcmp(buf,cl->token)==0)//add normal token check
					//auth ok
					break;
			}
			/////
//			c->id=rand();//for tests
//			c->server_id=serversGetIdAuto();//for tests
			//////
			fclose(f);
			return cl;
		}while(0);
		fclose(f);
	}
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