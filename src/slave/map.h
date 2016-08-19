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
	};

	class map {
		public:
			pointf source;
			pointf size;
			cell* grid;
			
			map();
			~map();
			cell* cells(int id);
			cell* cells(float x, float y);
			void reconfigure();
	};
}

#endif


#endif
