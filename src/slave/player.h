#ifndef CLASTERED_SERVER_SLAVE_PLAYER_HEADER
#define CLASTERED_SERVER_SLAVE_PLAYER_HEADER

#include <map>
#include <string>

#include "npc.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}

namespace clasteredServerSlave {

	class player {
		public:
			int id;
			std::string login;
			std::string pass;
			std::string name;
			clasteredServerSlave::npc* npc;
			
			player(int id);
			~player();
			void sendUpdates();
			void move();
	};
}

#endif


#endif
