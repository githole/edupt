#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "vec.h"
#include "constant.h"

namespace edupt {

struct Hitpoint {
	double t;
	Vec normal;
	Vec position;

	Hitpoint() : t(0.0), normal(), position() {}
};

struct Intersection {
	Hitpoint hitpoint;
	int object_id;

	Intersection() : object_id(-1) {}
};

};

#endif