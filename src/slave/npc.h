#ifndef CLASTERED_SERVER_SLAVE_NPC_HEADER
#define CLASTERED_SERVER_SLAVE_NPC_HEADER

#include <map>
#include <vector>
#include "lib/mutex.h"
#include "lib/packet.h"
#include "point.h"

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
			point position;
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
			char keys[4]; //x,y l- r+ t- b+		l,t,r,b
			clasteredServerSlave::bot bot;
			std::vector<bool> attrs; //attributes flags
			move_func movef;

			npc(int id, int slave=0, short type=0);
			~npc();
			bool clear();
			void move();
			void set_dir();
			void set_dir(float x, float y);
			int attr(void *attr);//get index by pointer
			void* attr(int index);//get pointer to attribute, size is size of attribute
			void update(clasteredServer::packet * p);
			bool updated(); 			
			void pack(bool all=0, bool server=0); 
			int gridOwner();
			std::vector<int> gridShares();

			static std::map<short, move_func> moves;
			
			static int addBot(float x, float y);
			static void init();
			static void moves_init();
			
			friend std::ostream& operator<<(std::ostream &stream, const npc &n);
		private:
			float vel;
			time_t timestamp;
			point direction;
			std::vector<void*> attr_shift;
			std::map<void*, int> shift_attr;			
			
			bool check_point(typeof(point::x) x, typeof(point::y) y);
			bool update_cells();
			void move0(typeof(point::x) x, typeof(point::y) y);
			void move1(typeof(point::x) x, typeof(point::y) y);
	};
}



#endif
