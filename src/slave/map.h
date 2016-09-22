#ifndef CLASTERED_SERVER_SLAVE_MAP_HEADER
#define CLASTERED_SERVER_SLAVE_MAP_HEADER

#include <map>
#include <queue>
#include <vector>

#include "lib/grid.h"
#include "npc.h"
#include "player.h"
#include "segment.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}

namespace clasteredServerSlave {

	struct cell{
		int id;
		std::vector<npc*> npcs;
		std::vector<segment*> segments;
		npc* getNpc(int id);
		void addNpc(npc *n);
	};

	class map {
		public:
			point source;
			pointi size;
			point cell;
			clasteredServerSlave::cell *grid=0;
			std::vector<segment*> segments;
			
			map(int x, int y);
			~map();
			clasteredServerSlave::cell* cells(int id);
			clasteredServerSlave::cell* cells(float x, float y);
			clasteredServerSlave::cell* cells(point &p);
			int to_grid(float x, float y);
			void reconfigure();
			std::vector<segment> cell_borders(int id);
		
		private:
			void clean_segments();
	};
}

#endif


#endif
