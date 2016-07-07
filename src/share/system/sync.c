#include <string.h>
#include <time.h>
#include <unistd.h> 

#include "sync.h"
#include "log.h"

//time passed after previous call of function
int timePassed(struct timeval * t){
	//config.time  struct timeval
	struct timeval end;
	gettimeofday(&end, 0);
	int out=((end.tv_sec - t->tv_sec)*1000000+
			end.tv_usec - t->tv_usec);
	memcpy(t,&end,sizeof(end));
	return out;
}

void syncTPS(int diff,int TPS){
if (TPS)
	if((diff=(1000000/TPS)-diff)>0){
		usleep(diff);
	}
}
