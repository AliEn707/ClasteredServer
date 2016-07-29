#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "workerbase.h"
#include "../system/t_mutex.h"
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
	t_mutexLock(w->mutex);
		while(!w->run){
			t_mutexUnlock(w->mutex);
			usleep(10000);
			t_mutexLock(w->mutex);
		}
	t_mutexUnlock(w->mutex);
	printLog("%s started\n",w->name);
	
	timePassed(&tv);
	t_mutexLock(w->mutex);
	while(w->run){
		t_mutexUnlock(w->mutex);
		t_mutexLock(w->mutex);
			if (w->paused){
				printLog("%s paused\n",w->name);
				paused=1;
			}
		t_mutexUnlock(w->mutex);
		while(paused){
			t_mutexLock(w->mutex);
				if (!w->paused){
					printLog("%s unpaused\n",w->name);
					paused=0;
					break;
				}
			t_mutexUnlock(w->mutex);
			usleep(10000);
			}
		t_mutexUnlock(w->mutex);
		
		w->timestamp=time(0);
		if (w->loop)
			w->loop(w);
		if (w->proceed){
			t_mutexLock(w->mutex);
				worklistForEachRemove(&w->works,*w->proceed,w);
			t_mutexUnlock(w->mutex);
		}
		if (!w->recheck){
			syncTPS(timePassed(&tv),w->TPS);
			timePassed(&tv);
		}
		w->recheck=0;
		t_mutexLock(w->mutex);
	}
	t_mutexUnlock(w->mutex);
	if (w->close)
		out=w->close(w);
	
	t_mutexLock(w->mutex);
		worklistErase(&w->works, 0);
	t_mutexUnlock(w->mutex);
	t_mutexUnlock(w->mutex);
	t_mutexRemove(w->mutex);
	printf("%s closed\n", w->name);
//	pthread_exit(out);
	return out;
}

void workerbaseStart(worker * w){
	t_mutexLock(w->mutex);
		w->run=1;
	t_mutexUnlock(w->mutex);
}

void workerbaseStop(worker * w){
	t_mutexLock(w->mutex);
		w->run=0;
	t_mutexUnlock(w->mutex);
	//pthread_join(w->pid, 0);
}

void workerbasePause(worker * w){
	t_mutexLock(w->mutex);
		w->paused=1;
	t_mutexUnlock(w->mutex);
}

void workerbaseUnpause(worker * w){
	t_mutexLock(w->mutex);
		w->paused=0;
	t_mutexUnlock(w->mutex);
}

void workerbaseAddWork(worker * w, void* obj){
	t_mutexLock(w->mutex);
		worklistAdd(&w->works, obj);
		w->$works++;
	t_mutexUnlock(w->mutex);
}

worker *workerbaseCreate(worker * w){
	if((w->mutex=t_mutexGet(1))==0){
		perror("t_mutexGet");
		return 0;
	}
	t_mutexUnlock(w->mutex);
	
	if(pthread_create(&w->pid,0,workerbaseThread,w)!=0)
		return 0;
	return w;
}
