#include <cstdio>
#include <cstdlib>
#include <math.h>

#include "map.h"
#include "world.h"
#include "NLTmxMap/NLTmxMap.h"

namespace clasteredServerSlave{

	static void* loadFile( const char * filename, bool appendNull ) {
    
		FILE* f = fopen( filename, "r" );
		if ( !f ) {
			return 0;
		}
		
		fseek( f, 0, SEEK_END );
		int length = ftell( f ) + appendNull;
		fseek( f, 0, SEEK_SET );
		
		void* buffer = malloc( length );
		fread( buffer, length, 1, f );
		fclose( f );
		
		if ( appendNull ) {
			((char*)buffer)[ length-1 ] = 0;
		}
		
		return buffer;
	}
	
	map::map(int x, int y){
		cell.x=x;
		cell.y=y;
		reconfigure();
	}
	
	map::~map(){
		if (grid)
			delete[] grid;
		clean_segments();
	}
	
	void map::clean_segments(){
		for(int i=0, size=segments.size();i<size;i++){
			delete segments[i];
		}
		segments.clear();
	}
	
	void map::reconfigure(){
		char * xml = (char*) loadFile( "data/map.tmx", true );
		if (xml){
			NLTmxMap* map = NLLoadTmxMap( xml );
			//fill data
			world::map_size[0]=map->tileWidth*map->width;
			world::map_size[1]=map->tileHeight*map->height;
			delete map;
			free(xml);
		}
		if (grid)
			delete[] grid;
		clean_segments();
		{//set main map borders
			point lt(0, 0);
			point rt(0, world::map_size[1]-1);
			point rb(world::map_size[0]-1, world::map_size[1]-1);
			point lb(world::map_size[0]-1, 0);
			segments.push_back(new segment(lt,rt));
			segments.push_back(new segment(rt,rb));
			segments.push_back(new segment(rb,lb));
			segments.push_back(new segment(lb,lt));
		}
		size.x=ceil((1.0*world::map_size[0])/cell.x);//TODO: change to local server area
		size.y=ceil((1.0*world::map_size[1])/cell.y);
		int grid_size=size.x*size.y;
		grid=new clasteredServerSlave::cell[grid_size+1];
		for(int i=0;i<grid_size;i++){
			vector<segment> borders=cell_borders(i);
//			printf("%d: ",i);
			for(int j=0,jend=segments.size();j<jend;j++){
				for(int b=0;b<4;b++){
					if (borders[b].cross(segments[j])){
						grid[i].segments.push_back(segments[j]);
//						printf("%d|%d[%g,%g %g,%g](%g,%g %g,%g) ", j, b,segments[j]->a.x,segments[j]->a.y,segments[j]->b.x,segments[j]->b.y,borders[b].a.x,borders[b].a.y,borders[b].b.x,borders[b].b.y);
						break;
					}
				}
			}
//			printf("\n");
		}
	}
	
	cell* map::cells(int id){
		return &grid[id];
	}
	
	cell* map::cells(point &p){
		return cells(to_grid(p.x, p.y));
	}
	
	cell* map::cells(float x, float y){
		return cells(to_grid(x, y));
	}
	
	int map::to_grid(float x, float y){
		return (int)(x/cell.x)+((int)(y/cell.y))*size.y;
	}
	
	std::vector<segment> map::cell_borders(int id){
		vector<segment> v;
		int x=id%size.y;
		int y=id/size.y;
		point p1(x*cell.x, y*cell.y);
		point p2(x*cell.x, y*cell.y+cell.y);
		point p3(x*cell.x+cell.x, y*cell.y+cell.y);
		point p4(x*cell.x+cell.x, y*cell.y);
		segment s1(p1, p2);
		segment s2(p2, p3);
		segment s3(p3, p4);
		segment s4(p4, p1);
		v.push_back(s1);
		v.push_back(s2);
		v.push_back(s3);
		v.push_back(s4);
		return v;
	}
}
