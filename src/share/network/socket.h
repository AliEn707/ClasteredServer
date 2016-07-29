#ifndef SOCKET_HEADER
#define SOCKET_HEADER

#include "../system/t_mutex.h"
#include "bytes_order.h"


typedef
struct {
	int sockfd;
	struct{
		t_mutex_t write;
		t_mutex_t read;
	}mutex;
	void (*encode)(void*buf,short,void*arg);
	void *_encode;
	void (*decode)(void*buf,short,void*arg);
	void *_decode;
} socket_t;

socket_t *socketNew(int sockfd);
int socketClear(socket_t *sock);

socket_t *socketServer(int port);
socket_t *socketConnect(char *host, int port);

void socketSemWrite(socket_t *sock, int act);
void socketSemRead(socket_t *sock, int act);

int socketSend(socket_t *sock, void *buf, int size);

#define socketSendNumber(sock, x) ({\
		typeof(x) _x=byteSwap(x);\
		socketSend(sock,&_x,sizeof(_x));\
	})
	
#define socketSendString(sock, x) ({\
		short $x=strlen(x);\
		short _x=byteSwap($x);\
		socketSend(sock,&_x,sizeof($x)) +\
		socketSend(sock,x,$x);\
	})
	
int socketRecv(socket_t *sock, void *buf, int size);

#define socketRecvNumber(sock, x) ({\
		int $x=socketRecv(sock,&x,sizeof(x));\
		x=byteSwap(x);\
		$x;\
	})

#define socketRecvString(sock, x) ({\
		short $x,_x;\
		if ((_x=socketRecv(sock,&$x,sizeof($x)))>0){\
			short __x;\
			$x=byteSwap($x);\
			if ((__x=socketRecv(sock,x,$x))>0){\
				_x+=__x;\
				x[__x]=0;\
			}else\
			_x=__x;\
		}\
		_x;\
	})

//check for socket data available
socket_t* socketRecvCheck(socket_t *sock);

void socketClose(socket_t *s);

#endif