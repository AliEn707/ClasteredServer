#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "socketworkers.h"
#include "../../share/containers/worklist.h"
#include "../../share/workers/workerbase.h"
#include "../../share/system/log.h"
#include "../client.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ workers for clients 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define MAX_WORKERS 400

static short total=0;
static worker socketworkers[MAX_WORKERS];
static soketworker_data data[MAX_WORKERS];

static void init(void *_w){
	worker* w=_w;
	soketworker_data *wd=w->data;
	wd->checks=10;
	//add some actions for every work element
	sprintf(w->name,"SocketWorker %d",w->id);
	printLog("%s created\n",w->name);
}

static void loop(void *_w){
	//add some actions for every iteration
}


static void* clientMessageEach(void* d, void * _c){
	client_message* m=d;
	client *c=_c;
	t_semSet(m->sem,0,-1);
	if (m->ready){
		t_semSet(m->sem,0,1);
		packetSend(&m->packet,c->sock);
		clientMessageClear(m);
		return d;
	}
	t_semSet(m->sem,0,1);
	return 0;
}

static void* proceed(void *data, void *_w){
	worker* w=_w;
	soketworker_data *wd=w->data;
	client *c=data;
	int i;
	clientMessagesProceed(c, clientMessageEach);
	for(i=0;i<wd->checks;i++){
		if (socketRecvCheck(c->sock)!=0){
			c->timestamp=time(0);
			do{
				if (packetReceive(&wd->packet, c->sock)>0){
					if (clientPacketProceed(c, &wd->packet)==0)
						break;
				}
				w->$works--;
				c->broken=1;
				printf("error with client\n");
				return _w;
			}while(0);
		}else
			break;
	}
	if (i>=wd->checks)
		w->recheck=1;
	return 0;
}

static void* close(void *_w){
	//worker* w=_w;
	return 0;
}

#define socketworkersActionAll(action)\
	void socketworkers ## action ## All(){\
		int i;\
		for(i=0;i<total;i++)\
			workerbase ## action (&socketworkers[i]);\
	}

socketworkersActionAll(Start)
socketworkersActionAll(Stop)
socketworkersActionAll(Pause)
socketworkersActionAll(Unpause)

#define socketworkersAction(action)\
	void socketworkers ## action(int n){\
		if (n<MAX_WORKERS && n>=0)\
			workerbase ## action (&socketworkers[n]);\
	}

socketworkersAction(Start)
socketworkersAction(Stop)
socketworkersAction(Pause)
socketworkersAction(Unpause)

void socketworkersAddWork(int n, void* work){
	if (n<MAX_WORKERS)
		workerbaseAddWork(&socketworkers[n], work);
}

int socketworkersAddWorkAuto(void* work){
	int i;
	int n=0;
	if (total==0)
		return -1;
	int $works=socketworkers[n].$works;
	for(i=0;i<total;i++){
		if (socketworkers[i].$works<$works){
			$works=socketworkers[i].$works;
			n=i;
		}
	}
	workerbaseAddWork(&socketworkers[n], work);
	return n;
}

int socketworkersCreate(int num, int TPS){
	int i;
	memset(socketworkers, 0, sizeof(socketworkers));
	total=num;
	if (total>MAX_WORKERS){
		total=MAX_WORKERS;
		printLog("set num to %d\n",MAX_WORKERS);
	}
	for(i=0;i<total;i++){
		socketworkers[i].id=i;
		socketworkers[i].TPS=TPS;
		socketworkers[i].init=&init;
		socketworkers[i].loop=&loop;
		socketworkers[i].proceed=&proceed;
		socketworkers[i].close=&close;
		socketworkers[i].data=&data[i];
		workerbaseCreate(&socketworkers[i]);
	}
	return 0;
}