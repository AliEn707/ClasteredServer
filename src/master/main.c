#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"
#include "client.h"
#include "server.h"
#include "listeners.h"
#include "storage.h"
#include "messages/client.h"
#include "messages/server.h"
#include "messageprocessor.h"
#include "workers/socketworkers.h"
#include "workers/serverworkers.h"
#include "workers/listenworkers.h"
#include "../share/containers/bintree.h"
#include "../share/network/bytes_order.h"
#include "../share/network/listener.h"
#include "../share/network/packet.h"
#include "../share/system/sync.h"
#include "../share/system/log.h"

#define CONFIG_FILE "config.cfg"

int main_loop;

storage_config* mainStorageConfig(){
	return &config.storage;
}

log_config* mainLogConfig(){
	return &config.log;
}

static int readConfig(){
	FILE* f;
	if ((f=fopen(CONFIG_FILE,"rt"))==0){
		printf("cant open %s, using defaults\n",CONFIG_FILE);
		sprintf(config.storage.file, "%s", "storage.txt");
		config.log.debug=1;
		return 1;
	}
	while(feof(f)==0){
		char buf[100];
		fscanf(f, "%s", buf);
		if (strcmp(buf, "storage_config")==0){
			fscanf(f, "%s", config.storage.file);
		}else
		if (strcmp(buf, "log_file")==0){
			fscanf(f, "%s", config.log.file);
		}else
		if (strcmp(buf, "log_debug")==0){
			fscanf(f, "%hd", &config.log.debug);
		}
	}
	fclose(f);
	return 0;
}

static void default_sigaction(int signal, siginfo_t *si, void *arg){
	printf("Stopping\n");
	main_loop=0;
}

//	FILE *f = fmemopen(&w, sizeof(w), "r+");

int main(int argc,char* argv[]){
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = default_sigaction;
	sa.sa_flags   = SA_SIGINFO;

	//sigaction(SIGSEGV, &sa, NULL);	
	sigaction(SIGINT, &sa, NULL);	
	sigaction(SIGTERM, &sa, NULL);	
	
	srand(time(0));
	
	memset(&config,0,sizeof(config));
	readConfig();
	storageInit();
	
	clientsInit();
	serversInit();
	listenersInit();
	clientMessageProcessorInit();
	serverMessageProcessorInit();

	listenworkersCreate(1, 0);
	socketworkersCreate(1, 4);
	serverworkersCreate(2, 10);
	
	listenworkersStartAll();
	socketworkersStartAll();
	serverworkersStartAll();
	
	//test
	listener* l=listenerStart(8000);
	if (!l)
		return 1;
	listenersAdd(l);
	printf("%d\n",l);
	listenworkersAddWorkAll(l);
//	listenworkersAddWorkAll(listenersAdd(listenerStart(8000)));
	//do some work
	main_loop=1;
	do{
		timePassed(&tv); //start timer
		//////test
		
		//////
		serversCheck();
		clientsCheck();
		syncTPS(timePassed(&tv),TPS);
	}while(main_loop);
	//clearing
	listenworkersClose();
	socketworkersStopAll();
	serverworkersStopAll();
	sleep(1);
	messageprocessorClear();
	listenersClear();
	serversClear();
	clientsClear();
	storageClear();
	printf("Exiting\n");
	return 0;
}