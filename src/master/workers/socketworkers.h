#ifndef SOCKETWORKERS_HEADER
#define SOCKETWORKERS_HEADER

#include "../../share/network/packet.h"

typedef 
struct {
	//some worker params
	char buf[PACKET_SIZE];
	packet packet;
} soketworker_data;


void socketworkersStartAll();
void socketworkersStopAll();
void socketworkersPauseAll();
void socketworkersUnpauseAll();

void socketworkersStart(int n);
void socketworkersStop(int n);
void socketworkersPause(int n);
void socketworkersUnpause(int n);

void socketworkersAddWork(int n, void* work);
int socketworkersAddWorkAuto(void* work); //add work to the most free worker

int socketworkersCreate(int num, int TPS);


#endif