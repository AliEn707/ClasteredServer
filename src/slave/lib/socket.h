#ifndef CLASTERED_SERVER_SOCKET_HEADER
#define CLASTERED_SERVER_SOCKET_HEADER

#include "packet.h"
#include "mutex.h"

extern "C"{
}

namespace clasteredServer {

	class socket {
		private: 
			int sockfd;
			struct{
				mutex write;
				mutex read;
			} mutex;
		public:
			socket(int);
			~socket();
			void flush();
			int send(void*,int);
			int send(packet*);
			int recv(void*, int);
			int recv(packet*);
			void lockRead();
			void unlockRead();
			void lockWrite();
			void unlockWrite();
			static socket* connect(char*, int);
	};
}


#endif
