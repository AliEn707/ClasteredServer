#ifndef LISTENER_HEADER
#define LISTENER_HEADER

#include "../containers/worklist.h"
#include "socket.h"

#define PRIVATE_POLICY_HTTP_HEADER "HTTP/1.1 200 OK\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: 88\r\nConnection: close\r\n\r\n"
#define PRIVATE_POLICY "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>"

typedef 
struct {
	int sockfd;
	short broken;
} listener;

listener* listenerStart(int port);

listener* listenerNew(int sock);

void listenerClear(listener * l);

#endif
