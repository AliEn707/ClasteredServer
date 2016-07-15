#ifndef SERVERWORKERS_HEADER
#define SERVERWORKERS_HEADER

#include "../../share/network/packet.h"

typedef 
struct {
	//some worker params
	packet packet;
	short checks;
} serverworker_data;

typedef 
struct {
	short $data;
	char data[PACKET_SIZE];
} serverworker_work;

void serverworkersStartAll();
void serverworkersStopAll();
void serverworkersPauseAll();
void serverworkersUnpauseAll();

void serverworkersStart(int n);
void serverworkersStop(int n);
void serverworkersPause(int n);
void serverworkersUnpause(int n);

void serverworkersAddWork(int n, void* work);
int serverworkersAddWorkAuto(void* work); //add work to the most free worker

int serverworkersCreate(int num, int TPS);


#endif