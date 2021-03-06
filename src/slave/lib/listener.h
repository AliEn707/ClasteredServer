#ifndef CLASTERED_SERVER_LISTENER_HEADER
#define CLASTERED_SERVER_LISTENER_HEADER

extern "C"{
#include <pthread.h>
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
