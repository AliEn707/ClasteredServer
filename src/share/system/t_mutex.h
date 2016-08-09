#ifndef T_MUTEX_HEADER
#define T_MUTEX_HEADER

#include <pthread.h>
#include <stdlib.h>

#include "../system/log.h"


typedef pthread_mutex_t *t_mutex_t;

t_mutex_t t_mutexGet();

void t_mutexRemove(t_mutex_t mutex);

#define t_mutexLock(m) pthread_mutex_lock(m)
#define t_mutexUnlock(m) pthread_mutex_unlock(m)

#define t_mutexCritical(mutex, action)\
	do{\
		t_mutexLock(mutex);\
			action;\
		t_mutexUnlock(mutex);\
	}while(0)

#define t_mutexCriticalAuto(mutex, action) ({\
		typeof(action) _$_o;\
		t_mutexLock(mutex);\
			_$_o=action;\
		t_mutexUnlock(mutex);\
		_$_o;\
	})

#define t_mutexCriticalInt(mutex, action) ({\
		int _$_o;\
		t_mutexLock(mutex);\
			_$_o=action;\
		t_mutexUnlock(mutex);\
		_$_o;\
	})

#endif