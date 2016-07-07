#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h> 
#include <errno.h>
#include <netdb.h>

#include "socket.h"
#include "bytes_order.h"
#include "../system/t_sem.h"
#include "../system/log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with sockets 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

socket_t *socketNew(int sockfd){
	socket_t *sock;
	if ((sock=malloc(sizeof(*sock)))==0){
		perror("malloc sock");
		close(sockfd);
		return 0;
	}	
	memset(sock,0,sizeof(*sock));
	sock->sockfd=sockfd;
	if ((sock->sem.read=t_semGet(1))==0){
		perror("malloc sem.read");
		socketClear(sock);
		return 0;
	}
	t_semSet(sock->sem.read, 0, 1);
	if ((sock->sem.write=t_semGet(1))==0){
		perror("malloc sem.write");
		socketClear(sock);
		return 0;
	}
	t_semSet(sock->sem.write, 0, 1);
	return sock;
}

int socketClear(socket_t *sock){
	if (sock==0)
		return 0;
	close(sock->sockfd);
	t_semRemove(sock->sem.read);
	t_semRemove(sock->sem.write);
	free(sock);
	return 0;
}


socket_t *socketConnect(char *host, int port){
	int sockfd;
	struct sockaddr_in servaddr;
	struct hostent *server;
	server = gethostbyname(host);
	if (server == NULL) {
		perror("gethostbyname");
		return 0;
	}
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		return 0;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	memcpy((char *)&servaddr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
//	servaddr.sin_addr.s_addr=inet_addr("172.16.1.40");//argv[1]);
	servaddr.sin_port=htons(port);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		perror("connect");
		close(sockfd);
		return 0;
	}
	
	return socketNew(sockfd);
}


void socketSemWrite(socket_t* sock, int act){
	t_semSet(sock->sem.write, 0, act);
}

void socketSemRead(socket_t* sock, int act){
	t_semSet(sock->sem.read, 0, act);
}

int socketSend(socket_t *sock, void * buf, int size){
	int need=size;
	int get;
	get=send(sock->sockfd,buf,need,MSG_NOSIGNAL);
	if (get<=0)
		return get;
	if (get==need)
		return get;
	printf("send not all\n");
	while(need>0){
		need-=get;
		if((get=send(sock->sockfd,buf+(size-need),need,MSG_NOSIGNAL))<=0)
			return get;
	}
	return size;
}

//80ms*18 ~ 1440ms waiting
#ifndef RECV_SLEEP_TIME
#define RECV_SLEEP_TIME 80000
#endif

#ifndef RECV_MAX_RETRIES
#define RECV_MAX_RETRIES 18
#endif

int socketRecv(socket_t *sock, void * buf, int size){
	int need=size;
	int get;
	get=recv(sock->sockfd,buf,need,MSG_DONTWAIT);
	if (get==0)
		return 0;
	if (get<0)
		if (errno!=EAGAIN)
			return -1;
	if (get==need)
		return get;
	printf("get not all\n");
	int $_$=0;
	do{
		printf("try %d to read %d\n", $_$, need);
		if (get>0)
			need-=get;
//		printf("try to get\n");
		if((get=recv(sock->sockfd,buf+(size-need),need,MSG_DONTWAIT))<=0)
			if (errno!=EAGAIN)
				return -1;
		usleep(RECV_SLEEP_TIME);
		$_$++;
		if ($_$>RECV_MAX_RETRIES)//max tries of read
			return -1;
	}while(need>0);
	return size;
}

socket_t* socketRecvCheck(socket_t *sock){
	struct pollfd poll_set;
	poll_set.fd = sock->sockfd;
	poll_set.events = POLLIN;
	poll_set.revents = 0;
	if (poll(&poll_set, 1, 1)>0)
		return sock;
	return 0;
}
