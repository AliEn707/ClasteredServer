#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "serverworkers.h"
#include "../../share/workers/workerbase.h"
#include "../../share/system/log.h"
#include "../server.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	workers works with slave servers 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define MAX_WORKERS 400

static short total=0;
static worker serverworkers[MAX_WORKERS];
static serverworker_data data[MAX_WORKERS];
static void init(void* _w){
	worker* w=_w;
	serverworker_data *wd=w->data;
	wd->checks=10;
	//add some actions for every work element
	sprintf(w->name,"ServerWorker %d",w->id);
	printLog("%s created\n",w->name);
}

static void loop(void *_w){
	//add some actions for every iteration
}

static void* proceed(void *data,void *_w){
	worker* w=_w;
	serverworker_data *wd=w->data;
	server *s=data;
	int i;
	for(i=0;i<wd->checks;i++){
		if (socketRecvCheck(s->sock)!=0){
			if (packetReceive(&wd->packet, s->sock)>0){
				serverPacketProceed(s, &wd->packet);
			}else{
				w->$works--;
				s->broken=1;
				printf("Server %d connection lost\n", s->id);
				return data;
			}
		}else
			break;
	}
	if (i>=wd->checks) //when we done wd->checks iterations, we need to recheck without sleep
		w->recheck=1;
	return 0;
}

static void* close(void* _w){
	//worker* w=_w;
	return 0;
}

#define serverworkersActionAll(action)\
	void serverworkers ## action ## All(){\
		int i;\
		for(i=0;i<total;i++)\
			workerbase ## action (&serverworkers[i]);\
	}

serverworkersActionAll(Start)
serverworkersActionAll(Stop)
serverworkersActionAll(Pause)
serverworkersActionAll(Unpause)

#define serverworkersAction(action)\
	void serverworkers ## action(int n){\
		if (n<MAX_WORKERS && n>=0)\
			workerbase ## action (&serverworkers[n]);\
	}

serverworkersAction(Start)
serverworkersAction(Stop)
serverworkersAction(Pause)
serverworkersAction(Unpause)

void serverworkersAddWork(int n, void* work){
	if (n<MAX_WORKERS)
		workerbaseAddWork(&serverworkers[n], work);
}

int serverworkersAddWorkAuto(void* work){
	int i;
	int n=0;
	int $works=serverworkers[n].$works;
	for(i=0;i<total;i++){
		if (serverworkers[i].$works<$works){
			$works=serverworkers[i].$works;
			n=i;
		}
	}
	workerbaseAddWork(&serverworkers[n], work);
	return n;
}

int serverworkersCreate(int num, int TPS){
	int i;
	memset(serverworkers, 0, sizeof(serverworkers));
	total=num;
	if (total>MAX_WORKERS){
		total=MAX_WORKERS;
		printLog("set num to %d\n",MAX_WORKERS);
	}
	for(i=0;i<total;i++){
		serverworkers[i].id=i;
		serverworkers[i].TPS=TPS;
		serverworkers[i].init=&init;
		serverworkers[i].loop=&loop;
		serverworkers[i].proceed=&proceed;
		serverworkers[i].close=&close;
		serverworkers[i].data=&data[i];
		workerbaseCreate(&serverworkers[i]);
	}
	return 0;
}