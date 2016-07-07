#ifndef LISTENERS_HEADER
#define LISTENERS_HEADER

#include "../share/network/listener.h"

void listenersInit();

void listenersClear();

listener* listenersAdd(listener* l);

void listenersForEach(void*(f)(listener *l, void *arg));

#endif
