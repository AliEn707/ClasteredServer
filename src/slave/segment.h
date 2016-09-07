#ifndef CLASTERED_SERVER_SLAVE_SEGMENT_HEADER
#define CLASTERED_SERVER_SLAVE_SEGMENT_HEADER

#include "point.h"

namespace clasteredServerSlave {
	class segment{
		public:
			point a,b;
			
			segment();
			segment(point _a, point _b);
			float distanse(point &p);
			float length();
	};
	
}

#endif
