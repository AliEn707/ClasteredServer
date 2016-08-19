#ifndef CLASTERED_SERVER_SLAVE_MESSAGES_PROCESSOR_HEADER
#define CLASTERED_SERVER_SLAVE_MESSAGES_PROCESSOR_HEADER

#include <map>

#include "../lib/packet.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}

namespace clasteredServerSlave {
	typedef void*(*processor)(clasteredServer::packet*);
	struct processors{
		static std::map<char, processor> messages;
		static void init();
	};
}

#endif


#endif
