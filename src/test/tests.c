#include <string.h>
#include <stdio.h>

#include "../share/containers/bintree.h"
#include "../share/system/log.h"
#include "../master/client.h"
#include "../master/chat.h"

struct {
	log_config log;
	short run;
} config;

log_config* mainLogConfig(){
	return &config.log;
}

void perror_test(){
	perror("test");
}

void bintree_test(){
	#define MAX_KEY 2000000
	bintree r={0};
	long long int i;
	float q=0;
	int p=100;
	int keys=MAX_KEY/50;
	printf("size: %d\n", (int)bintreeSize(&r));
	for(i=MAX_KEY-keys;i<MAX_KEY;i++){
		bintreeAdd(&r,i,(void*)(long)i);
		if (i%((int)(keys/p))==0){
			printf("%g%%. %lld\n",q, i);
			q+=100.0/p;
		}
	}
	printf("size: %d\n", (int)bintreeSize(&r));
	bintreeErase(&r,0);
//	bintreeForEach(&r,removeE,&r);
//	bintreeForEach(&r,removeE,&r);
	printf("size: %d\n", (int)bintreeSize(&r));
}


int main(){
	memset(&config,0,sizeof(config));
	config.log.debug=1;
	
	bintree_test();
	perror_test();
}