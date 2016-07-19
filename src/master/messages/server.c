#include <stdio.h>
#include <string.h>

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

#define serverMessageProcessor(id)\
	messageprocessorServerAdd(id, &message ## id)

#define voidMessageProcessor(i)\
	static void *message ## i(server*s, packet* p){\
	return 0;\
}

//functions to proceed messages from slave servers, named messageN where N is message type, messages without processors will be sent to client
static void* addAttrToPacket(char* k, char* v, void * p){
	packetAddString(p,k);
	packetAddString(p,v);
	return 0;
}
///get client attributes 
///in: {1, n, 3, 6[n], int, string[n]} strings of attributes names
/// out: {1, n, 3, 6[2*n], int, string[2*n]}
static void *message1(server *sv, packet *p){
	FILE *f=packetGetStream(p);
	char c;
	short size,s;
	int id, i;
	if (f){	
		char **keys=0;
		short $keys;
		size=fread(&c,sizeof(c),1,f);//packet type
		size=fread(&c,sizeof(c),1,f);//attributes number
//		printf("%d\n",c);
		$keys=c-1;
		for(i=c;i>0;i--){
			size=fread(&c,sizeof(c),1,f);
//			printf("%d\n",c);
		}
		size=fread(&id,sizeof(id),1,f);//attributes number
		if ((keys=malloc(sizeof(*keys)*$keys))!=0){
			memset(keys, 0, sizeof(*keys)*$keys);
			for(i=0;i<$keys;i++){
				size=fread(&s,sizeof(s),1,f);
				if ((keys[i]=malloc(sizeof(char)*s))!=0){
					size=fread(keys[i],s,1,f);
					keys[i][size]=0;
//					printf("%d\n",c);
				}
			}
			fclose(f);
			packetInitFast(p);
			packetAddChar(p, MSG_S_CLIENT_ATTRIBUTES);
			packetAddChar(p, $keys*2+1);
			packetAddChar(p, 3);//int
			for(i=0;i<$keys*2;i++){
				packetAddChar(p, 6);
			}
			packetAddNumber(p, id);//int
			storageAttributesForEach(id, keys, $keys, addAttrToPacket, p);
			packetSend(p, sv->sock);
			
			for(i=0;i<$keys;i++){
				free(keys[i]);
			}
			free(keys);
			return 0;
		}

		fclose(f);
	}
	return 0;//check for error return
}

///set client attributes {2, n, 3, 6[n], int, string[n]} key value pairs of strings of attributes
static void *message2(server *sv, packet *p){
	char **keys, **values, *data=packetGetData(p);
	char c=*(data++);
	short s, $keys=(c-1)/2;
	int i, id=*((typeof(id)*)(data+=c));
	data+=sizeof(id);
	if((keys=malloc($keys*sizeof(*keys)))!=0){
		if((values=malloc($keys*sizeof(*values)))!=0){
			for(i=0;i<$keys;i++){
				s=*((typeof(s)*)data);
				*data=0;
				data+=sizeof(s);
				keys[i]=data;
				data+=s;
				s=*((typeof(s)*)data);
				*data=0;
				data+=sizeof(s);
				values[i]=data;
				data+=s;
			}
			storageAttributesSet(id, keys, values, $keys);
			//add success message
			free(values);
		}
		free(keys);
	}
	return 0;
}

voidMessageProcessor(3)
voidMessageProcessor(4)
voidMessageProcessor(5)
voidMessageProcessor(6)
voidMessageProcessor(7)
voidMessageProcessor(8)
voidMessageProcessor(9)
voidMessageProcessor(10)
voidMessageProcessor(11)
voidMessageProcessor(12)
voidMessageProcessor(13)
voidMessageProcessor(14)
voidMessageProcessor(15)
voidMessageProcessor(16)
voidMessageProcessor(17)
voidMessageProcessor(18)
voidMessageProcessor(19)
voidMessageProcessor(20)


void serverMessageProcessorInit(){
	serverMessageProcessor(1);
	serverMessageProcessor(2);
	serverMessageProcessor(3);
	serverMessageProcessor(4);
	serverMessageProcessor(5);
	serverMessageProcessor(6);
	serverMessageProcessor(7);
	serverMessageProcessor(8);
	serverMessageProcessor(9);
	serverMessageProcessor(10);
	serverMessageProcessor(11);
	serverMessageProcessor(12);
	serverMessageProcessor(13);
	serverMessageProcessor(14);
	serverMessageProcessor(15);
	serverMessageProcessor(16);
	serverMessageProcessor(17);
	serverMessageProcessor(18);
	serverMessageProcessor(19);
	serverMessageProcessor(20);
}

