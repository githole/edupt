#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "vec.h"
#include "constant.h"

namespace edupt {

struct Hitpoint {
	double distance;
	Vec normal;
	Vec position;

	Hitpoint() : distance(kINF), normal(), position() {}
};

struct Intersection {
	Hitpoint hitpoint;
	int object_id;

	Intersection() : object_id(-1) {}
};

};

#endif
