#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "listenworkers.h"
#include "socketworkers.h"
#include "../../share/workers/workerbase.h"
#include "../../share/system/log.h"
#include "../client.h"
#include "../server.h"
#include "../../share/network/listener.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	workers waiting for clients			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


#define MAX_WORKERS 400

#define max(a,b) ((a)<(b)?(b):(a))

static short total=0;
static worker listenworkers[MAX_WORKERS];
static listenworker_data data[MAX_WORKERS];
static t_sem_t sem;

static void init(void* _w){
	worker* w=_w;
	listenworker_data *wd=w->data;
	//add some actions for every work element
	sprintf(w->name,"ListenWorker %d",w->id);
	printLog("%s created\n",w->name);
	memset(&wd->set,0,sizeof(wd->set));
	FD_ZERO(&wd->set);
	wd->maxfd=0;
}

static void* check(void*data,void*_w){
	listener *l=data;
	worker* w=_w;
	listenworker_data *wd=w->data;
	
	if (FD_ISSET(l->sockfd, &wd->set)){
		int sockfd;
		if ((sockfd = accept(l->sockfd, 0, 0))<0){
			perror("accept");
			l->broken=1;
		}else{
			return socketNew(sockfd);
		}
	}
	return 0;
}

static void loop(void* _w){
	worker* w=_w;
	listenworker_data *wd=w->data;
	socket_t *sock=0;
	struct timeval t={0, 80000};
	//packetInit(&wd->packet);
	//printf("1\t%d\n", w->sem->val);
	//add some actions for every iteration
	t_semSet(sem,0,-1);
		if(select(wd->maxfd+1, &wd->set, 0, 0, &t)>0){
			sock=worklistForEachReturn(&w->works,check,_w);
		}
	t_semSet(sem,0,1);
	if (sock){
		printf("%s: client connected\n", w->name);
		do {
			short size;
			char c,buf[100];
			packetInit(&wd->packet);
			socketRecv(sock, buf, 3);
			buf[3]=0;
			//size=*((short*)buf);
			c=buf[2];
			if (c==0){//socket
				//mestype 0 this is hello mes, in other place it is blank mes
				printf("%s: try as pure socket\n", w->name);
				socketRecvNumber(sock, c);//elements
				if (c==1){
					socketRecvNumber(sock, c);//element type
					if (c==6){
						size=socketRecvString(sock, buf);
						buf[size]=0;
						//check buf as client key
						//send answer
						c=1;
						packetAddNumber(&wd->packet, c);
						packetAddNumber(&wd->packet, c);
						packetAddNumber(&wd->packet, c);
						packetAddNumber(&wd->packet, c);
						packetSend(&wd->packet, sock);//[1,1,1,1]
						printf("%s: got %s client go to worker\n", w->name, buf);
						socketworkersAddWorkAuto(clientNew(sock));//after that client must get id
						break;
					}
				}
			}else if (strstr(buf,"<po")!=0 ){
				//Flash policy 
				socketSend(sock,PRIVATE_POLICY,sizeof(PRIVATE_POLICY));
				socketClear(sock);
				break;//already clear socket
			}else if (strstr(buf,"GET")!=0){//TODO: add web socket
				//Javaapplet policy
				socketSend(sock,PRIVATE_POLICY_HTTP_HEADER,sizeof(PRIVATE_POLICY_HTTP_HEADER)-1);
				socketSend(sock,PRIVATE_POLICY,sizeof(PRIVATE_POLICY)-1);
				socketClear(sock);
				break;//already clear socket
			}else if (strstr(buf,"POS")!=0){ //Http-Rest, used only post requests
					//TODO: add chose different worker
			}
			printf("%s: client failed\n", w->name);
			socketClear(sock);
		}while(0);
	}
	FD_ZERO(&wd->set);
	wd->maxfd=0;
}

static void* proceed(void*data,void*_w){
	worker* w=_w;
	listenworker_data *wd=w->data;
	listener *l=data;
	if (!l->broken){
		FD_SET(l->sockfd, &wd->set);
		wd->maxfd = max(l->sockfd, wd->maxfd);
	}
	return 0;
}

static void* close(void* _w){
	//worker* w=_w;
	return 0;
}

#define listenworkersActionAll(action)\
	void listenworkers ## action ## All(){\
		int i;\
		for(i=0;i<total;i++)\
			workerbase ## action (&listenworkers[i]);\
	}

listenworkersActionAll(Start)
listenworkersActionAll(Stop)
listenworkersActionAll(Pause)
listenworkersActionAll(Unpause)

#define listenworkersAction(action)\
	void listenworkers ## action(int n){\
		if (n<MAX_WORKERS && n>=0)\
			workerbase ## action (&listenworkers[n]);\
	}

listenworkersAction(Start)
listenworkersAction(Stop)
listenworkersAction(Pause)
listenworkersAction(Unpause)

void listenworkersAddWorkAll(void* work){
	int i;
	for(i=0;i<total;i++)
		workerbaseAddWork(&listenworkers[i], work);
}

void listenworkersAddWork(int n, void* work){
	if (n<MAX_WORKERS)
		workerbaseAddWork(&listenworkers[n], work);
}

int listenworkersAddWorkAuto(void* work){
	int i;
	int n=0;
	int $works=listenworkers[n].$works;
	for(i=0;i<total;i++){
		if (listenworkers[i].$works<$works){
			$works=listenworkers[i].$works;
			n=i;
		}
	}
	workerbaseAddWork(&listenworkers[n], work);
	return n;
}

int listenworkersCreate(int num, int TPS){
	int i;
	memset(listenworkers, 0, sizeof(listenworkers));
	total=num;
	sem=t_semGet(1);
	t_semSet(sem,0,1);
	if (total>MAX_WORKERS){
		total=MAX_WORKERS;
		printLog("set num to %d\n",MAX_WORKERS);
	}
	for(i=0;i<total;i++){
		listenworkers[i].id=i;
		listenworkers[i].TPS=TPS;
		listenworkers[i].init=&init;
		listenworkers[i].loop=&loop;
		listenworkers[i].proceed=&proceed;
		listenworkers[i].close=&close;
		listenworkers[i].data=&data[i];
		workerbaseCreate(&listenworkers[i]);
	}
	return 0;
}

void listenworkersClose(){
	listenworkersStopAll();
	t_semRemove(sem);
}