#include <cstdio>
extern "C"{
}
#include "../world.h"
#include "message.h"

#define addProcessor(id)\
	messages[id]=&message ## id

using namespace clasteredServer;

namespace clasteredServerSlave {
	
	std::map<char, processor> processors::messages;
	
	static void* message2(packet* p){
		//server connected
		printf("server %d connected\n", p->chanks[0].value.i);
		world::grid->add(p->chanks[0].value.i, 1);
		return 0;
	}
	
	static void* message3(packet* p){
		//server disconnected
		printf("server %d disconnected\n", p->chanks[0].value.i);
		world::grid->remove(p->chanks[0].value.i, 1);
		return 0;
	}
	
	static void* message4(packet* p){
		//servers info
		for(unsigned i=0;i<p->chanks.size();i++){
			world::grid->add(p->chanks[i].value.i, i==p->chanks.size()-1);//reconfigure on last
		}
		return 0;
	}
	
	static void* message5(packet* p){
		//client connected
		printf("client connected %d\n", p->chanks[0].value.i);
		world::players[p->chanks[0].value.i]=new player(p->chanks[0].value.i);
		return 0;
	}
	
	static void* message6(packet* p){
		//client disconnected
		printf("client disconnected %d\n", p->chanks[0].value.i);
		player *pl=world::players[p->chanks[0].value.i];
		if (pl){
			if (pl->npc){
				pl->npc->slave_id=0;
			}
			world::players.erase(p->chanks[0].value.i);
			delete pl;
		}
		return 0;
	}
	
	static void* message9(packet* p){
		//got new id
		world::ids.push(p->chanks[0].value.i);
		printf("got new id %d\n", p->chanks[0].value.i);
		return 0;
	}
	
	static void* message10(packet* p){
		//server ready
		printf("server %d ready\n", p->chanks[0].value.i);
		world::grid->add(p->chanks[0].value.i, 1);
		//add grid update
		return 0;
	}
	
	///custom messages
	static void* message40(packet* p){
		if (p->dest.type==CLIENT_MESSAGE){
			
		}else{//npc info
			npc* n;
			withLock(world::m, n=world::npcs[p->chanks[0].value.i]);
			if (!n){
				n=new npc(p->chanks[0].value.i, p->dest.id);
				withLock(world::m, world::npcs[n->id]=n);
				packet p1;
				p1.setType(41);
				p1.add(n->id);
				p1.dest.type=SERVER_MESSAGE;
				p1.dest.id=p->dest.id;
				world::sock->send(&p1);
			}
			n->m.lock();
				n->update(p);
			n->m.unlock();
		}
		//proseed npc data
		return 0;
	}
	
	static void* message41(packet* p){
		if (p->dest.type==CLIENT_MESSAGE){//keys status
			player* pl=world::players[p->dest.id];
			if (pl && pl->npc){
				for(unsigned i=0;i<p->chanks.size();i+=2){
					pl->npc->keys[(int)p->chanks[i].value.c]=p->chanks[i+1].value.c;
				}
				pl->npc->m.lock();
					pl->npc->set_dir();
				pl->npc->m.unlock();
			}
		}else if (p->dest.type==SERVER_MESSAGE){//ask for npc info
			for(unsigned i=0;i<p->chanks.size();i++){
				npc* n;
				withLock(world::m, n=world::npcs[p->chanks[i].value.i]);
				if (n){
					n->pack(1,1);
					n->p.dest.id=p->dest.id;
					world::sock->send(&n->p);
					printf("sent info about %d\n",n->id);
				}
			}
		}
		//proseed npc data
		return 0;
	}
	
	void processors::init(){
		addProcessor(2);
		addProcessor(3);
		addProcessor(4);
		addProcessor(5);
		addProcessor(6);
		addProcessor(9);
		addProcessor(10);
		addProcessor(40);
		addProcessor(41);
	}
}