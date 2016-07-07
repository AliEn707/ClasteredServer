#ifndef WORKERBASE_HEADER
#define WORKERBASE_HEADER

#include "../system/t_sem.h"
#include "../containers/worklist.h"

typedef
struct worker{
	//static vars
	int id;
	char name[40];
	int TPS;
	pthread_t pid;
	//dynamic vars
	time_t timestamp;
	short run;
	short paused;
	int $works;
	//inner vars
	worklist works;
	t_sem_t sem;
	//functions
	void* (*proceed)(void*data,void*); //must clear data before nonzero return
	void (*loop)(void*);
	void (*init)(void*);
	void *(*close)(void*);
	//custom data struct
	void* data;
} worker;

void workerbaseUnpause(worker * data);
void workerbasePause(worker * data);
void workerbaseStop(worker * data);
void workerbaseStart(worker * data);
void workerbaseAddWork(worker * data, void* obj);
worker *workerbaseCreate(worker * data);

#endif