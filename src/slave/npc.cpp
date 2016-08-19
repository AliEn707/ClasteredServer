#include <cstdio>
#include <cstdlib>
extern "C"{
#include <math.h>
#include <string.h>
}
#include "npc.h"
#include "world.h"

using namespace clasteredServer;

namespace clasteredServerSlave{

	void normalize(pointf* p){
		float l=sqrtf(p->x*p->x+p->y*p->y);
		if (l>0){
			p->x/=l;
			p->y/=l;
		}
	}
	
	npc::npc(int _id, int slave){
		id=_id;
		slave_id=slave?:world::id;
		attr_shift.push_back(&position.x); //0
		attr_shift.push_back(&position.y); //1
		attr_shift.push_back(&direction.x);
		attr_shift.push_back(&direction.y);
		
		for(unsigned i=0;i<attr_shift.size();i++){
			shift_attr[attr_shift[i]]=i;
			attrs.push_back(0);
		}
		///
		vel=1;
	}
	
	void npc::clear(){
		for(std::vector<bool>::iterator it = attrs.begin(), end = attrs.end();it != end; ++it){
			*it=0;
		}
		_updated.pack.done=0;
		_updated.pack.all=0;
		_updated.pack.server=0;
	}
	
	void npc::move(){
		if (direction.x || direction.y)
			move(direction.x*vel, direction.y*vel);
	}
	
	void npc::move(float x, float y){
		position.x+=x;
		attrs[attr(&position.x)]=1;
		position.y+=y;
		attrs[attr(&position.y)]=1;
		if (bot.used){
			if (sqr(position.x-bot.goal.x)+sqr(position.y-bot.goal.y)<=2*vel){
				bot.goal.x=(rand()%1000)/10.0;
				bot.goal.y=(rand()%1000)/10.0;
				set_dir();
			}
		}
	}
	
	void npc::set_dir(){
		if (bot.used)
			set_dir(bot.goal.x-position.x, bot.goal.y-position.y);
		else
			set_dir(keys[2]-keys[0], keys[3]-keys[1]);
	}

	void npc::set_dir(float x, float y){
		direction.x=x;
		direction.y=y;
		timestamp=time(0);
		normalize(&direction);
	}
	
	int npc::attr(void *attr){
		return shift_attr[attr];
	}
	
	void* npc::attr(int index){
		return attr_shift[index];
	}
	
	void npc::update(packet * p){
		for(unsigned i=1;i<p->chanks.size();i++){
			int index=(int)p->chanks[i++].value.c;
//			printf("index %d\n", index);
			if (index>=0){
				if (p->chanks[i].type<6){
//					printf("sizeof chank %d\n",p->chanks[i].size());
					void* data=p->chanks[i].data();
					if (data)
						memcpy(attr(index), p->chanks[i].data(), p->chanks[i].size());
					else
						printf("npc update corrupt chank on index %d %d\n", (int)index, i);
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
					
				}
				set_dir();
			}
		}
	}
	
	bool npc::updated(){
		for(unsigned i;i<attrs.size();i++)
			if (attrs[i]){
				return 1;
			}
		return 0;
	}
	
#define packAttr(p,a)\
	if (attrs[attr(&(a))]){\
		p.add((char)attr(&(a)));\
		m.lock();\
			p.add(a);\
		m.unlock();\
	}
	//includes locks
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
				m.lock();
					p.add(owner_id);
				m.unlock();
				_updated.pack.all=1;
			} 
			if (server){
				m.lock();
				for(int i=0;i<4;i++){
					p.add((char)(-2-i));
					p.add((char)keys[i]);
				}
				m.unlock();
				_updated.pack.server=1;
			} 
			_updated.pack.done=1;
		}
	}
#undef packAttr
	
	int npc::addBot(float x, float y){
		npc* n=new npc(world::getId());
		n->position.x=x;
		n->position.y=y;
		n->set_dir(rand()%100, rand()%100);
		
		n->bot.goal.x=n->position.x+n->direction.x*n->vel;
		n->bot.goal.y=n->position.y+n->direction.y*n->vel;
		n->bot.used=1;
		world::npcs[n->id]=n;
		return n->id;
	}
}
