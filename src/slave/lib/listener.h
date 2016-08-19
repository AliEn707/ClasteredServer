#ifndef CLASTERED_SERVER_LISTENER_HEADER
#define CLASTERED_SERVER_LISTENER_HEADER


#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>

#ifdef __cplusplus
}
#include "socket.h"

namespace clasteredServer {

	class listener {
		private: 
			int listenerfd;
		public:
			listener(int);
			~listener();
			socket* accept();
	};
}

#endif


#endif
