#include <string.h>


#include "../share/containers/worklist.h"
#include "../share/network/listener.h"
#include "../share/system/log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	listeners container 				                       ║
║ created by Dennis Yarikov						                       ║
║ jul 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


static worklist listeners={0};

void listenersInit(){
	memset(&listeners, 0,sizeof(listeners));
}

void listenersClear(){
	worklistErase(&listeners, (void(*)(void*))listenerClear);
}

listener* listenersAdd(listener* l){
	worklistAdd(&listeners, l);
	return l;
}

