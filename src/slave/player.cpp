#include <cstdio>
#include <map>
#include <math.h>

#include "player.h"
#include "npc.h"
#include "world.h"

using namespace clasteredServer;

namespace clasteredServerSlave{

	player::player(int _id){
		id=_id;
		for(std::map<int, clasteredServerSlave::npc*>::iterator it = world::npcs.begin(), end = world::npcs.end();it != end; ++it){
			clasteredServerSlave::npc* n=it->second;
			if (n && n->owner_id==id){
				npc=n;
				break;
			}
		}
		if (!npc){
			npc=new clasteredServerSlave::npc(world::getId());
			npc->owner_id=id;
			printf("created npc %d for user %d\n", npc->id, id);
			world::m.lock();
				world::npcs[npc->id]=npc;
			world::m.unlock();
			packet p;
			p.dest.type=CLIENT_MESSAGE;
			p.dest.id=id;
			p.setType(MSG_CLIENT_UPDATE);
			p.add((char)1);
			p.add(npc->id);
			world::sock->send(&p);
		}
		npc->slave_id=world::id;
	}

	player::~player(){
//		if (npc){
//			world::npcs.erase(npc->id);
//			delete npc;
//		}
	}		
	
	void player::sendUpdates(){
		for(std::map<int, clasteredServerSlave::npc*>::iterator it = world::npcs.begin(), end = world::npcs.end();it != end; ++it){
			clasteredServerSlave::npc* n=it->second;
			if (n)
				if (withLock(n->m,n->updated())){
					n->pack();
					n->p.dest.id=id;
					world::sock->send(&n->p);
				}
		}
	}
	
	void player::move(){
		int oid=world::grid->getOwner(npc->position.x, npc->position.y);
		if (world::id!=oid){
			packet p;
			p.setType(MSG_MASTER_MOVE_CLIENT);//move client
			p.add(id);
			p.add(oid);
			world::sock->send(&p);
		}
	}
}
