#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
extern "C"{
#include <math.h>
#include <string.h>
}
#include "npc.h"
#include "npc/moves.h"
#include "world.h"

using namespace clasteredServer;


namespace clasteredServerSlave{
	
	npc::npc(int id, int slave, short type): id(id),  type(type), cell_id(0){
		slave_id=slave?:world::id;
		memset(&bot,0,sizeof(bot));
		memset(keys,0,sizeof(keys));
		memset(&direction,0,sizeof(direction));
		memset(&_updated,0,sizeof(_updated));
		
		attr.push_back(position.x); //0
		attr.push_back(position.y); //1
		attr.push_back(direction.x);
		attr.push_back(direction.y);
		
		for(unsigned i=0;i<attr.size();i++){
			attrs.push_back(0);
		}
		
		movef=npc::moves[type];
		timestamp=time(0);
		//TODO: add normal spawn position
		///
		position.x=10;
		position.y=10;
		vel=10;
		r=5;
	}
	
	npc::~npc(){
		player *p=world::players[owner_id];
		if (p)
			p->npc=0;
		//add returning of id
	}
		
	bool npc::clear(){
		if (time(0)-timestamp>15 && (gridOwner()!=world::id || (!bot.used && !world::players[owner_id]))){
			return 1;
		}
			
		for(unsigned i=0;i<attrs.size();i++){
			attrs[i]=0;
		}
		_updated.pack.done=0;
		_updated.pack.all=0;
		_updated.pack.server=0;
		return 0;
	}
	
	void npc::move(){
		if (movef)
			(this->*movef)(direction.x*vel, direction.y*vel);
	}
	
#define m world::map
	bool npc::update_cells(){//TODO:improve performance
		int _cell_id=m.to_grid(position.x, position.y);
		if (cell_id!=_cell_id){//if npc move to other cell
			std::map<int, bool> e;
			std::vector<int> &&v=m.near_cells(_cell_id, r);
			//set old cells to true
			for(int i=0,end=cells.size();i<end;i++){
				e[cells[i]]=1;
			}
			//set new cells to false
			for(int i=0,end=v.size();i<end;i++){
				std::map<int, bool>::iterator it = e.find(v[i]);
				if (it!=e.end())
					if (it->second)
						e[it->first]=0;
			}
			//remove npc from old
			//invert false
			std::list<int> for_del;
			for(std::map<int, bool>::iterator i=e.begin(), end=e.end();i!=end;++i){
//				printf("%d\n", i->first);
				if (i->second){
					m.cells(i->first)->npcs.erase(id);///WTF!!!
					for_del.push_back(i->first);
//					e.erase(i->first);
				}else{
					e[i->first]=1;
				}
			} 
			//remove true
			for(auto i:for_del){//!need to use!
				e.erase(i);
			}
			//add npc to new cells
			for(int i=0,end=v.size();i<end;i++){
//				printf("\t %d\n", v[i]);
				if (!e[v[i]])
					m.cells(v[i])->npcs[id]=this;
			}
			//c->npcs.erase(id);
			cell_id=_cell_id;
			cells=v;
			return 1;
		}
		return 0;
	}
#undef m
	
	void npc::set_dir(){
		if (bot.used)
			set_dir(bot.goal.x-position.x, bot.goal.y-position.y);
		else
			set_dir(keys[0], keys[1]);
	}

	void npc::set_dir(float x, float y){
		direction.x=x;
		attrs[attr(&direction.x)]=1;
		direction.y=y;
		attrs[attr(&direction.y)]=1;
		timestamp=time(0);
		direction.normalize();
	}
	
