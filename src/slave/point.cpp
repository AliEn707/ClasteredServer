#include <math.h>

#include "world.h"
#include "point.h"

namespace clasteredServerSlave {
	
	point::point(){
		x=0;
		y=0;
	}
	
	point::point(typeof(x) _x, typeof(y) _y){
		x=_x;
		y=_y;
	}
	
	void point::normalize(){
		float l=sqrtf(sqr(x)+sqr(y));
		if (l>0){
			x/=l;
			y/=l;
		}
	}
	
	float point::distanse(point &b){
		return sqrtf(sqr(b.x-x)+sqr(b.y-y));
	}

	float point::scalar(point a,point b){
		return a.x*b.x+a.y*b.y;
	}

	point point::toVector(point &a,point &b){
		point o(b.x-a.x, b.y-a.y);
		return o;
	}

}
