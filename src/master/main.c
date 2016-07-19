#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"
#include "chat.h"
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
	char buf[700];
	while(feof(f)==0){
		fscanf(f, "%s", buf);
		if (buf[0]=='#'){
			size_t $str=400;
			char *str=malloc($str);
			if (str){
				getline(&str,&$str,f);
				free(str);
			}
		}else if (strcmp(buf, "port")==0){
			short port;
			fscanf(f, "%hd", &port);
			listener* l=listenerStart(port);
			if (l){
				listenersAdd(l);
//				printf("Listener %d added\n",l->sockfd);
			}
		}else
		if (strcmp(buf, "sw_total")==0){
			fscanf(f, "%hd", &config.serverworkers.total);
		}else
		if (strcmp(buf, "sw_tps")==0){
			fscanf(f, "%hd", &config.serverworkers.tps);
		}else
		if (strcmp(buf, "cw_total")==0){
			fscanf(f, "%hd", &config.socketworkers.total);
		}else
		if (strcmp(buf, "cw_tps")==0){
			fscanf(f, "%hd", &config.socketworkers.tps);
		}else
		if (strcmp(buf, "lw_total")==0){
			fscanf(f, "%hd", &config.listenworkers.total);
		}else
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

static void* proceedListener(listener *l, void *arg){
//	printf("added listener %d to listen workers\n", l->sockfd);
	listenworkersAddWorkAll(l);
	return 0;
}

//	FILE *f = fmemopen(&w, sizeof(w), "r+");

#define startWorkers(type)\
	type##workersCreate(config.type##workers.total,config.type##workers.tps)

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
	config.serverworkers.tps=1;
	config.socketworkers.tps=1;

	listenersInit();
	clientsInit();
	serversInit();
	
	chatsInit();
	
	readConfig();
	storageInit();
	
	clientMessageProcessorInit();
	serverMessageProcessorInit();
	
	startWorkers(listen);
	startWorkers(socket);
	startWorkers(server);
	
	listenworkersStartAll();
	socketworkersStartAll();
	serverworkersStartAll();
	
	//test
	listenersForEach(proceedListener);
//	listenworkersAddWorkAll(listenersAdd(listenerStart(8000)));
	//do some work
	main_loop=1;
//	printf("Start main loop\n");
	do{
		timePassed(&tv); //start timer
		//////test
		
		//////
		serversCheck();
		clientsCheck();
		chatsCheck();
		syncTPS(timePassed(&tv),TPS);
	}while(main_loop);
	//clearing
	sleep(2);
	//deadlock here??
	socketworkersStopAll();
	serverworkersStopAll();
	listenworkersClose();
	sleep(1);
	messageprocessorClear();
	listenersClear();
	chatsClear();
	serversClear();
	clientsClear();
	storageClear();
	printf("Exiting\n");
	sleep(1);
	return 0;
}