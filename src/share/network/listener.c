#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include "listener.h"
#include "socket.h"
#include "../system/log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ listen socket implementation 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

listener* listenerStart(int port){
	int listenerfd;
	struct sockaddr_in addr;
	printf("listener start on %d\n", port);
	if((listenerfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("socket");
		return 0;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenerfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind");
		close(listenerfd);
		return 0;
	}
	
	if(listen(listenerfd, 1)<0){
		perror("listen");
		close(listenerfd);
		return 0;
	}
	
	printf("listener on %d started\n", port);
	return listenerNew(listenerfd);
}

listener* listenerNew(int sock){
	listener * l;
	if ((l=malloc(sizeof(*l)))==0){
		perror("malloc");
		close(sock);
		return 0;
	}
	l->sockfd=sock;
	printf("listener fd=%d created\n", l->sockfd);
	return l;
}

void listenerClear(listener * l){
	close(l->sockfd);
	printf("listener fd=%d cleared\n", l->sockfd);
	free(l);
}

socket_t *listenerAccept(listener *l){
	int sockfd;
		if ((sockfd = accept(l->sockfd, 0, 0))<0){
			perror("accept");
			l->broken=1;
		}else{
			return socketNew(sockfd);
		}
return 0;
}