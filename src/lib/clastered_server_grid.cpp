#include <vector>

#include "clastered_server_grid_cpp.h"
namespace clasteredServer {
	struct server{
		int id, index;
		struct{
			float l,t,r,b;
		} area;
	};
//cpp class and code
	grid::grid(float s[2], float o){
		int i;
		offset=o;
		for (i=0;i<2;i++){
			size[i]=s[i];
			cell[i]=0;
			data_size[i]=0;
			counts[i]=0;
		}
		id=0;
	}
	
	grid::~grid(){
	}
	
	int grid::getOwner(float x, float y){
		return data[to_grid(x/cell[0],y/cell[1])].owner;
	}
	
	std::vector<int> grid::getShares(float x, float y){//return array of int of different size
		return data[to_grid(x/cell[0],y/cell[1])].shares;
	}

	void grid::reconfigure(std::vector<int> a){//take array of int different size
		reconfigure(a,0);
	}			
	void grid::reconfigure(std::vector<int> a, int id){//take array of int different size
		std::vector<server> servers;
	}
	
	int grid::to_grid(int x, int y){
		int index=y*data_size[0]+x;
		return index>0?index:0;
	}


//c bindings
#ifdef __cplusplus
extern "C" {
#endif

#include "clastered_server_grid_c.h"

#ifdef __cplusplus
}
#endif

	clastered_server_grid_t clasteredServerGridInit(float size[2], float offset) {
		return new grid(size, offset);
	}

	void clasteredServerGridDestroy(clastered_server_grid_t untyped_ptr) {
		grid* typed_ptr = static_cast<grid*>(untyped_ptr);
		delete typed_ptr;
	}

	int clasteredServerGridGetOwner(clastered_server_grid_t untyped_self, float x, float y) {
		grid* typed_self = static_cast<grid*>(untyped_self);
		return typed_self->getOwner(x, y);
	}

	int* clasteredServerGridGetShares(clastered_server_grid_t untyped_self, float x, float y,int* size) {
		grid* typed_self = static_cast<grid*>(untyped_self);
		std::vector<int> shares=typed_self->getShares(x, y);
		*size=shares.size();
		return shares.data();//need to correct
	}

	void clasteredServerGridReconfigure(clastered_server_grid_t untyped_self, int* a, short size, int id) {
		grid* typed_self = static_cast<grid*>(untyped_self);
		std::vector<int> v(a, a+size);
		typed_self->reconfigure(v, id);
	}
}
