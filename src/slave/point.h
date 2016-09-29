#ifndef CLASTERED_SERVER_SLAVE_POINT_HEADER
#define CLASTERED_SERVER_SLAVE_POINT_HEADER

namespace clasteredServerSlave {
	template <class T>
		class point_{
			public:
				T x,y;
				
				point_();
				point_(T _x, T _y);
				void normalize();
				template<class T1>
					float distanse(point_<T1> &b);
				template<class T1>
					T distanse2(point_<T1> &b);

				template<class T1, class T2>
					static T scalar(point_<T1> &&a, point_<T2> &&b);
				template<class T1, class T2>
					static point_<T> toVector(point_<T1> &a, point_<T2> &b);

		};
	
	template <class T>
		std::ostream& operator<<(std::ostream &stream, const point_<T> &p);
	
	typedef point_<float> point;
	
	typedef point_<short> points;
	typedef point_<int> pointi;
	typedef point_<float> poinf;
	
}

#include "point_definition.h"

#endif