#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "workerbase.h"
#include "../system/t_sem.h"
#include "../system/sync.h"
#include "../system/log.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	     		                  ║
║ created by Dennis Yarikov						                   			    				    ║
║ sep 2014									           										            ║
╚══════════════════════════════════════════════════════════════╝
*/

static void * workerbaseThread(void * arg){
	worker * w=arg;
	struct timeval tv={0,0};
	short paused=0;
	void* out=0;
	
	if (w->init)
		w->init(w);
	///set thread name
	//pthread_setname_np(w->pid, w->name);
	t_semSet(w->sem,0,-1);
		while(!w->run){
			t_semSet(w->sem,0,1);
			usleep(10000);
			t_semSet(w->sem,0,-1);
		}
	t_semSet(w->sem,0,1);
	printLog("%s started\n",w->name);
	
	t_semSet(w->sem,0,-1);
	while(w->run){
		timePassed(&tv);
		t_semSet(w->sem,0,1);
		t_semSet(w->sem,0,-1);
			if (w->paused){
				printLog("%s paused\n",w->name);
				paused=1;
			}
		t_semSet(w->sem,0,1);
		while(paused){
			t_semSet(w->sem,0,-1);
				if (!w->paused){
					printLog("%s unpaused\n",w->name);
					paused=0;
					break;
				}
			t_semSet(w->sem,0,1);
			usleep(10000);
			}
		t_semSet(w->sem,0,1);
		
		w->timestamp=time(0);
		if (w->loop)
			w->loop(w);
		if (w->proceed){
			t_semSet(w->sem,0,-1);
				worklistForEachRemove(&w->works,*w->proceed,w);
			t_semSet(w->sem,0,1);
		}
		syncTPS(timePassed(&tv),w->TPS);
		t_semSet(w->sem,0,-1);
	}
	t_semSet(w->sem,0,1);
	if (w->close)
		out=w->close(w);
	
	t_semRemove(w->sem);
	worklistErase(&w->works, 0);
	printf("%s closed\n", w->name);
	pthread_exit(out);
	return out;
}

void workerbaseStart(worker * w){
	t_semSet(w->sem,0,-1);
		w->run=1;
	t_semSet(w->sem,0,1);
}

void workerbaseStop(worker * w){
	t_semSet(w->sem,0,-1);
		w->run=0;
	t_semSet(w->sem,0,1);
	//pthread_join(w->pid, 0);
}

void workerbasePause(worker * w){
	t_semSet(w->sem,0,-1);
		w->paused=1;
	t_semSet(w->sem,0,1);
}

void workerbaseUnpause(worker * w){
	t_semSet(w->sem,0,-1);
		w->paused=0;
	t_semSet(w->sem,0,1);
}

void workerbaseAddWork(worker * w, void* obj){
	t_semSet(w->sem,0,-1);
		worklistAdd(&w->works, obj);
		w->$works++;
	t_semSet(w->sem,0,1);
}

worker *workerbaseCreate(worker * w){
	if((w->sem=t_semGet(1))==0){
		perror("t_semGet");
		return 0;
	}
	t_semSet(w->sem,0,1);
	
	if(pthread_create(&w->pid,0,workerbaseThread,w)!=0)
		return 0;
	return w;
}
