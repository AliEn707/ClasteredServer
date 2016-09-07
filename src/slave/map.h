#ifndef CLASTERED_SERVER_SLAVE_MAP_HEADER
#define CLASTERED_SERVER_SLAVE_MAP_HEADER

#include <map>
#include <queue>
#include <vector>

#include "lib/grid.h"
#include "npc.h"
#include "player.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}

namespace clasteredServerSlave {

	struct cell{
		int id;
		std::vector<npc*> npcs;
		
		npc* getNpc(int id);
		void addNpc(npc *n);
	};

	class map {
		public:
			point source;
			point size;
			point cell;
			clasteredServerSlave::cell* grid;
			
			map();
			~map();
			clasteredServerSlave::cell* cells(int id);
			clasteredServerSlave::cell* cells(float x, float y);
			void reconfigure();
	};
}

#endif


#endif
