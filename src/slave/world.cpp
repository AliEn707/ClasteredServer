
#include "lib/packet.h"
#include "world.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with sockets 			                       ║
║ created by Dennis Yarikov						                       ║
║ aug 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

using namespace clasteredServer;

namespace clasteredServerSlave {
	
	int world::id=0;
	float world::map_size[2]={320, 320};
	float world::map_offset=32;
	bool world::main_loop=0;
	clasteredServer::mutex world::m;
	clasteredServer::grid* world::grid=0;
	clasteredServer::socket* world::sock=0;
	clasteredServerSlave::map world::map(10,10);
	std::queue<int> world::ids;
	std::map<int, npc*>world:: npcs;
	std::map<int, player*> world::players;

	void world::init(){
		//read data from disk and init
		grid=new clasteredServer::grid(map_size, map_offset);
	}

	int world::getId(){
		packet p;
		int o=ids.front();
		if (ids.size()-1<10){
			p.setType(6);
			world::sock->send(&p);
		}
		ids.pop();
		return o;
	}
	
	void world::clear(){
		if (sock)
			delete sock;
		if (grid)
			delete grid;
	}
	
}
