#ifndef SYNC_HEADER
#define SYNC_HEADER

#include <sys/time.h>

int timePassed(struct timeval * t);

void syncTPS(int z, int TPS);//if TPS==0 work without delay

/*
usage:
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	
	
	while(1){
		timePassed(&tv); //start timer
		...
		syncTPS(timePassed(&tv),TPS);
	}
*/

#endif