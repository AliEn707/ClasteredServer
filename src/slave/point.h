#ifndef CLASTERED_SERVER_SLAVE_POINT_HEADER
#define CLASTERED_SERVER_SLAVE_POINT_HEADER

namespace clasteredServerSlave {
	class point{
		public:
			float x,y;
			
			point();
			point(typeof(x) _x, typeof(y) _y);
			void normalize();
			float distanse(point &b);
			float distanse2(point &b);
			
			static float scalar(point a, point b);
			static point toVector(point &a, point &b);

			friend std::ostream& operator<<(std::ostream &stream, const point &p);
	};
	
	struct pointi{
		int x,y;
	};
}
#endif