#include <vector>
#include <map>
#include <new>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <iostream>

#include "clastered_server_grid.h"

namespace clasteredServer {
	struct server{
		int id, index;
		server_area area;
	};

	bool operator==(const data_cell &b, const data_cell &o) {
		return (b.owner == o.owner && b.shares == o.shares);
	}
	bool operator<(const data_cell &b, const data_cell &o) {
		return (b.owner < o.owner && b.shares != o.shares);
	}


//cpp class and code
	grid::grid(float s[2], float o){
		int i;
		offset=o;
		for (i=0;i<2;i++){
			size[i]=s[i];
			cell[i]=0;
			grid_size[i]=0;
		}
		id=0;
		data=NULL;
	}
	
	grid::~grid(){
		//free every cell
		if (data){
			for(std::map<data_cell, data_cell*>::const_iterator it = cells.begin(), end = cells.end(); it != end; ++it)
				delete it->second;
			delete[] data;
		}
		cells.clear();
	}
	
	int grid::getOwner(const float x, const float y){
		return data[to_grid(x/cell[0], y/cell[1])]->owner;
	}
	
	std::vector<int> grid::getShares(const float x, const float y){//return array of int of different size
		return data[to_grid(x/cell[0], y/cell[1])]->shares;
	}

	bool grid::reconfigure(const std::vector<int> &a){
		return reconfigure(a,0);
	}			
	bool grid::reconfigure(const std::vector<int> &a, const int _id){
		std::map<int, server> servers;
		int counts[2]={0};
		
		server_ids=a;
		std::sort(server_ids.begin(), server_ids.end());
		id=_id;
		//cleanup
		if (data){
			for(std::map<data_cell, data_cell*>::const_iterator it = cells.begin(), end = cells.end(); it != end; ++it)
				delete it->second;
			delete[] data;
		}
		cells.clear();
		
		for(int n=a.size();n>0;n--){
			counts[0]=1;
			int r=round(sqrt(n));
			for (int i = 1; i <= r; i++) {
			   if (n%i == 0)
				  counts[0] = i;
			}
			counts[1]=n/counts[0];
			if (size[0]>size[1]){
				int $=counts[0];
				counts[0]=counts[1];
				counts[1]=$;
			}
			if (offset<=size[0]/counts[0] && offset<=size[1]/counts[1]){
//				printf("size %d\n",n);
				break;
			}
		}
//		printf("counts %d %d\n", counts[0], counts[1]);
//		printf("area %g %g\n", size[0]/counts[0], size[1]/counts[1]);
		for(int i=0;i<2;i++){
			float l=size[i]/counts[i];
			float d=offset;
			for(int n=0;n<1000000;n++){
				float a=l/n;
				int m=ceil(d/a);
				if (d<=m*a && 1.1f*d>=m*a){
					cell[i]=a;
					break;
				}
			}
			grid_size[i]=ceil(size[i]/cell[i]);
		}
		
		for(std::vector<int>::iterator it=server_ids.begin(),end=server_ids.end();it<end;++it){
			int _id=*it;
			int i=it-server_ids.begin();
			server s;
			s.id=_id;
			s.index=i;
			s.area.l=size[0]/cell[0]/counts[0]*(i% counts[0]);
			s.area.t=size[1]/cell[1]/counts[1]*(i/ counts[0]);
			s.area.r=size[0]/cell[0]/counts[0]*(1+(i% counts[0]));
			s.area.b=size[1]/cell[1]/counts[1]*(1+(i/ counts[0]));
			servers[_id]=s;
		}
#define pushShares(o, x1,y1) \
	if (x1>=0 && x1<counts[0] && y1>=0 && y1<counts[1])\
		o.shares.push_back(server_ids[(y1)*counts[0]+x1])
//		printf("grid_size %d %d total %d\n",grid_size[0],grid_size[1],grid_size[0]*grid_size[1]);
		data=new data_cell*[grid_size[0]*grid_size[1]];
		for(int y=0;y<grid_size[1];y++){
			for(int x=0;x<grid_size[0];x++){
				server s=servers[server_ids[x/(grid_size[0]/counts[0])+y/(grid_size[1]/counts[1])*counts[0]]];
				data_cell o;
				o.owner=s.id;
				if (id==0 || s.id==id){
					int ix=s.index%counts[0];
					int iy=s.index/counts[0];
					int _offset[2]={ceil(offset/cell[0]),ceil(offset/cell[1])};
					bool l=s.area.l<=x && x<s.area.l+_offset[0];
					bool t=s.area.t<=y && y<s.area.t+_offset[1];
					bool r=s.area.r>x && x>=s.area.r-_offset[0];
					bool b=s.area.b>y && y>=s.area.b-_offset[1];
					int x1;
					int y1;
					if (l){
						x1=ix-1;
						y1=iy;
						pushShares(o, x1,y1);
						if (t){
							x1=x1;
							y1=y1-1;
							pushShares(o, x1,y1);
						}
					}
					if (t){
						x1=ix;
						y1=iy-1;
						pushShares(o, x1,y1);
						if (r){
							x1=x1+1;
							y1=y1;
							pushShares(o, x1,y1);
						}
					}
					if (r){
						x1=ix+1;
						y1=iy;
						pushShares(o, x1,y1);
						if (b){
							x1=x1;
							y1=y1+1;
							pushShares(o, x1,y1);
						}
					}
					if (b){
						x1=ix;
						y1=iy+1;
						pushShares(o, x1,y1);
						if (l){
							x1=x1-1;
							y1=y1;
							pushShares(o, x1,y1);
						}
					}
				}
				data[to_grid(x,y)]=(cells[o]?:cells[o]=(new data_cell(o)));
			}
		}
		//some work
		return !data;
	}
#undef pushShares
	
	int grid::to_grid(const int x, const int y){
		int index=y*grid_size[0]+x;
		return index>0?index:0;
	}


//c bindings

	clastered_server_grid_t clasteredServerGridInit(float size[2], float offset) {
		try{
			return new grid(size, offset);
		}catch (...){
			return 0;
		}
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
		std::vector<int> shares = typed_self->getShares(x, y);//WTF?!
		*size=shares.size();
		return shares.data();//need to check
	}

	int clasteredServerGridReconfigure(clastered_server_grid_t untyped_self, int* a, short size, int id) {
		grid* typed_self = static_cast<grid*>(untyped_self);
		std::vector<int> v(a, a+size);
		try{
			return typed_self->reconfigure(v, id);
		}catch (...){
			return 0;
		}
	}
}

/*
int main(){
	float a[2]={32000,32000};
	clasteredServer::grid g(a,20);
	std::vector<int> v;
	for(int i=1;i<10;i++){
//		printf("added %d\n",i);
		v.push_back(i);
	}
	g.reconfigure(v);
	
	printf("%d\n",g.getOwner(16123,23444));
	
	return 0;
}
*/
