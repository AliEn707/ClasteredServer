#include <cstdio>
#include <cstdlib>

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
		auto length = ftell( f ) + appendNull;
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
		size.x=world::map_size[0]/cell.x;//TODO: change to local server area
		size.y=world::map_size[1]/cell.y;
		grid=new clasteredServerSlave::cell[(int)(size.x*size.y)+1];
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
		return (x/cell.x)+(y/cell.y)/size.y;
	}
}
