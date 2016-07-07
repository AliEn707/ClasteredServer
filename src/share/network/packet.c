#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../network/socket.h"
#include "../system/log.h"
#include "packet.h"

//init packet
packet* packetInit(packet *p){
	if (p==0)
		if ((p=malloc(sizeof(*p)))==0){
			perror("malloc packetInit");
			return 0;
		}
	memset(p,0,sizeof(*p));
	return p;
}

//add data to packet, if size of packet more than PACKET_SIZE, send it and start new
int packetAddData(packet *p, void* data, int size){
	if (size>=PACKET_SIZE || p->$buf+size>=PACKET_SIZE){
		printf("packet too big");
		return 0;
	}else{
		memcpy(p->buf+p->$buf,data,size);
		p->$buf+=size;
	}
	return 1;
}

//send packet
int packetSend(packet *p, socket_t *sock){
	int o=0,_o;
	if (p->$buf<=0)
		return o;
	//TODO:add encoding
	if (sock->encode)
		sock->encode(p->buf,p->$buf,sock->_encode);
	socketSemWrite(sock,-1);
		if ((o=socketSendNumber(sock, p->$buf))>0){
			if((_o=socketSend(sock, p->buf, p->$buf))<=0)
				o=_o;
			else
				o+=_o;
		}
	socketSemWrite(sock,1);
	return o;
}

int packetReceive(packet* p, socket_t *sock){
	socketSemRead(sock,-1);
		p->$buf=socketRecvString(sock, p->buf);
	socketSemRead(sock,1);
	if (sock->decode)
		sock->decode(p->buf,p->$buf,sock->_decode);
	//TODO: add decoding
	return p->$buf;
}

short packetGetSize(packet *p){
	return p->$buf;
}

void packetSetSize(packet *p, short size){
	p->$buf=size;
	p->buf[size]=0;
}

void *packetGetData(packet *p){
	return p->buf;
}

FILE *packetGetStream(packet *p){
	return fmemopen(p->buf, p->$buf, "r+");
}
