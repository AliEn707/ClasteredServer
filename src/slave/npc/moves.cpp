#include <cstdlib>

#include "moves.h"
#include "../npc.h"
#include "../world.h"

namespace clasteredServerSlave{
	
	std::map<short, move_func> npc::moves;

#define addMoveFunc(id)\
	moves[id]=(move_func)&npc_moves::move ## id;
	
	void npc_moves::init(){
		addMoveFunc(0);
		addMoveFunc(1);
	}
#undef addMoveFunc
	
	//common move
	void npc_moves::move0(typeof(point::x) x, typeof(point::y) y){
		if (x!=0)
			if (check_point(position.x+x,position.y)){
				position.x+=x;
				attrs[attr(&position.x)]=1;
			}
		if (y!=0)
			if (check_point(position.x,position.y+y)){
				position.y+=y;
				attrs[attr(&position.y)]=1;
			}
		
		if (bot.used){
//			printf("bot %d on %g %g -> %g %g\n", id, position.x, position.y, bot.goal.x, bot.goal.y);
			if (position.distanse2(bot.goal)<=3*vel){
				bot.goal.x=(rand()%(((int)world::map_size[0]-20)*100))/100.0+10;
				bot.goal.y=(rand()%(((int)world::map_size[1]-20)*100))/100.0+10;
//				printf("new goal on %d -> %g %g\n", id, bot.goal.x, bot.goal.y);
				set_dir();
			}
		}
		update_cells();
	}
	
	//bullet move, go straight for dist, enemy or wall, than suicide
	void npc_moves::move1(typeof(point::x) x, typeof(point::y) y){
		if (x!=0 && y!=0){
			if (check_point(position.x+x,position.y+y)){
				position.x+=x;
				position.y+=y;
				bot.dist+=vel;
				attrs[attr(&position.x)]=1;
			} else {
				//TODO: suicide
			}
		}
		
		if (bot.used){
			//TODO: add check for touch enemy and suicude if need
		}
		//dont need to update cells
	}
}
