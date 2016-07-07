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
packet* packetInit(packet *p);
//add data to packet, if size of packet more than PACKET_SIZE, send it and start new ( )
int packetAddData(packet *p, void* data, int size);

#define packetAddNumber(pack, data) ({\
		typeof(data) a=byteSwap(data);\
		packetAddData(pack, &a, sizeof(a));\
	})

#define packetAddString(pack, data) ({\
		short $data=strlen(data);\
		short _data=byteSwap($data);\
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