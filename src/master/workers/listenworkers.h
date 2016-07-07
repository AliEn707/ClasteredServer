#ifndef LISTENWORKERS_HEADER
#define LISTENWORKERS_HEADER
#include <sys/time.h>

#include "../../share/network/packet.h"

typedef 
struct {
	//some worker params
	packet packet;
	int maxfd;
	fd_set set;
} listenworker_data;

typedef 
struct {
	short $data;
	char data[PACKET_SIZE];
} listenworker_work;

void listenworkersStartAll();
void listenworkersStopAll();
void listenworkersPauseAll();
void listenworkersUnpauseAll();

void listenworkersStart(int n);
void listenworkersStop(int n);
void listenworkersPause(int n);
void listenworkersUnpause(int n);

//add work to current worker
void listenworkersAddWork(int n, void* work);
//add work to the less busy worker
int listenworkersAddWorkAuto(void* work); 
void listenworkersAddWorkAll(void* work); 

//create num of workers, that works not more than TPS tiks per second, if 0 - without delay
int listenworkersCreate(int num, int TPS);

void listenworkersClose();

#endif