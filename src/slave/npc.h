#ifndef CLASTERED_SERVER_SLAVE_NPC_HEADER
#define CLASTERED_SERVER_SLAVE_NPC_HEADER

#include <map>
#include <vector>
#include "lib/mutex.h"
#include "lib/packet.h"
#include "point.h"
#include "attrs_map.h"

extern "C"{
#include <time.h>
}


namespace clasteredServerSlave {
	class npc;
	typedef void (npc:: *move_func)(typeof(point::x) x, typeof(point::y) y);
	
	struct bot {
		bool used;
		point goal;
		int dist; //moved distance
	};
	
	class npc {
		public:
			short state;
			int id;
			short type;
			int r; //radius of collision
			bool alive;
			point position; //0,1
			short health;
			clasteredServer::packet p;
			clasteredServer::mutex m;
			int owner_id;
			int slave_id;
			int cell_id;
			std::vector<int> cells;
			struct{
				struct{
					bool done;
					bool all;
					bool server;
				} pack;
			} _updated;
			char keys[4]; //x,y(l- r+ t- b+), angle		l,t,r,b
			clasteredServerSlave::bot bot;
			attrs_map attr;
			std::vector<bool> attrs; //attributes flags
			move_func movef;

			npc(int id, int slave=0, short type=0);
			~npc();
			bool clear();
			void move();
			void set_dir();
			void set_dir(float x, float y);
			void update(clasteredServer::packet * p);
			bool updated(); 			
			void pack(bool all=0, bool server=0); 
			int gridOwner();
			std::vector<int>& gridShares();

			static std::map<short, move_func> moves;
			
			static int addBot(float x, float y);
			static void init();
			
			friend std::ostream& operator<<(std::ostream &stream, const npc &n);
		protected:
			float vel;
			time_t timestamp;
			pointf direction; //2,3
			
			bool check_point(typeof(point::x) x, typeof(point::y) y);
			bool update_cells();
	};
	
}



#endif
