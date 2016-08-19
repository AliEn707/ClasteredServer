#ifndef CLASTERED_SERVER_SYNC_HEADER
#define CLASTERED_SERVER_SYNC_HEADER

#ifdef __cplusplus
extern "C"{

#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

}

namespace clasteredServer {
	class sync{
		public:
			sync();
			int timePassed();
			void syncTPS(int TPS);
		private:
			struct timeval t;
	};
}

#endif

#endif
