#ifndef CLASTERED_SERVER_GRID_CPP_HEADER
#define CLASTERED_SERVER_GRID_CPP_HEADER

namespace clasteredServer {
	class grid {
		struct data_cell{
			int owner;
			std::vector<int> shares;
		};
		
		private:
			int id;
			float offset;
			float cell[2];
			float size[2];
			int data_size[2];
			std::vector<int> server_ids;
			std::vector<data_cell> data;
			
			int to_grid(int x, int y);
		
		public:
			grid(float s[2], float o);
			~grid();
			int getOwner(float x, float y);
			std::vector<int> getShares(float x, float y);//return array of int of different size
			void reconfigure(std::vector<int> a);//take array of int different size
			void reconfigure(std::vector<int> a, int id);//take array of int different size
	};
}
#endif
