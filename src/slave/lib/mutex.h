#ifndef CLASTERED_SERVER_MUTEX_HEADER
#define CLASTERED_SERVER_MUTEX_HEADER

#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>

#ifdef __cplusplus
}

namespace clasteredServer {

	class mutex {
		private: 
			pthread_mutex_t m;
		public:
			mutex();
			~mutex();
			void lock();
			void unlock();
	};
}

#define withLock(m,s) ({\
		typeof(s) o;\
		m.lock();\
		o=s;\
		m.unlock();\
		o;\
	})

#endif


#endif
