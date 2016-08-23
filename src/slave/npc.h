#ifndef CLASTERED_SERVER_SLAVE_NPC_HEADER
#define CLASTERED_SERVER_SLAVE_NPC_HEADER

#include <map>
#include <vector>
#include "lib/mutex.h"
#include "lib/packet.h"

#ifdef __cplusplus
extern "C"{
#include <time.h>
#endif


#ifdef __cplusplus
}


namespace clasteredServerSlave {

	struct pointf{
		float x,y;
	};
	
	struct pointi{
		int x,y;
	};
	
	struct bot{
		bool used;
		pointf goal;
	};
	
	void normalize(pointf*);

	class npc {
		public:
			short state;
			int id;
			bool alive;
			pointf position;
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
			char keys[4]; //l,t,r,b
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
			pointf direction;
			std::vector<void*> attr_shift;
			std::map<void*, int> shift_attr;			
	};
}

#endif


#endif
