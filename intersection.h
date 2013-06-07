#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "vec.h"
#include "constant.h"

namespace edupt {

struct Hitpoint {
	double distance_;
	Vec normal_;
	Vec position_;

	Hitpoint() : distance_(kINF), normal_(), position_() {}
};

struct Intersection {
	Hitpoint hitpoint_;
	int object_id_;

	Intersection() : object_id_(-1) {}
};

};

#endif
