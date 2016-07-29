#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include<errno.h>

//#define t_sem
#include "t_mutex.h"
#include "log.h"


t_mutex_t t_mutexGet(){
	t_mutex_t m;
	if ((m=malloc(sizeof(*m)))==0){
		perror("malloc");
		return 0;
	}
	if (pthread_mutex_init(m, 0)!=0){
		perror("pthread_mutex_init");
		free(m);
		return 0;
		//add error show
	}
	return m;
}

void t_mutexRemove(t_mutex_t m){
	int o;
	if((o=pthread_mutex_destroy(m))!=0){
		if (o==EBUSY)
			perror("remover locked mutex");
		else
			perror("pthread_mutex_destroy");
	}
	free(m);
}