	void npc::update(packet * p){
		for(unsigned i=1;i<p->chanks.size();i++){
			int index=(int)p->chanks[i++].value.c;
			if (p->chanks[i-1].type!=1)
				printf("chank index type error, got %d\n", p->chanks[i-1].type);
//			printf("index %d\n", index);
			if (index>=0){
				if (index<(int)attrs.size()){
					if (p->chanks[i].type<6){
//						printf("sizeof chank %d\n",p->chanks[i].size());
						void* data=p->chanks[i].data();
						if (data && p->chanks[i].size()==attr.size(index))
							memcpy(attr(index), p->chanks[i].data(), p->chanks[i].size());
						else//smth wrong with server>server proxy
							printf("npc update corrupt chank on index %d %d\n", (int)index, i);
					}
				}else{
					printf("got strange index %d\n", (int)index);
				}
			}else{
				//special params
				switch(index){
					case -1:
						owner_id=p->chanks[i].value.i;
						break;
					case -2:
						keys[0]=p->chanks[i].value.c;
						break;
					case -3:
						keys[1]=p->chanks[i].value.c;
						break;
					case -4:
						keys[2]=p->chanks[i].value.c;
						break;
					case -5:
						keys[3]=p->chanks[i].value.c;
						break;					
					case -6:
						bot.used=p->chanks[i].value.c;
//						printf("%d got bot used x %d\n", id, bot.used);
						break;					
					case -7:
						bot.goal.x=p->chanks[i].value.f;
//						printf("%d got goal x %g\n",id, bot.goal.x);
						break;					
					case -8://TODO:find why it prints 2 times
						bot.goal.y=p->chanks[i].value.f;
//						printf("%d got goal y %g\n",id, bot.goal.y);
						break;
				}
			}
		}
		set_dir();
	}
	
	bool npc::updated(){
		for(unsigned i=0;i<attrs.size();i++){
			if (attrs[i]){
				return 1;
			}
		}
		return 0;
	}
	
#define packAttr(p,a)\
	if (attrs[attr(&(a))]){\
		p.add((char)attr(&(a)));\
		p.add(a);\
	}
	
	void npc::pack(bool all, bool server){
		if (!_updated.pack.done || 
				_updated.pack.server!=server || 
				_updated.pack.all!=all){
			p.init();
			p.setType(server?MSG_SERVER_NPC_UPDATE:MSG_CLIENT_NPC_UPDATE);//npc update
			p.add(id);
			packAttr(p, position.x);
			packAttr(p, position.y);
			packAttr(p, direction.x);
			packAttr(p, direction.y);
			
			p.dest.type=server?SERVER_MESSAGE:CLIENT_MESSAGE;
			if (all){
				p.add((char)-1);
				p.add(owner_id);
				_updated.pack.all=1;
			} 
			if (server){
				if (all){
					p.add((char)-6);
					p.add((char)bot.used);
				}
				if (!bot.used){
					for(int i=0;i<4;i++){
						p.add((char)(-2-i));//-2 to -5
						p.add((char)keys[i]);
					}
				}else{
					p.add((char)-7);
					p.add(bot.goal.x);
					p.add((char)-8);
					p.add(bot.goal.y);
				}
				_updated.pack.server=1;
			} 
			_updated.pack.done=1;
		}
	}
#undef packAttr
	
	int npc::gridOwner(){
		return world::grid->getOwner(position.x, position.y);
	}
	
	std::vector<int>& npc::gridShares(){
		return world::grid->getShares(position.x, position.y);		
	}
	
	int npc::addBot(float x, float y){
		npc* n=new npc(world::getId());
		n->position.x=x;
		n->position.y=y;
		n->direction.y=0.1;
		n->bot.goal.x=x;
		n->bot.goal.y=y;
		
		n->bot.used=1;
		withLock(world::m, world::npcs[n->id]=n);
		printf("added bot %d on %g, %g\n", n->id, n->position.x, n->position.y);
		return n->id;
	}
	
	bool npc::check_point(typeof(point::x) x, typeof(point::y) y){
		point p(x,y);
		std::vector<int> &&ids=world::map.near_cells(x, y, r); //!check this!
		//printf("segments %d \n", world::map.segments.size());
		for(int c=0,cend=ids.size();c<cend;c++){//TODO: change to check by map grid
			clasteredServerSlave::cell *cell=world::map.cells(ids[c]);
			for(int i=0,end=cell->segments.size();i<end;i++){//TODO: change to check by map grid
				segment *s=cell->segments[i];
				if(s->distanse(p)<=r){
					//printf("dist \n");
					return 0;
				}
			}
		}
		return 1;
	}

	void npc::init(){
		npc_moves::init();
	}
	
}
