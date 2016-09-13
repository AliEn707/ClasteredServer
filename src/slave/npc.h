#ifndef CLASTERED_SERVER_SLAVE_NPC_HEADER
#define CLASTERED_SERVER_SLAVE_NPC_HEADER

#include <map>
#include <vector>
#include "lib/mutex.h"
#include "lib/packet.h"
#include "point.h"

#ifdef __cplusplus
extern "C"{
#include <time.h>
#endif


#ifdef __cplusplus
}


namespace clasteredServerSlave {

	struct bot{
		bool used;
		point goal;
	};

	class npc {
		public:
			short state;
			int id;
			int d;
			bool alive;
			point position;
			short health;
			clasteredServer::packet p;
			clasteredServer::mutex m;
			int owner_id;
			int slave_id;
			struct{
				struct{
					bool done;
					bool all;
					bool server;
				} pack;
			} _updated;
			char keys[4]; //x,y l-r+t-b+		l,t,r,b
			clasteredServerSlave::bot bot;
			clasteredServer::mutex access;
			std::vector<bool> attrs; //attributes flags
	
			npc(int id, int slave=0);
			~npc();
			bool clear();
			void move();
			void move(float x, float y);
			void set_dir();
			void set_dir(float x, float y);
			int attr(void *attr);//get index by pointer
			void* attr(int index);//get pointer to attribute, size is size of attribute
			void update(clasteredServer::packet * p);
			bool updated(); 			
			void pack(bool all=0, bool server=0); 
			int gridOwner();
			std::vector<int> gridShares();

			static int addBot(float x, float y);
		private:
			float vel;
			time_t timestamp;
			point direction;
			std::vector<void*> attr_shift;
			std::map<void*, int> shift_attr;			
			
			bool check_point(typeof(point::x) x, typeof(point::y) y);
	};
}

#endif


#endif
