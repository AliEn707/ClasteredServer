#ifndef CLASTERED_SERVER_GRID_CPP_HEADER
#define CLASTERED_SERVER_GRID_CPP_HEADER

namespace clasteredServer {
	struct data_cell{
		int owner;
		std::vector<int> shares;
	};
		
	class grid {
		private:
			int id;
			float offset;
			float cell[2]; //size of cell in units
			float size[2]; //size of map in units
			int grid_size[2]; //size of grid in cells
			data_cell** data; //grid
			std::vector<int> server_ids; //ids of server, sorted
			std::map<data_cell, data_cell*> cells; //all cells used in grid
			
			int to_grid(int x, int y);
		
		public:
			grid(float s[2], float o);
			~grid();
			int getOwner(const float x, const float y);
			std::vector<int> getShares(const float x, const float y);//return array of int of different size
			bool reconfigure(const std::vector<int> &a);//take array of int different size
			bool reconfigure(const std::vector<int> &a, const int id);//take array of int different size
	};
}
#endif
