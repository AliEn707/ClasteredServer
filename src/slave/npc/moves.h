#ifndef CLASTERED_SERVER_SLAVE_NPC_MOVES_HEADER
#define CLASTERED_SERVER_SLAVE_NPC_MOVES_HEADER

#include "../npc.h"

namespace clasteredServerSlave{
	class npc;
	
	class npc_moves: npc{
		public:
			static void init();
		private:
			void move0(typeof(point::x) x, typeof(point::y) y);
			void move1(typeof(point::x) x, typeof(point::y) y);
	};
}	

#endif
