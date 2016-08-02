#ifndef PACKET_HEADER
#define PACKET_HEADER

#include <stdio.h>

#include "bytes_order.h"
#include "socket.h"

#define PACKET_SIZE 32768 //signed short
typedef
struct{
	short $buf;
	char buf[PACKET_SIZE];
} packet;

//create new packet
packet* packetNew(short size);

packet* packetInit(packet *p);
#define packetInitFast(p) packetSetSize(p,0)

//add data to packet, if size of packet more than PACKET_SIZE, send it and start new ( )
int packetAddData(packet *p, void* data, int size);

//universal function data must be variable
#define packetAddNumber(pack, data) ({\
		typeof(data) $=byteSwap(data);\
		packetAddData(pack, &$, sizeof($));\
	})

//specific functions for using numbers directly
//(char[]){1}
#define packetAddChar(pack, data) ({\
		char $=data;\
		packetAddData(pack, (char[]){1}, sizeof(char));\
		packetAddData(pack, &$, sizeof($));\
	})

#define packetAddShort(pack, data) ({\
		short $=data;$=byteSwap($);\
		packetAddData(pack, (char[]){2}, sizeof(char));\
		packetAddData(pack, &$, sizeof($));\
	})

#define packetAddInt(pack, data) ({\
		int $=data;$=byteSwap($);\
		packetAddData(pack, (char[]){3}, sizeof(char));\
		packetAddData(pack, &$, sizeof($));\
	})

#define packetAddFloat(pack, data) ({\
		float $=data;$=byteSwap($);\
		packetAddData(pack, (char[]){4}, sizeof(char));\
		packetAddData(pack, &$, sizeof($));\
	})

#define packetAddDouble(pack, data) ({\
		double $=data;$=byteSwap($);\
		packetAddData(pack, (char[]){5}, sizeof(char));\
		packetAddData(pack, &$, sizeof($));\
	})

#define packetAddString(pack, data) ({\
		short $data=strlen(data);\
		short _data=byteSwap($data);\
		packetAddData(pack, (char[]){6}, sizeof(char));\
		packetAddData(pack,&_data,sizeof($data)) +\
		packetAddData(pack,data,$data);\
	})

//send packet and free memory ( <=0 for error)
int packetSend(packet *p, socket_t *sock);

int packetReceive(packet* p, socket_t *sock); //buf must have anough space

void *packetGetData(packet *p);
FILE *packetGetStream(packet *p);

short packetGetSize(packet *p);
void packetSetSize(packet *p, short size);

#endif